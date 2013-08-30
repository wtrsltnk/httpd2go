#include "webserver.h"
#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

WebServer::WebServer()
{ }

WebServer::~WebServer()
{ }

bool WebServer::Start(Environment& env)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi, mysqlpi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    // Start the child process.
    if( !CreateProcess( NULL,   // No module name (use command line)
        (LPSTR)env.BuildCommandLine().c_str(),        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    )
    {
        printf( "CreateProcess failed (%d).\n", GetLastError() );
        return false;
    }

    ofstream localPortFile;
    localPortFile.open((env.Approot() + "/port.tmp").c_str());

    localPortFile << pi.dwProcessId << endl;

    if (env.HasMySQL())
    {
        ZeroMemory( &mysqlpi, sizeof(mysqlpi) );
        // Start the child process.
        if(CreateProcess( NULL,   // No module name (use command line)
                (LPSTR)env.BuildMySQLCommandLine().c_str(),        // Command line
                NULL,           // Process handle not inheritable
                NULL,           // Thread handle not inheritable
                TRUE,          // Set handle inheritance to FALSE
                0,              // No creation flags
                NULL,           // Use parent's environment block
                NULL,           // Use parent's starting directory
                &si,            // Pointer to STARTUPINFO structure
                &mysqlpi )      // Pointer to PROCESS_INFORMATION structure
            )
        {
            localPortFile << mysqlpi.dwProcessId << endl;
        }
    }
    localPortFile.close();

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, 100 );
    if (env.HasMySQL())
        WaitForSingleObject( mysqlpi.hProcess, 100 );

    // Close process and thread handles.
    CloseHandle( pi.hThread );
    CloseHandle( pi.hProcess );
    if (env.HasMySQL())
    {
        CloseHandle( mysqlpi.hThread );
        CloseHandle( mysqlpi.hProcess );
    }
    return true;
}

void StopProcess(int pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess != NULL)
    {
        cout << "Stopping process with ID: " << pid << endl;
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
    }
}

void CloseChildProcesses()
{
    PROCESSENTRY32 entry;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "httpd.exe" ) == 0)
            {
                HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                PROCESSENTRY32 pe = { 0 };
                pe.dwSize = sizeof(PROCESSENTRY32);

                if( Process32First(h, &pe)) {
                    do {
                        if (pe.th32ProcessID == entry.th32ProcessID) {
                            StopProcess(entry.th32ProcessID);
                        }
                    } while( Process32Next(h, &pe));
                }

                CloseHandle(h);
            }
        }
    }
    CloseHandle(snapshot);
}

void WebServer::Stop(Environment& env)
{
    std::ifstream localPortFile ((env.Approot() + "/port.tmp").c_str(), std::ifstream::in);
    std::string line;

    while (localPortFile.good())
    {
        getline(localPortFile, line);
        int processId = atoi(line.c_str());
        StopProcess(processId);
    }
    localPortFile.close();
    remove((env.Approot() + "/port.tmp").c_str());

    // Nu gaan we child processen zoeken die gestart zijn
    CloseChildProcesses();
}
