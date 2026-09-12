#include "server.h"
#include "crypto.h"
#include "collect.h"
#include <sstream>
#include <memory>
#include <stdexcept>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <array>
#include <iostream>

using namespace std;

namespace
{
volatile LONG stopping = 0;
const SYSTEMTIME serverStarted = GetTime();
const ULONGLONG serverStartTick = GetTickCount64();

void printTime(ostream &out, const SYSTEMTIME &time)
{
    out << setfill('0') << setw(4) << time.wYear << '-' << setw(2) << time.wMonth << '-' << setw(2)
        << time.wDay << ' ' << setw(2) << time.wHour << ':' << setw(2) << time.wMinute << ':' << setw(2)
        << time.wSecond;
}

const char *driveTypeName(UINT type)
{
    switch (type)
    {
    case DRIVE_REMOVABLE:
        return "removable";
    case DRIVE_FIXED:
        return "fixed";
    case DRIVE_REMOTE:
        return "network";
    case DRIVE_CDROM:
        return "cdrom";
    case DRIVE_RAMDISK:
        return "ramdisk";
    case DRIVE_NO_ROOT_DIR:
        return "invalid_root";
    default:
        return "unknown";
    }
}

BOOL WINAPI console_handler(DWORD signal)
{
    if (signal != CTRL_C_EVENT && signal != CTRL_BREAK_EVENT)
    {
        return FALSE;
    }
    InterlockedExchange(&stopping, 1);
    return TRUE;
}

string logText(const string &text)
{
    string result;
    for (unsigned char ch : text)
    {
        if (ch < 32 || ch == 127)
        {
            const char *hex = "0123456789ABCDEF";
            result += "\\x";
            result += hex[ch >> 4];
            result += hex[ch & 15];
        }
        else
        {
            result += static_cast<char>(ch);
        }
    }
    return result;
}

void logEvent(const char *event, SOCKET socket, const string &peer, const string &details)
{
    ostringstream line;
    line << '[';
    printTime(line, GetTime());
    line << "] " << event << " client=" << peer << " socket=" << socket;
    if (!details.empty())
    {
        line << ' ' << logText(details);
    }
    cout << line.str() << endl;
}

struct Connection
{
    SOCKET socket = INVALID_SOCKET;
    WSAEVENT event = WSA_INVALID_EVENT;
    array<char, 8192> buffer{};
    string input;
    string output;
    size_t sent = 0;
    bool eof = false;
    unique_ptr<CryptoSession> crypto;
    string peer;
    string pendingLog;
    string closeReason = "server stopped";

    void close()
    {
        if (socket != INVALID_SOCKET)
        {
            if (!peer.empty())
            {
                logEvent("DISCONNECT", socket, peer, closeReason);
            }
            closesocket(socket);
        }
        if (event != WSA_INVALID_EVENT)
        {
            WSACloseEvent(event);
        }
        socket = INVALID_SOCKET;
        event = WSA_INVALID_EVENT;
        crypto.reset();
        input.clear();
        output.clear();
        sent = 0;
        eof = false;
        peer.clear();
        pendingLog.clear();
        closeReason = "server stopped";
    }

    ~Connection()
    {
        close();
    }
};

void report(const char *operation, int error = WSAGetLastError())
{
    cerr << operation << " failed: " << error << '\n';
}

bool subscribe(Connection &connection, long mask)
{
    connection.event = WSACreateEvent();
    if (connection.event == WSA_INVALID_EVENT)
    {
        report("WSACreateEvent");
        return false;
    }

    if (WSAEventSelect(connection.socket, connection.event, mask) == SOCKET_ERROR)
    {
        report("WSAEventSelect");
        return false;
    }
    return true;
}

bool flush(Connection &client)
{
    while (client.sent < client.output.size())
    {
        WSABUF buffer{static_cast<ULONG>(client.output.size() - client.sent),
                      client.output.data() + client.sent};
        DWORD sent = 0;
        if (WSASend(client.socket, &buffer, 1, &sent, 0, nullptr, nullptr) == SOCKET_ERROR)
        {
            const int error = WSAGetLastError();
            if (error == WSAEWOULDBLOCK)
            {
                return true;
            }
            report("WSASend", error);
            client.closeReason = "send failed: " + to_string(error);
            return false;
        }
        if (sent == 0)
        {
            client.closeReason = "send returned zero";
            return false;
        }
        client.sent += sent;
    }
    if (!client.pendingLog.empty())
    {
        // Log only after the entire encrypted record has been handed to Winsock.
        logEvent("SENT", client.socket, client.peer,
                 "wire_bytes=" + to_string(client.output.size()) + " " + client.pendingLog);
        SecureZeroMemory(client.pendingLog.data(), client.pendingLog.size());
        client.pendingLog.clear();
    }
    client.output.clear();
    client.sent = 0;
    return true;
}

string utf8(const wstring &text)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), nullptr, 0,
                                   nullptr, nullptr);
    string result(size, '\0');
    if (size)
    {
        WideCharToMultiByte(CP_UTF8, 0, text.data(), static_cast<int>(text.size()), result.data(), size,
                            nullptr, nullptr);
    }
    return result;
}

string dispatch(string request)
{
    try
    {
        if (!request.empty() && request.back() == '\r')
        {
            request.pop_back();
        }
        if (request.find('\0') != string::npos)
        {
            return "ERR invalid command\n";
        }
        auto split = request.find_first_of(" \t");
        string command = request.substr(0, split);
        string argument = split == string::npos ? "" : request.substr(split + 1);
        auto first = argument.find_first_not_of(" \t");
        argument = first == string::npos ? "" : argument.substr(first);
        if (command != "access" && command != "owner" && !argument.empty())
        {
            return "ERR unexpected argument\n";
        }
        ostringstream out;
        out << "OK ";
        if (command == "os")
        {
            out << utf8(GetOS());
        }
        else if (command == "time")
        {
            printTime(out, GetTime());
        }
        else if (command == "starttime")
        {
            out << "server_started=";
            printTime(out, serverStarted);
            out << " uptime_ms=" << GetTickCount64() - serverStartTick;
        }
        else if (command == "stime")
        {
            out << "uptime_ms=" << GetUptime();
        }
        else if (command == "ram")
        {
            auto ram = GetRAMInfo();
            out << "load_percent=" << ram.dwMemoryLoad << " total_bytes=" << ram.ullTotalPhys
                << " available_bytes=" << ram.ullAvailPhys
                << " used_bytes=" << ram.ullTotalPhys - ram.ullAvailPhys;
        }
        else if (command == "drive" || command == "space")
        {
            unique_ptr<DiskData, decltype(&free_disks)> disks(GetDrives(), free_disks);
            if (command == "space")
            {
                GetFreeSpace(disks.get());
            }
            for (auto disk = disks.get(); disk; disk = disk->Next)
            {
                if (disk != disks.get())
                {
                    out << "; ";
                }
                out << utf8(disk->Name);
                if (command == "drive")
                {
                    out << " type=" << disk->DriveType << " (" << driveTypeName(disk->DriveType)
                        << ") fs=" << utf8(disk->FileSystem);
                }
                else
                {
                    out << " free_gib=" << fixed << setprecision(3) << disk->FreeSpaceGb;
                }
            }
        }
        else if (command == "access" || command == "owner")
        {
            if (argument.size() >= 2 && argument.front() == '"' && argument.back() == '"')
            {
                argument = argument.substr(1, argument.size() - 2);
            }
            if (argument.empty())
            {
                return "ERR path required\n";
            }
            int size = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, argument.data(),
                                           static_cast<int>(argument.size()), nullptr, 0);
            if (!size)
            {
                return "ERR invalid UTF-8 path\n";
            }
            wstring path(size, L'\0');
            MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, argument.data(),
                                static_cast<int>(argument.size()), path.data(), size);
            if (command == "access")
            {
                out << utf8(GetAccess(path.c_str()));
            }
            else
            {
                auto owner = GetOwner(path.c_str());
                out << utf8(owner.OwnerName.empty() ? owner.OwnerSid : owner.OwnerName);
            }
        }
        else
        {
            return "ERR unknown command\n";
        }
        string response = out.str();
        for (auto &ch : response)
        {
            if (ch == '\r' || ch == '\n')
            {
                ch = ' ';
            }
        }
        return response + '\n';
    }
    catch (const exception &error)
    {
        return string("ERR ") + error.what() + '\n';
    }
}

bool exchange(Connection &client)
{
    try
    {
        if (!flush(client))
        {
            return false;
        }
        if (!client.output.empty())
        {
            return true;
        }
        auto requiredSize = [&]() -> size_t
        {
            return client.crypto->ready() ? client.crypto->requestSize(client.input)
                                          : CryptoSession::HelloSize;
        };
        size_t required = requiredSize();
        if ((required == 0 || client.input.size() < required) && !client.eof)
        {
            WSABUF buffer{static_cast<ULONG>(client.buffer.size()), client.buffer.data()};
            DWORD received = 0, flags = 0;
            if (WSARecv(client.socket, &buffer, 1, &received, &flags, nullptr, nullptr) == SOCKET_ERROR)
            {
                const int error = WSAGetLastError();
                if (error == WSAEWOULDBLOCK)
                {
                    return true;
                }
                client.closeReason = "receive failed: " + to_string(error);
                return false;
            }
            if (!received)
            {
                client.eof = true;
            }
            else
            {
                client.input.append(client.buffer.data(), received);
            }
            required = requiredSize();
        }
        if (required != 0 && client.input.size() >= required)
        {
            string record = client.input.substr(0, required);
            client.input.erase(0, required);
            if (!client.crypto->ready())
            {
                client.output = client.crypto->handshake(record);
                client.pendingLog = "ECDH handshake; AES-256-GCM session keys established";
            }
            else
            {
                string command = client.crypto->decrypt(record);
                if (!command.empty() && command.back() == '\n')
                {
                    command.pop_back();
                }
                string response =
                    command.find('\n') == string::npos ? dispatch(command) : "ERR one command per record\n";
                if (response.size() > CryptoSession::MaxResponse)
                {
                    response = "ERR response too large\n";
                }
                client.output = client.crypto->encrypt(response);
                client.pendingLog = "command=" + command + " response=" + response;
                SecureZeroMemory(command.data(), command.size());
                SecureZeroMemory(response.data(), response.size());
            }
        }
        else if (client.eof)
        {
            client.closeReason = client.input.empty() ? "client closed connection" : "truncated record";
            return false;
        }
        if (!flush(client))
        {
            return false;
        }
        if (client.output.empty())
        {
            if (!WSASetEvent(client.event))
            {
                client.closeReason = "WSASetEvent failed: " + to_string(WSAGetLastError());
                return false;
            }
        }
        return true;
    }
    catch (const exception &error)
    {
        // Invalid handshake, tag or sequence: close without a plaintext error.
        client.closeReason = string("protocol error: ") + error.what();
        return false;
    }
}

int serve(unsigned short port)
{
    array<Connection, WSA_MAXIMUM_WAIT_EVENTS> connections;
    Connection &listener = connections[0];
    listener.socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
    if (listener.socket == INVALID_SOCKET)
    {
        report("WSASocket");
        return 1;
    }
    BOOL exclusive = TRUE;
    if (setsockopt(listener.socket, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
                   reinterpret_cast<const char *>(&exclusive), sizeof(exclusive)) == SOCKET_ERROR)
    {
        report("setsockopt");
        return 1;
    }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (bind(listener.socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) == SOCKET_ERROR)
    {
        report("bind");
        return 1;
    }
    if (listen(listener.socket, SOMAXCONN) == SOCKET_ERROR)
    {
        report("listen");
        return 1;
    }
    if (!subscribe(listener, FD_ACCEPT | FD_CLOSE))
    {
        return 1;
    }
    cout << "TCP information server: 0.0.0.0:" << port << " (Ctrl+C to stop)" << endl;

    while (true)
    {
        if (InterlockedCompareExchange(&stopping, 0, 0))
        {
            break;
        }
        array<WSAEVENT, WSA_MAXIMUM_WAIT_EVENTS> events{};
        array<Connection *, WSA_MAXIMUM_WAIT_EVENTS> active{};
        DWORD count = 0;
        for (auto &connection : connections)
        {
            if (connection.socket != INVALID_SOCKET)
            {
                events[count] = connection.event;
                active[count++] = &connection;
            }
        }
        const DWORD result = WSAWaitForMultipleEvents(count, events.data(), FALSE, 200, FALSE);
        if (result == WSA_WAIT_TIMEOUT)
        {
            continue;
        }
        if (result == WSA_WAIT_FAILED)
        {
            report("WSAWaitForMultipleEvents");
            return 1;
        }

        for (DWORD i = 0; i < count; ++i)
        {
            if (WSAWaitForMultipleEvents(1, &events[i], FALSE, 0, FALSE) != WSA_WAIT_EVENT_0)
            {
                continue;
            }
            Connection &connection = *active[i];
            WSANETWORKEVENTS network{};
            if (WSAEnumNetworkEvents(connection.socket, connection.event, &network) == SOCKET_ERROR)
            {
                report("WSAEnumNetworkEvents");
                if (i == 0)
                {
                    return 1;
                }
                connection.closeReason = "WSAEnumNetworkEvents failed";
                connection.close();
                continue;
            }
            bool failed = false;
            for (int bit = 0; bit < FD_MAX_EVENTS; ++bit)
            {
                if ((network.lNetworkEvents & (1L << bit)) && network.iErrorCode[bit])
                {
                    report("Network event", network.iErrorCode[bit]);
                    connection.closeReason = "network error: " + to_string(network.iErrorCode[bit]);
                    failed = true;
                }
            }
            if (failed)
            {
                if (i == 0)
                {
                    return 1;
                }
                connection.close();
                continue;
            }
            if (i == 0)
            {
                if (network.lNetworkEvents & FD_CLOSE)
                {
                    return 1;
                }
                if (!(network.lNetworkEvents & FD_ACCEPT))
                {
                    continue;
                }

                sockaddr_in clientAddress{};
                int clientAddressSize = sizeof(clientAddress);
                SOCKET socket = accept(listener.socket, reinterpret_cast<sockaddr *>(&clientAddress),
                                       &clientAddressSize);
                if (socket == INVALID_SOCKET)
                {
                    const int error = WSAGetLastError();
                    if (error != WSAEWOULDBLOCK)
                    {
                        report("accept", error);
                    }
                    continue;
                }
                string peer = string(inet_ntoa(clientAddress.sin_addr)) + ":" +
                              to_string(ntohs(clientAddress.sin_port));
                Connection *slot = nullptr;
                for (auto &candidate : connections)
                {
                    if (candidate.socket == INVALID_SOCKET)
                    {
                        slot = &candidate;
                        break;
                    }
                }
                if (!slot)
                {
                    logEvent("REJECT", socket, peer, "client limit reached");
                    closesocket(socket);
                    continue;
                }
                slot->socket = socket;
                slot->peer = peer;
                logEvent("CONNECT", socket, peer, "TCP connection accepted; waiting for ECDH");
                slot->crypto = make_unique<CryptoSession>();
                if (!subscribe(*slot, FD_READ | FD_WRITE | FD_CLOSE))
                {
                    slot->closeReason = "cannot subscribe to socket events";
                    slot->close();
                }
            }
            else
            {

                if (!exchange(connection))
                {
                    connection.close();
                }
            }
        }
    }
    return 0;
}
} // namespace

int run_tcp_server(unsigned short port)
{
    WSADATA data{};
    const int error = WSAStartup(MAKEWORD(2, 2), &data);
    if (error)
    {
        report("WSAStartup", error);
        return 1;
    }
    InterlockedExchange(&stopping, 0);
    if (!SetConsoleCtrlHandler(console_handler, TRUE))
    {
        cerr << "SetConsoleCtrlHandler failed: " << GetLastError() << '\n';
        WSACleanup();
        return 1;
    }
    const int result = serve(port);
    SetConsoleCtrlHandler(console_handler, FALSE);
    WSACleanup();
    return result;
}

int main(int argc, char *argv[])
{
    SetConsoleOutputCP(CP_UTF8);
    unsigned long port = 8080;
    if (argc > 2)
    {
        cerr << "Usage: server.exe [port]\n";
        return 1;
    }
    if (argc == 2)
    {
        const string value = argv[1];
        if (value.empty() || value.find_first_not_of("0123456789") != string::npos)
        {
            cerr << "Port must be an integer from 1 to 65535\n";
            return 1;
        }
        try
        {
            port = stoul(value);
        }
        catch (const exception &)
        {
            port = 0;
        }
        if (port == 0 || port > 65535)
        {
            cerr << "Port must be an integer from 1 to 65535\n";
            return 1;
        }
    }
    return run_tcp_server(static_cast<unsigned short>(port));
}
