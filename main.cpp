#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <limits>

#include "arguments.h"
#include "environment.h"
#include "webserver.h"
#include "systray.h"

using namespace std;

int main(int argc, char*argv[])
{
    HWND hWnd = GetConsoleWindow();
    ShowWindow( hWnd, SW_HIDE );

    Arguments args(argc, argv);
    WebServer srvr;
    Environment env;

    if (args.Ok())
    {
        if (env.Ok())
        {
            srvr.Stop(env);
            string url = env.CreateProject(args.ProjectPath(), args.ProjectName());

            if (url != "")
            {
                cout << "Starting webserver. Your project will be ready at: " << url << endl;
                cout << "Remove the project by running: httpd2go.exe --delete " << args.ProjectName() << endl;
                cout << "Stop the webserver by stopping this instance or running: httpd2go.exe --stop" << endl;
                srvr.Start(env);
                cout << "Bye!" << endl;
            }
        }
    }
    else if (args.Delete())
    {
        if (env.Ok())
        {
            srvr.Stop(env);
            if (env.DeleteProject(args.ProjectName()))
                cout << "Project " << args.ProjectName() << " deleted." << endl;

            cout << "Starting webserver." << endl;
            cout << "Stop the webserver by stopping this instance or running: httpd2go.exe --stop" << endl;
            srvr.Start(env);
            cout << "Bye!" << endl;
        }
    }
    else if (args.Stop())
    {
        cout << "Stopping webserver." << endl;
        if (env.HasMySQL())
            cout << "Stopping MySQL." << endl;
        srvr.Stop(env);
    }
    else
    {
        srvr.Start(env);
    }

    return StartSystray(&srvr, &env);
}


/*
****** Construct the default.conf.cpp with the following C# code:

using System;
using System.Collections.Generic;
using System.Text;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            string[] lines = System.IO.File.ReadAllLines(@"C:\Documents and Settings\Administrator\My Documents\Downloads\httphp\_default.conf");

            List<string> writeLines = new List<string>();
            writeLines.Add("#include <iostream>");
            writeLines.Add("#include <fstream>");
            writeLines.Add("");
            writeLines.Add("void WriteDefaultConf(std::ofstream& stream) {");
            foreach (string line in lines)
                writeLines.Add("\tstream << \"" + line.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\\n\";");
            writeLines.Add("}");

            System.IO.File.WriteAllLines(@"D:\Code\ballpark-svn\httphp\default.conf.cpp", writeLines.ToArray());
        }
    }
}

****** Construct the php.ini.cpp with the following C# code:

using System;
using System.Collections.Generic;
using System.Text;

namespace ConsoleApplication1
{
    class Program
    {
        static void Main(string[] args)
        {
            string[] lines = System.IO.File.ReadAllLines(@"C:\Documents and Settings\Administrator\My Documents\Downloads\httphp\php.ini");

            List<string> writeLines = new List<string>();
            writeLines.Add("#include <iostream>");
            writeLines.Add("#include <fstream>");
            writeLines.Add("");
            writeLines.Add("void WritePhpIni(std::ofstream& stream) {");
            foreach (string line in lines)
                writeLines.Add("\tstream << \"" + line.Replace("\\", "\\\\").Replace("\"", "\\\"") + "\\n\";");
            writeLines.Add("}");

            System.IO.File.WriteAllLines(@"D:\Code\ballpark-svn\httphp\php.ini.cpp", writeLines.ToArray());
        }
    }
}

*/

