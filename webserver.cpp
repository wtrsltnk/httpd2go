#include "webserver.h"
#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <iostream>

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

    if (env.hasMySQL())
    {
        ZeroMemory( &mysqlpi, sizeof(mysqlpi) );
        // Start the child process.
        CreateProcess( NULL,   // No module name (use command line)
            (LPSTR)env.BuildMySQLCommandLine().c_str(),        // Command line
            NULL,           // Process handle not inheritable
            NULL,           // Thread handle not inheritable
            FALSE,          // Set handle inheritance to FALSE
            0,              // No creation flags
            NULL,           // Use parent's environment block
            NULL,           // Use parent's starting directory
            &si,            // Pointer to STARTUPINFO structure
            &mysqlpi )      // Pointer to PROCESS_INFORMATION structure
        ;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );
    if (env.hasMySQL())
        WaitForSingleObject( mysqlpi.hProcess, INFINITE );

    // Close process and thread handles.
    CloseHandle( pi.hThread );
    CloseHandle( pi.hProcess );
    if (env.hasMySQL())
    {
        CloseHandle( mysqlpi.hThread );
        CloseHandle( mysqlpi.hProcess );
    }
    return true;
}

void WebServer::Stop()
{
    PROCESSENTRY32 entry;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if (Process32First(snapshot, &entry) == TRUE)
    {
        while (Process32Next(snapshot, &entry) == TRUE)
        {
            if (stricmp(entry.szExeFile, "httpd.exe" ) == 0 || stricmp(entry.szExeFile, "mysqld.exe" ) == 0)
            {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                if (hProcess != NULL)
                {
                    cout << "Stopping " << entry.szExeFile <<  " with processID: " << entry.th32ProcessID << endl;
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
                else
                {
                    cout << "Unable to open process " << GetLastError() << endl;
                }
            }
        }
    }
    CloseHandle(snapshot);
}
