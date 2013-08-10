#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <limits>

#include "arguments.h"
#include "environment.h"
#include "webserver.h"

using namespace std;

int main(int argc, char*argv[])
{
    Arguments args(argc, argv);
    WebServer srvr;

    if (args.Ok())
    {
        Environment env;

        if (env.Ok())
        {
            srvr.Stop();
            string url = env.CreateProject(args.ProjectPath(), args.ProjectName());

            if (url != "")
            {
                cout << "Starting webserver. Your project will be ready at: " << url << endl;
                cout << "Remove the project by running: httphp.exe --delete " << args.ProjectName() << endl;
                cout << "Stop the webserver by stopping this instance or running: httphp.exe --stop" << endl;
                srvr.Start(env);
                cout << "Bye!" << endl;
            }
        }
    }
    else if (args.Delete())
    {
        Environment env;

        if (env.Ok())
        {
            srvr.Stop();
            if (env.DeleteProject(args.ProjectName()))
                cout << "Project " << args.ProjectName() << " deleted." << endl;

            cout << "Starting webserver." << endl;
            cout << "Stop the webserver by stopping this instance or running: httphp.exe --stop" << endl;
            srvr.Start(env);
            cout << "Bye!" << endl;
        }
    }
    else if (args.Stop())
    {
        cout << "Stopping webserver." << endl;
        srvr.Stop();
    }
    return 0;
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

