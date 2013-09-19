#include "arguments.h"
#include <iostream>

using namespace std;

Arguments::Arguments(int argc, char* argv[])
    : _ok(false), _stop(false), _delete(false)
{
    if (argc >= 2)
    {
        if (string(argv[1]) == "--help")
            this->PrintHelp();
        else if (string(argv[1]) == "--stop")
            this->_stop = true;
        else if (string(argv[1]) == "--delete")
        {
            if (argc > 2)
            {
                this->_projectName = argv[2];
                this->_delete = true;
            }
        }
        else
        {
            int i = 0;
            while (argv[1][i] != '\0')
                if (argv[1][i++] == '\\') argv[1][i-1] = '/';

            this->_projectPath = argv[1];
            this->_ok = true;

            if (argc > 2)
                this->_projectName = argv[2];
            else
                this->_projectName = this->_projectPath.substr(this->_projectPath.find_last_of('/') + 1);

            string tmp = this->_projectName;
            this->_projectName = "";
            for (unsigned int i = 0; i < tmp.size(); i++)
            {
                char c = tmp[i];
                if ((c >= 'a' && c <= 'z')
                        || (c >= 'A' && c <= 'Z')
                        || (c >= '0' && c <= '9')
                        || (c == '-')
                        || (c == '_'))
                {
                    this->_projectName += tmp[i];
                }
            }
        }
    }
}

Arguments::~Arguments()
{ }

void Arguments::PrintHelp()
{

}
