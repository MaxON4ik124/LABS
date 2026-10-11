#include "toml.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
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

void WINAPI ControlHandler(DWORD request);

int addLogMessage(const char* str, DWORD code = ERROR_SUCCESS)
{
    FILE* log = fopen("log.txt", "a+");

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

int CreateArchive()
{
    try
    {
        fs::create_directories(serviceConfig.dstDirectory);
        const fs::path archivePath = serviceConfig.dstDirectory / fs::u8path(serviceConfig.archiveName);
        Zipper archive(archivePath.string(), Zipper::Overwrite);
        unsigned int cnt = 0;
        for(const auto& entry : fs::directory_iterator(serviceConfig.srcDirectory))
        {
            if(!entry.is_regular_file())
                continue;

            if (fs::equivalent(entry.path(), archivePath))
                continue;
            
            const wstring filename = entry.path().filename().wstring();
            
            bool inConfig = false;

            for(const string& pattern : serviceConfig.patterns)
            {
                const wstring mask = fs::u8path(pattern).wstring();

                if(PathMatchSpecW(filename.c_str(), mask.c_str()))
                {
                    inConfig = true;
                    break;
                }
            }

            if(!inConfig)
                continue;
            
            ifstream input(entry.path(), ios::binary);
            if(!input)
                throw runtime_error("Cannot open file: " + entry.path().string());

            const string nameInZip = entry.path().filename().u8string();

            if(!archive.add(input, nameInZip))
                throw runtime_error("Cannot add file: " + nameInZip);
            ++cnt;
        }

        archive.close();

        const string message = "Archive created. Total files: " + to_string(cnt);
        addLogMessage(message.c_str());
        return 0;
    }
    catch(const std::exception& e)
    {
        addLogMessage(e.what(), ERROR_GEN_FAILURE);
        return -1;
    }
    
}


int InitService()
{
    if(LoadConfig() != 0)
        return -1;

    if(CreateArchive() != 0)
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
    int error;
    unsigned int i = 0;

    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwCurrentState = SERVICE_START_PENDING;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = 0;
    serviceStatus.dwWaitHint = 0;
    serviceStatusHandle = RegisterServiceCtrlHandlerA(serviceName, ControlHandler);
    if (serviceStatusHandle == (SERVICE_STATUS_HANDLE)0) 
    {
        return;
    }
    error = InitService();
    if (error) 
    {
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
        serviceStatus.dwWin32ExitCode = -1;
        SetServiceStatus(serviceStatusHandle, &serviceStatus);
        return;
    }
    serviceStatus.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus (serviceStatusHandle, &serviceStatus);
    while (serviceStatus.dwCurrentState == SERVICE_RUNNING)
    {
        char buffer[255];
        snprintf(buffer, sizeof(buffer), "%u", i);
        int result = addLogMessage(buffer);
        if (result) 
        {
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            serviceStatus.dwWin32ExitCode = -1;
            SetServiceStatus(serviceStatusHandle, &serviceStatus);
            return;
        }
        i++;
    }
    return;
}



void WINAPI ControlHandler(DWORD request)
{
    switch(request)
    {
        case SERVICE_CONTROL_STOP:
            addLogMessage("Stopped.");
            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
            return;
        case SERVICE_CONTROL_SHUTDOWN:
            addLogMessage("Shutdown.");
            serviceStatus.dwWin32ExitCode = 0;
            serviceStatus.dwCurrentState = SERVICE_STOPPED;
            SetServiceStatus (serviceStatusHandle, &serviceStatus);
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
