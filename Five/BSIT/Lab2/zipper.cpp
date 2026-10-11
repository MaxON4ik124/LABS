#include "toml.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <tuple>
#include <cstdint>
#include <mutex>
#include <windows.h>
#include <winsvc.h>
#include <stdio.h>
#include <string.h>
#include <zipper/zipper.h>
#include <shlwapi.h>


using namespace std;
using namespace zipper;
namespace fs = std::filesystem;

struct Config
{
    fs::path srcDirectory;
    fs::path dstDirectory;
    string archiveName;
    vector<string> patterns;
};




Config serviceConfig;
SERVICE_STATUS serviceStatus = {};
SERVICE_STATUS_HANDLE serviceStatusHandle = NULL;
char serviceName[] = "zipper";
const char* servicePath = NULL;
HANDLE serviceStopEvent = NULL;
mutex serviceStatusMutex;

void WINAPI ControlHandler(DWORD request);

int addLogMessage(const char* str, DWORD code = ERROR_SUCCESS)
{
    wchar_t modulePath[32768];
    DWORD length = GetModuleFileNameW(NULL, modulePath, 32768);
    if (length == 0 || length >= 32768)
        return -1;
    const fs::path logPath =
        fs::path(wstring(modulePath, length)).parent_path() / L"log.txt";
    FILE* log = _wfopen(logPath.c_str(), L"a");

    if (log == NULL)
    {
        return -1;
    }
    fprintf(log, "[code: %lu] %s\n", static_cast<unsigned long>(code), str);
    fclose(log);
    return 0;
}

fs::path GetConfigPath()
{
    wchar_t buffer[32768];

    DWORD length = GetModuleFileNameW(NULL, buffer, static_cast<DWORD>(sizeof(buffer) / sizeof(buffer[0])));

    if (length == 0)
        throw runtime_error("Cannot get executable path");

    if (length >= sizeof(buffer) / sizeof(buffer[0]))
        throw runtime_error("Executable path is too long");

    return fs::path(wstring(buffer, length)).parent_path() / L"config.toml";
}

int LoadConfig()
{
    try
    {
        const fs::path configPath = GetConfigPath();
        ifstream input(configPath, ios::binary);

        if(!input)
            throw runtime_error("Cannot open config.toml");

        auto list = toml::parse(input);

        auto readString = [&](const char* key) -> string
        {
            auto value = list["files"][key].value<string>();

            if (!value || value->empty())
                throw runtime_error(string("Missing or invalid key: files.") + key);

            return *value;
        };

        auto resolvePath = [&](const string& text) -> fs::path
        {
            fs::path path = fs::u8path(text);

            if (path.is_relative())
                path = configPath.parent_path() / path;

            return path.lexically_normal();
        };

        Config loaded;

        loaded.srcDirectory = resolvePath(readString("source_directory"));
        loaded.dstDirectory = resolvePath(readString("destination_directory"));
        loaded.archiveName = readString("archive_name");
        if (loaded.archiveName == "." || loaded.archiveName == ".." || loaded.archiveName.find_first_of("\\/:*?\"<>|") != string::npos)
            throw std::runtime_error("Invalid archive_name");

        auto* patterns = list["files"]["patterns"].as_array();

        if (!patterns || patterns->empty())
            throw runtime_error("files.patterns must be a non-empty array");

        for (const auto& item : *patterns)
        {
            auto pattern = item.value<string>();

            if (!pattern || pattern->empty())
                throw runtime_error("Each pattern must be a non-empty string");

            loaded.patterns.push_back(*pattern);
        }

        if (!fs::is_directory(loaded.srcDirectory))
            throw runtime_error("Source directory does not exist");
        
        serviceConfig = move(loaded);
        return 0;
    }
    catch (const toml::parse_error& error)
    {
        ostringstream message;
        message << "TOML parse error: " << error;
        addLogMessage(message.str().c_str(), ERROR_INVALID_DATA);
        return -1;
    }
    catch (const exception& error)
    {
        addLogMessage(error.what(), ERROR_INVALID_DATA);
        return -1;
    }

}

fs::path ArchivePath()
{
    return serviceConfig.dstDirectory / fs::u8path(serviceConfig.archiveName);
}

fs::path TemporaryArchivePath()
{
    fs::path path = ArchivePath();
    path += L".zipper-" + to_wstring(GetCurrentProcessId()) + L".tmp";
    return path;
}

bool IsArchiveOutput(const fs::path& path)
{
    for (const fs::path& output : {ArchivePath(), TemporaryArchivePath()})
    {
        error_code error;
        if (fs::equivalent(path, output, error) && !error)
            return true;
        const wstring a = fs::absolute(path).lexically_normal().wstring();
        const wstring b = fs::absolute(output).lexically_normal().wstring();
        if (_wcsicmp(a.c_str(), b.c_str()) == 0)
            return true;
    }
    return false;
}

bool SamePath(const fs::path& left, const fs::path& right)
{
    const wstring a = fs::absolute(left).lexically_normal().wstring();
    const wstring b = fs::absolute(right).lexically_normal().wstring();
    return _wcsicmp(a.c_str(), b.c_str()) == 0;
}

int RenameArchive(const fs::path& oldPath, const fs::path& newPath)
{
    if (oldPath.empty() || SamePath(oldPath, newPath))
        return 0;

    error_code existsError;
    if (!fs::exists(oldPath, existsError))
    {
        if (existsError)
            throw system_error(existsError, "Cannot inspect previous archive");
        return 0;
    }

    fs::create_directories(newPath.parent_path());
    if (!MoveFileExW(oldPath.c_str(), newPath.c_str(),
                     MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        throw system_error(GetLastError(), system_category(),
                           "Cannot rename previous archive");
    }

    const string message = "Archive renamed to: " + newPath.string();
    addLogMessage(message.c_str());
    return 0;
}

bool StopRequested()
{
    return serviceStopEvent &&
        WaitForSingleObject(serviceStopEvent, 0) == WAIT_OBJECT_0;
}

wstring PatternMask(const string& pattern)
{
    wstring mask = fs::u8path(pattern).wstring();
    replace(mask.begin(), mask.end(), L'/', L'\\');
    return mask;
}

bool MatchesPattern(const fs::path& relativePath)
{
    const wstring path = relativePath.wstring();
    const wstring filename = relativePath.filename().wstring();

    for (const string& pattern : serviceConfig.patterns)
    {
        const wstring mask = PatternMask(pattern);

        // Matching the complete relative path supports patterns such as
        // "lib\\*.*", while matching the file name preserves the old
        // behavior of patterns such as "*.java".
        if (PathMatchSpecW(path.c_str(), mask.c_str()) ||
            PathMatchSpecW(filename.c_str(), mask.c_str()))
            return true;

        // A directory pattern (for example, "lib" or "assets\\*") selects
        // every file below that directory.
        fs::path directory = relativePath.parent_path();
        while (!directory.empty())
        {
            const wstring directoryPath = directory.wstring();
            const wstring directoryName = directory.filename().wstring();
            if (PathMatchSpecW(directoryPath.c_str(), mask.c_str()) ||
                PathMatchSpecW(directoryName.c_str(), mask.c_str()))
                return true;

            const fs::path parent = directory.parent_path();
            if (parent == directory)
                break;
            directory = parent;
        }
    }

    return false;
}

int CreateArchive()
{
    fs::path temporaryPath;
    try
    {
        fs::create_directories(serviceConfig.dstDirectory);
        const fs::path archivePath = ArchivePath();
        temporaryPath = TemporaryArchivePath();
        Zipper archive(temporaryPath.string(), Zipper::Overwrite);
        unsigned int cnt = 0;
        for (const auto& entry :
             fs::recursive_directory_iterator(serviceConfig.srcDirectory))
        {
            if(!entry.is_regular_file())
                continue;

            if (IsArchiveOutput(entry.path()))
                continue;

            if (StopRequested())
                throw runtime_error("Archive update cancelled: service stopping");

            const fs::path relativePath =
                entry.path().lexically_relative(serviceConfig.srcDirectory);
            if (relativePath.empty() || relativePath == L".")
                continue;
            if (!MatchesPattern(relativePath))
                continue;

            ifstream input(entry.path(), ios::binary);
            if(!input)
                throw runtime_error("Cannot open file: " + entry.path().string());

            const string nameInZip = relativePath.generic_u8string();

            if(!archive.add(input, nameInZip))
                throw runtime_error("Cannot add file: " + nameInZip);
            ++cnt;
        }

        archive.close();

        if (StopRequested())
            throw runtime_error("Archive update cancelled: service stopping");

        if (!MoveFileExW(temporaryPath.c_str(), archivePath.c_str(),
                        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
            throw system_error(GetLastError(), system_category(),
                               "Cannot replace archive");

        const string message =
            "Archive created. Total files: " + to_string(cnt);
        addLogMessage(message.c_str());
        return 0;
    }
    catch(const std::exception& e)
    {
        if (!temporaryPath.empty())
        {
            error_code ignored;
            fs::remove(temporaryPath, ignored);
        }
        addLogMessage(e.what(), ERROR_GEN_FAILURE);
        return -1;
    }
    
}


uint64_t FileHash(const fs::path& path)
{
    ifstream input(path, ios::binary);
    if (!input)
        throw runtime_error("Cannot read monitored file: " + path.string());

    uint64_t hash = UINT64_C(14695981039346656037);
    char buffer[65536];
    while (input)
    {
        if (StopRequested())
            throw runtime_error("Monitoring cancelled: service stopping");
        input.read(buffer, sizeof(buffer));
        for (streamsize i = 0; i < input.gcount(); ++i)
        {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= UINT64_C(1099511628211);
        }
    }
    if (!input.eof())
        throw runtime_error("Cannot read monitored file: " + path.string());
    return hash;
}

using SourceSnapshot =
    map<wstring, tuple<uintmax_t, fs::file_time_type, uint64_t>>;

SourceSnapshot CaptureSourceSnapshot()
{
    SourceSnapshot snapshot;
    for (const auto& entry :
         fs::recursive_directory_iterator(serviceConfig.srcDirectory))
    {
        if (!entry.is_regular_file() || IsArchiveOutput(entry.path()))
            continue;

        const fs::path relativePath =
            entry.path().lexically_relative(serviceConfig.srcDirectory);
        if (relativePath.empty() || relativePath == L".")
            continue;
        snapshot.emplace(relativePath.generic_wstring(),
                         make_tuple(entry.file_size(), entry.last_write_time(),
                                    FileHash(entry.path())));
    }
    return snapshot;
}

struct MonitorState
{
    bool initialized = false;
    uint64_t configHash = 0;
    SourceSnapshot source;
    fs::path archivePath;
    string lastError;
    ULONGLONG lastConfigAttempt = 0;
};

int MonitorOnce(MonitorState& state)
{
    try
    {
        const uint64_t configHash = FileHash(GetConfigPath());
        const bool configChanged =
            !state.initialized || configHash != state.configHash;
        const fs::path previousArchivePath = state.archivePath;

        if (configChanged)
        {
            const string errorKey = "Config reload failed: " + to_string(configHash);
            const ULONGLONG now = GetTickCount64();
            if (state.lastError == errorKey &&
                now - state.lastConfigAttempt < 5000)
                return -1;
            state.lastConfigAttempt = now;
            if (LoadConfig() != 0)
            {
                state.lastError = errorKey;
                return -1;
            }

            RenameArchive(previousArchivePath, ArchivePath());
        }

        SourceSnapshot snapshot = CaptureSourceSnapshot();
        if (!configChanged && snapshot == state.source)
        {
            state.lastError.clear();
            return 0;
        }

        if (CreateArchive() != 0)
            return -1;


        state.configHash = configHash;
        state.source = move(snapshot);
        state.archivePath = ArchivePath();
        state.initialized = true;
        state.lastError.clear();
        return 1;
    }
    catch (const exception& error)
    {
        if (state.lastError != error.what())
            addLogMessage(error.what(), ERROR_GEN_FAILURE);
        state.lastError = error.what();
        return -1;
    }
}

void RunMonitor(MonitorState& state, HANDLE stopEvent, DWORD interval = 1000)
{
    while (WaitForSingleObject(stopEvent, 0) == WAIT_TIMEOUT)
    {
        MonitorOnce(state);
        DWORD result = WaitForSingleObject(stopEvent, interval);
        if (result != WAIT_TIMEOUT)
            break;
    }
}

int InitService()
{
    if(LoadConfig() != 0)
        return -1;

    return addLogMessage("Service initialized.");
}

int InstallService() 
{
    SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if(!hSCManager) 
    {
        addLogMessage("Error: Can't open Service Control Manager");
        return -1;
    }
    SC_HANDLE hService = CreateServiceA(
        hSCManager,
        serviceName,
        serviceName,
        SERVICE_ALL_ACCESS,
        SERVICE_WIN32_OWN_PROCESS,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_NORMAL,
        servicePath,
        NULL, NULL, NULL, NULL, NULL
    );
    if(!hService) 
    {
        int err = GetLastError();
        switch(err) 
        {
            case ERROR_ACCESS_DENIED:
                addLogMessage("Error: ERROR_ACCESS_DENIED");
                break;
            case ERROR_CIRCULAR_DEPENDENCY:
                addLogMessage("Error: ERROR_CIRCULAR_DEPENDENCY");
                break;
            case ERROR_DUPLICATE_SERVICE_NAME:
                addLogMessage("Error: ERROR_DUPLICATE_SERVICE_NAME");
                break;
            case ERROR_INVALID_HANDLE:
                addLogMessage("Error: ERROR_INVALID_HANDLE");
                break;
            case ERROR_INVALID_NAME:
                addLogMessage("Error: ERROR_INVALID_NAME");
                break;
            case ERROR_INVALID_PARAMETER:
                addLogMessage("Error: ERROR_INVALID_PARAMETER");
                break;
            case ERROR_INVALID_SERVICE_ACCOUNT:
                addLogMessage("Error: ERROR_INVALID_SERVICE_ACCOUNT");
                break;
            case ERROR_SERVICE_EXISTS:
                addLogMessage("Error: ERROR_SERVICE_EXISTS");
                break;
            default:
                addLogMessage("Error: Undefined");
        }
        CloseServiceHandle(hSCManager);
        return -1;
    }
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    addLogMessage("Success install service!");
    return 0;
}
int RemoveService() 
{
    SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL,
    SC_MANAGER_ALL_ACCESS);
    if(!hSCManager) 
    {
        addLogMessage("Error: Can't open Service Control Manager");
        return -1;
    }
    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName, SERVICE_STOP | DELETE);
    if(!hService) 
    {
        addLogMessage("Error: Can't remove service");
        CloseServiceHandle(hSCManager);
        return -1;
    }
    DeleteService(hService);
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    addLogMessage("Success remove service!");
    return 0;
}


int StartInstalledService()
{
    SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName, SERVICE_START);

    if(!StartServiceA(hService, 0, NULL))
    {
        CloseServiceHandle(hSCManager);
        addLogMessage("Error: Can't start service");
        return -1;
    }
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return 0;
}

int StopInstalledService()
{
    SC_HANDLE hSCManager = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    
    if(!hSCManager)
    {
        DWORD error = GetLastError();
        addLogMessage("Error: Can't open SCm", error);
        return -1;
    }
    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName, SERVICE_STOP);

    if(!hService)
    {
        DWORD error = GetLastError();
        addLogMessage("Error: Can't open service", error);
        return -1;
    }

    SERVICE_STATUS status = {};
    BOOL success = ControlService(hService, SERVICE_CONTROL_STOP, &status);
    DWORD error = success ? ERROR_SUCCESS : GetLastError();
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    if (!success)
    {
        if (error == ERROR_SERVICE_NOT_ACTIVE)
        {
            addLogMessage("Service is already stopped.");
            return 0;
        }

        addLogMessage("Error: Can't request service stop", error);
        return -1;
    }

    addLogMessage("Service stop requested.");
    return 0;
}



void WINAPI ServiceMain(DWORD, LPSTR*)
{
    unique_lock<mutex> statusLock(serviceStatusMutex);
    int error;

    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = 0;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    serviceStatusHandle = RegisterServiceCtrlHandlerA(serviceName, ControlHandler);
    if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
        return;
    serviceStatus.dwWaitHint = 5000;
    serviceStatus.dwCheckPoint = 1;
    SetServiceStatus(serviceStatusHandle, &serviceStatus);

    serviceStopEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
    if (!serviceStopEvent)
    {
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = GetLastError();
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        return;
    }

    error = InitService();
    if (error) 
    {
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = -1;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        CloseHandle(serviceStopEvent);
        serviceStopEvent = NULL;
        return;
    }
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    SetServiceStatus (serviceStatusHandle, &serviceStatus);
    statusLock.unlock();

    MonitorState monitorState;
    RunMonitor(monitorState, serviceStopEvent);

    statusLock.lock();
    if (serviceStopEvent)
    {
        CloseHandle(serviceStopEvent);
        serviceStopEvent = NULL;
    }

    serviceStatus.dwCurrentState = SERVICE_STOPPED;
    serviceStatus.dwControlsAccepted = 0;
    serviceStatus.dwWin32ExitCode = 0;
    SetServiceStatus(serviceStatusHandle, &serviceStatus);
    return;
}



void WINAPI ControlHandler(DWORD request)
{
    lock_guard<mutex> statusLock(serviceStatusMutex);
    if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
        return;
    switch(request)
    {
        case SERVICE_CONTROL_STOP:
            addLogMessage("Stop requested.");
            serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            serviceStatus.dwControlsAccepted = 0;
            serviceStatus.dwWaitHint = 30000;
            serviceStatus.dwCheckPoint = 1;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            if (serviceStopEvent)
                SetEvent(serviceStopEvent);
            return;
        case SERVICE_CONTROL_SHUTDOWN:
            addLogMessage("Shutdown.");
            serviceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            serviceStatus.dwControlsAccepted = 0;
            serviceStatus.dwWaitHint = 30000;
            serviceStatus.dwCheckPoint = 1;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            if (serviceStopEvent)
                SetEvent(serviceStopEvent);
            return;
        default:
            break;
    }
    SetServiceStatus (serviceStatusHandle, &serviceStatus);
    return;
}

int main(int argc, char* argv[])
{
    servicePath = argv[0];
    if(argc - 1 == 0) 
    {
        SERVICE_TABLE_ENTRYA ServiceTable[2] = {};
        ServiceTable[0].lpServiceName = serviceName;
        ServiceTable[0].lpServiceProc = ServiceMain;
        if(!StartServiceCtrlDispatcherA(ServiceTable))
        {
            addLogMessage("Error: StartServiceCtrlDispatcher");
        }
    } 
    else if(strcmp(argv[argc-1], "install") == 0)
        InstallService(); 
    else if(strcmp(argv[argc-1], "remove") == 0)
        RemoveService(); 
    else if(strcmp(argv[argc-1], "start") == 0)
        StartInstalledService();
    else if(strcmp(argv[argc-1], "stop") == 0)
        StopInstalledService();


    return 0;
}
