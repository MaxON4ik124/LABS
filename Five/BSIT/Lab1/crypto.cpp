#include "crypto.h"
#include <array>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <vector>


using namespace std;

namespace
{
void check(NTSTATUS status)
{
    if (status < 0)
    {
        throw runtime_error("Cryptographic operation failed");
    }
}

template <typename T, auto Destroy> struct Handle
{
    T value = nullptr;
    ~Handle()
    {
        if (value)
        {
            Destroy(value);
        }
    }
};

PUCHAR bytes(const string &value)
{
    return reinterpret_cast<PUCHAR>(const_cast<char *>(value.data()));
}

void appendNumber(string &target, uint64_t value, unsigned count)
{
    for (unsigned i = count; i > 0; --i)
    {
        target.push_back(static_cast<char>(value >> ((i - 1) * 8)));
    }
}

uint64_t readNumber(const char *source, unsigned count)
{
    uint64_t value = 0;
    for (unsigned i = 0; i < count; ++i)
    {
        value = (value << 8) | static_cast<unsigned char>(source[i]);
    }
    return value;
}

void derive(BCRYPT_SECRET_HANDLE secret, HCRYPTPROV provider, const string &label, const string &transcript,
            HCRYPTKEY *key)
{
    BCryptBuffer buffers[] = {
        {sizeof(BCRYPT_SHA256_ALGORITHM), KDF_HASH_ALGORITHM, const_cast<wchar_t *>(BCRYPT_SHA256_ALGORITHM)},
        {static_cast<ULONG>(label.size()), KDF_SECRET_PREPEND, bytes(label)},
        {static_cast<ULONG>(transcript.size()), KDF_SECRET_APPEND, bytes(transcript)}};
    BCryptBufferDesc parameters{BCRYPTBUFFER_VERSION, 3, buffers};
    array<UCHAR, 32> material{};
    ULONG written = 0;
    NTSTATUS status =
        BCryptDeriveKey(secret, BCRYPT_KDF_HASH, &parameters, material.data(), material.size(), &written, 0);
    if (status < 0 || written != material.size())
    {
        SecureZeroMemory(material.data(), material.size());
        throw runtime_error("Key derivation failed");
    }
    struct KeyBlob
    {
        BLOBHEADER header;
        DWORD size;
        array<BYTE, 32> key;
    } blob{{PLAINTEXTKEYBLOB, CUR_BLOB_VERSION, 0, CALG_AES_256}, 32, material};
    const BOOL imported = CryptImportKey(provider, reinterpret_cast<BYTE *>(&blob), sizeof(blob), 0, 0, key);
    SecureZeroMemory(&blob, sizeof(blob));
    SecureZeroMemory(material.data(), material.size());
    if (!imported)
    {
        throw runtime_error("CryptImportKey failed");
    }
    DWORD mode = CRYPT_MODE_ECB;
    if (!CryptSetKeyParam(*key, KP_MODE, reinterpret_cast<BYTE *>(&mode), 0))
    {
        throw runtime_error("CryptSetKeyParam failed");
    }
}
}

CryptoSession::~CryptoSession()
{
    if (ownKey)
    {
        BCryptDestroyKey(ownKey);
    }
    if (ecdh)
    {
        BCryptCloseAlgorithmProvider(ecdh, 0);
    }
    if (receiveKey)
    {
        CryptDestroyKey(receiveKey);
    }
    if (sendKey)
    {
        CryptDestroyKey(sendKey);
    }
    if (provider)
    {
        CryptReleaseContext(provider, 0);
    }
}

bool CryptoSession::ready() const
{
    return receiveKey && sendKey;
}

string CryptoSession::createHello()
{
    if (ecdh || provider)
    {
        throw runtime_error("Handshake already started");
    }
    check(BCryptOpenAlgorithmProvider(&ecdh, BCRYPT_ECDH_P256_ALGORITHM, nullptr, 0));
    check(BCryptGenerateKeyPair(ecdh, &ownKey, 256, 0));
    check(BCryptFinalizeKeyPair(ownKey, 0));
    ownHello.assign(HelloSize, '\0');
    ownHello.replace(0, 4, "LBS1");
    ULONG written = 0;
    check(BCryptExportKey(ownKey, nullptr, BCRYPT_ECCPUBLIC_BLOB, bytes(ownHello) + 4, 72, &written, 0));
    if (written != 72)
    {
        throw runtime_error("Invalid public key size");
    }
    return ownHello;
}

void CryptoSession::agree(const string &hello)
{
    if (!ownKey || provider || hello.size() != HelloSize || hello.substr(0, 4) != "LBS1")
    {
        throw runtime_error("Invalid handshake");
    }
    BCRYPT_ECCKEY_BLOB header{};
    memcpy(&header, hello.data() + 4, sizeof(header));
    if (header.dwMagic != BCRYPT_ECDH_PUBLIC_P256_MAGIC || header.cbKey != 32)
    {
        throw runtime_error("Expected an ECDH P-256 public key");
    }
    Handle<BCRYPT_KEY_HANDLE, BCryptDestroyKey> peer;
    Handle<BCRYPT_SECRET_HANDLE, BCryptDestroySecret> secret;
    check(BCryptImportKeyPair(ecdh, nullptr, BCRYPT_ECCPUBLIC_BLOB, &peer.value, bytes(hello) + 4, 72, 0));
    check(BCryptSecretAgreement(ownKey, peer.value, &secret.value, 0));
    if (!CryptAcquireContextW(&provider, nullptr, MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES, CRYPT_VERIFYCONTEXT))
    {
        throw runtime_error("CryptAcquireContext failed");
    }
    const string transcript = clientSide ? ownHello + hello : hello + ownHello;
    derive(secret.value, provider, "LBS1 client to server", transcript, clientSide ? &sendKey : &receiveKey);
    derive(secret.value, provider, "LBS1 server to client", transcript, clientSide ? &receiveKey : &sendKey);
    BCryptDestroyKey(ownKey);
    ownKey = nullptr;
}

string CryptoSession::handshake(const string &hello)
{
    string response = createHello();
    agree(hello);
    return response;
}

string CryptoSession::beginClientHandshake()
{
    clientSide = true;
    return createHello();
}

void CryptoSession::finishClientHandshake(const string &hello)
{
    if (!clientSide)
    {
        throw runtime_error("Client handshake not started");
    }
    agree(hello);
}

size_t CryptoSession::requestSize(const string &input) const
{
    if (input.size() < 4)
    {
        return 0;
    }
    const size_t size = static_cast<size_t>(readNumber(input.data(), 4));
    if (size < 24 || size > (clientSide ? MaxResponse : MaxRequest) + 24)
    {
        throw runtime_error("Invalid record size");
    }
    return size + 4;
}

namespace
{
using Block = array<BYTE, 16>;

Block encryptBlock(HCRYPTKEY key, Block block)
{
    DWORD size = static_cast<DWORD>(block.size());
    if (!CryptEncrypt(key, 0, FALSE, 0, block.data(), &size, block.size()) || size != 16)
    {
        throw runtime_error("CryptEncrypt failed");
    }
    return block;
}

Block multiply(Block x, Block y)
{

    Block result{};
    for (unsigned bit = 0; bit < 128; ++bit)
    {
        BYTE mask = static_cast<BYTE>(0U - ((x[bit / 8] >> (7 - bit % 8)) & 1U));
        for (unsigned i = 0; i < 16; ++i)
        {
            result[i] ^= y[i] & mask;
        }
        BYTE reduction = static_cast<BYTE>(0U - (y[15] & 1U));
        for (int i = 15; i > 0; --i)
        {
            y[i] = static_cast<BYTE>((y[i] >> 1) | (y[i - 1] << 7));
        }
        y[0] = static_cast<BYTE>((y[0] >> 1) ^ (0xe1 & reduction));
    }
    return result;
}

void hashBlocks(Block &state, const Block &h, const string &data)
{
    for (size_t offset = 0; offset < data.size(); offset += 16)
    {
        for (size_t i = 0; i < 16 && offset + i < data.size(); ++i)
        {
            state[i] ^= static_cast<BYTE>(data[offset + i]);
        }
        state = multiply(state, h);
    }
}

Block initialCounter(const string &nonce)
{
    Block counter{};
    memcpy(counter.data(), nonce.data(), 12);
    counter[15] = 1;
    return counter;
}

Block authenticationTag(HCRYPTKEY key, const string &nonce, const string &aad, const string &ciphertext)
{
    Block h = encryptBlock(key, Block{});
    Block state{};
    hashBlocks(state, h, aad);
    hashBlocks(state, h, ciphertext);
    string lengths;
    appendNumber(lengths, aad.size() * 8, 8);
    appendNumber(lengths, ciphertext.size() * 8, 8);
    hashBlocks(state, h, lengths);
    Block mask = encryptBlock(key, initialCounter(nonce));
    for (size_t i = 0; i < state.size(); ++i)
    {
        state[i] ^= mask[i];
    }
    SecureZeroMemory(h.data(), h.size());
    SecureZeroMemory(mask.data(), mask.size());
    return state;
}

string counterCrypt(HCRYPTKEY key, const string &nonce, const string &input)
{
    Block counter = initialCounter(nonce);
    string output(input.size(), '\0');
    for (size_t offset = 0; offset < input.size(); offset += 16)
    {
        // inc32; record limits keep this counter far below wraparound.
        for (int i = 15; i >= 12; --i)
        {
            if (++counter[i] != 0)
            {
                break;
            }
        }
        Block stream = encryptBlock(key, counter);
        for (size_t i = 0; i < 16 && offset + i < input.size(); ++i)
        {
            output[offset + i] = static_cast<char>(static_cast<BYTE>(input[offset + i]) ^ stream[i]);
        }
        SecureZeroMemory(stream.data(), stream.size());
    }
    return output;
}
} // namespace

string CryptoSession::decrypt(const string &frame)
{
    if (!ready() || frame.size() < 28 || requestSize(frame) != frame.size() ||
        receiveSequence == numeric_limits<uint64_t>::max() ||
        readNumber(frame.data() + 4, 8) != receiveSequence)
    {
        throw runtime_error("Invalid record sequence");
    }
    string nonce = clientSide ? "S2C1" : "C2S1";
    appendNumber(nonce, receiveSequence, 8);
    string ciphertext = frame.substr(12, frame.size() - 28);
    Block expected = authenticationTag(receiveKey, nonce, frame.substr(0, 12), ciphertext);
    unsigned difference = 0;
    for (size_t i = 0; i < expected.size(); ++i)
    {
        difference |= expected[i] ^ static_cast<BYTE>(frame[frame.size() - 16 + i]);
    }
    if (difference != 0)
    {
        throw runtime_error("Record authentication failed");
    }
    // No plaintext is produced or dispatched before the complete tag is checked.
    string plaintext = counterCrypt(receiveKey, nonce, ciphertext);
    ++receiveSequence;
    return plaintext;
}

string CryptoSession::encrypt(const string &plaintext)
{
    if (!ready() || plaintext.size() > (clientSide ? MaxRequest : MaxResponse) || sendSequence == numeric_limits<uint64_t>::max())
    {
        throw runtime_error("Invalid response size or sequence");
    }
    string header;
    appendNumber(header, plaintext.size() + 24, 4);
    appendNumber(header, sendSequence, 8);
    string nonce = clientSide ? "C2S1" : "S2C1";
    appendNumber(nonce, sendSequence, 8);
    string ciphertext = counterCrypt(sendKey, nonce, plaintext);
    Block tag = authenticationTag(sendKey, nonce, header, ciphertext);
    ++sendSequence;
    return header + ciphertext + string(reinterpret_cast<const char *>(tag.data()), tag.size());
}
