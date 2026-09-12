#include "crypto.h"
#include <ws2tcpip.h>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>

using namespace std;

namespace
{
string trim(const string &value)
{
    size_t first = value.find_first_not_of(" \t\r\n");
    if (first == string::npos)
    {
        return "";
    }
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}

struct Endpoint
{
    string host;
    string port;

    string name() const
    {
        return host + ":" + port;
    }
};

Endpoint parseEndpoint(const string &value)
{
    size_t colon = value.find(':');
    if (colon == string::npos || colon == 0 || colon != value.rfind(':'))
    {
        throw runtime_error("Use host:port, for example 127.0.0.1:8080");
    }
    Endpoint endpoint{value.substr(0, colon), value.substr(colon + 1)};
    if (endpoint.host.find_first_of(" \t\r\n/\\") != string::npos || endpoint.port.empty() ||
        endpoint.port.size() > 5 || endpoint.port.find_first_not_of("0123456789") != string::npos)
    {
        throw runtime_error("Invalid host:port");
    }
    unsigned long port = stoul(endpoint.port);
    if (port == 0 || port > 65535)
    {
        throw runtime_error("Port must be from 1 to 65535");
    }
    endpoint.port = to_string(port);
    transform(endpoint.host.begin(), endpoint.host.end(), endpoint.host.begin(),
              [](unsigned char ch) { return static_cast<char>(tolower(ch)); });
    return endpoint;
}

class Connection
{
  public:
    SOCKET socket = INVALID_SOCKET;
    CryptoSession crypto;

    ~Connection()
    {
        if (socket != INVALID_SOCKET)
        {
            shutdown(socket, SD_BOTH);
            closesocket(socket);
        }
    }

    void open(const Endpoint &endpoint)
    {
        addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        addrinfo *addresses = nullptr;
        int error = getaddrinfo(endpoint.host.c_str(), endpoint.port.c_str(), &hints, &addresses);
        if (error != 0)
        {
            throw runtime_error("Cannot resolve host: " + to_string(error));
        }
        unique_ptr<addrinfo, decltype(&freeaddrinfo)> guard(addresses, freeaddrinfo);
        for (auto address = addresses; address; address = address->ai_next)
        {
            SOCKET candidate = ::socket(address->ai_family, address->ai_socktype, address->ai_protocol);
            if (candidate == INVALID_SOCKET)
            {
                continue;
            }
            u_long nonblocking = 1;
            if (ioctlsocket(candidate, FIONBIO, &nonblocking) == SOCKET_ERROR)
            {
                closesocket(candidate);
                continue;
            }
            bool connected =
                ::connect(candidate, address->ai_addr, static_cast<int>(address->ai_addrlen)) == 0;
            if (!connected && WSAGetLastError() == WSAEWOULDBLOCK)
            {
                fd_set writable, failed;
                FD_ZERO(&writable);
                FD_ZERO(&failed);
                FD_SET(candidate, &writable);
                FD_SET(candidate, &failed);
                timeval timeout{5, 0};
                if (select(0, nullptr, &writable, &failed, &timeout) > 0)
                {
                    int result = 0;
                    int length = sizeof(result);
                    connected = getsockopt(candidate, SOL_SOCKET, SO_ERROR, reinterpret_cast<char *>(&result),
                                           &length) == 0 &&
                                result == 0 && FD_ISSET(candidate, &writable);
                }
            }
            nonblocking = 0;
            if (connected && ioctlsocket(candidate, FIONBIO, &nonblocking) == 0)
            {
                socket = candidate;
                break;
            }
            closesocket(candidate);
        }
        if (socket == INVALID_SOCKET)
        {
            throw runtime_error("Connection failed or timed out");
        }
        DWORD timeout = 10000;
        if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char *>(&timeout),
                       sizeof(timeout)) ||
            setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<char *>(&timeout), sizeof(timeout)))
        {
            throw runtime_error("Cannot set socket timeouts");
        }
        sendAll(crypto.beginClientHandshake());
        crypto.finishClientHandshake(receiveExact(CryptoSession::HelloSize));
        // Confirm that both peers derived matching keys before reporting success.
        if (request("time").rfind("OK ", 0) != 0)
        {
            throw runtime_error("Encrypted session confirmation failed");
        }
    }

    bool disconnected() const
    {
        fd_set readable;
        FD_ZERO(&readable);
        FD_SET(socket, &readable);
        timeval timeout{};
        int result = select(0, &readable, nullptr, nullptr, &timeout);
        if (result < 0)
        {
            return true;
        }
        if (result > 0)
        {
            char byte;
            return recv(socket, &byte, 1, MSG_PEEK) <= 0;
        }
        return false;
    }

    string request(const string &command)
    {
        sendAll(crypto.encrypt(command));
        string frame = receiveExact(4);
        size_t size = crypto.requestSize(frame);
        frame += receiveExact(size - 4);
        return crypto.decrypt(frame);
    }

  private:
    void sendAll(const string &data)
    {
        size_t offset = 0;
        while (offset < data.size())
        {
            int sent = send(socket, data.data() + offset, static_cast<int>(data.size() - offset), 0);
            if (sent <= 0)
            {
                throw runtime_error("Send failed: " + to_string(WSAGetLastError()));
            }
            offset += sent;
        }
    }

    string receiveExact(size_t size)
    {
        string data(size, '\0');
        size_t offset = 0;
        while (offset < size)
        {
            int received = recv(socket, data.data() + offset, static_cast<int>(size - offset), 0);
            if (received <= 0)
            {
                throw runtime_error(received == 0 ? "Server disconnected"
                                                  : "Receive failed: " + to_string(WSAGetLastError()));
            }
            offset += received;
        }
        return data;
    }
};

bool readLine(string &line)
{
    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    if (!GetConsoleMode(input, &mode))
    {
        return static_cast<bool>(getline(cin, line));
    }
    wstring wide;
    for (;;)
    {
        WCHAR buffer[256];
        DWORD count = 0;
        if (!ReadConsoleW(input, buffer, 256, &count, nullptr) || count == 0)
        {
            return false;
        }
        wide.append(buffer, count);
        if (wide.find(L'\n') != wstring::npos)
        {
            break;
        }
    }
    int size = WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), nullptr, 0,
                                   nullptr, nullptr);
    line.resize(size);
    WideCharToMultiByte(CP_UTF8, 0, wide.data(), static_cast<int>(wide.size()), line.data(), size, nullptr,
                        nullptr);
    return true;
}

void runClient()
{
    map<string, unique_ptr<Connection>> connections;
    string selected;
    cout << "connect host:port | disconnect host:port | list | get host:port\n"
            "os | time | starttime | ram | drive | space | access path | owner path | exit\n";
    string line;
    while (cout << "> " << flush, readLine(line))
    {
        line = trim(line);
        if (line.empty())
        {
            continue;
        }
        for (auto entry = connections.begin(); entry != connections.end();)
        {
            if (entry->second->disconnected())
            {
                cout << "Disconnected " << entry->first << '\n';
                if (selected == entry->first)
                {
                    selected.clear();
                }
                entry = connections.erase(entry);
            }
            else
            {
                ++entry;
            }
        }
        size_t split = line.find_first_of(" \t");
        string command = line.substr(0, split);
        string argument = split == string::npos ? "" : trim(line.substr(split + 1));
        try
        {
            if (command == "connect" || command == "disconnect" || command == "get")
            {
                Endpoint endpoint = parseEndpoint(argument);
                string name = endpoint.name();
                if (command == "connect")
                {
                    if (connections.count(name))
                    {
                        throw runtime_error("Already connected: " + name);
                    }
                    auto connection = make_unique<Connection>();
                    connection->open(endpoint);
                    connections.emplace(name, move(connection));
                    cout << "Connected " << name << " (AES-256-GCM)\n";
                }
                else
                {
                    if (!connections.count(name))
                    {
                        throw runtime_error("Not connected: " + name);
                    }
                    if (command == "get")
                    {
                        selected = name;
                        cout << "Selected " << name << '\n';
                    }
                    else
                    {
                        connections.erase(name);
                        if (selected == name)
                        {
                            selected.clear();
                        }
                        cout << "Disconnected " << name << '\n';
                    }
                }
            }
            else if (command == "list" && argument.empty())
            {
                if (connections.empty())
                {
                    cout << "No connections\n";
                }
                for (const auto &entry : connections)
                {
                    cout << (entry.first == selected ? "* " : "  ") << entry.first
                         << " socket=" << entry.second->socket << '\n';
                }
            }
            else if (command == "exit" && argument.empty())
            {
                break;
            }
            else
            {
                bool pathCommand = command == "access" || command == "owner";
                bool infoCommand = command == "os" || command == "time" || command == "starttime" ||
                                   command == "ram" || command == "drive" || command == "space";
                if ((!pathCommand && !infoCommand) || (infoCommand && !argument.empty()) ||
                    (pathCommand && argument.empty()))
                {
                    throw runtime_error("Unknown command or invalid arguments");
                }
                if (line.size() > CryptoSession::MaxRequest)
                {
                    throw runtime_error("Command exceeds 8192 UTF-8 bytes");
                }
                if (selected.empty())
                {
                    throw runtime_error("Select a server with get host:port first");
                }
                try
                {
                    cout << '[' << selected << "] " << connections.at(selected)->request(line);
                }
                catch (...)
                {
                    connections.erase(selected);
                    selected.clear();
                    throw;
                }
            }
        }
        catch (const exception &error)
        {
            cout << "ERR " << error.what() << '\n';
        }
    }
}
} // namespace

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    WSADATA data{};
    int error = WSAStartup(MAKEWORD(2, 2), &data);
    if (error)
    {
        cerr << "WSAStartup failed: " << error << '\n';
        return 1;
    }
    int result = 0;
    try
    {
        runClient();
    }
    catch (const exception &error)
    {
        cerr << error.what() << '\n';
        result = 1;
    }
    WSACleanup();
    return result;
}
