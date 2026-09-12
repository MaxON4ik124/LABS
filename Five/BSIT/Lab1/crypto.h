#pragma once

#include <winsock2.h>
#include <windows.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <cstdint>
#include <string>

using namespace std;

// One fresh ECDH exchange and independent AES-256 keys per connection/direction.
class CryptoSession
{
  public:
    static constexpr size_t HelloSize = 76;
    static constexpr size_t MaxRequest = 8192;
    static constexpr size_t MaxResponse = 262144;

    CryptoSession() = default;
    CryptoSession(const CryptoSession &) = delete;
    CryptoSession &operator=(const CryptoSession &) = delete;
    ~CryptoSession();

    bool ready() const;
    string handshake(const string &hello);
    string beginClientHandshake();
    void finishClientHandshake(const string &hello);
    size_t requestSize(const string &input) const;
    string decrypt(const string &frame);
    string encrypt(const string &plaintext);

  private:
    string createHello();
    void agree(const string &hello);
    bool clientSide = false;
    string ownHello;
    BCRYPT_ALG_HANDLE ecdh = nullptr;
    BCRYPT_KEY_HANDLE ownKey = nullptr;
    HCRYPTPROV provider = 0;
    HCRYPTKEY receiveKey = 0;
    HCRYPTKEY sendKey = 0;
    uint64_t receiveSequence = 0;
    uint64_t sendSequence = 0;
};
