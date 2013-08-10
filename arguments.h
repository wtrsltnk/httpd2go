#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>

class Arguments
{
public:
    Arguments(int argc, char* argv[]);
    virtual ~Arguments();

    void PrintHelp();

    bool Ok() { return this->_ok; }
    bool Stop() { return this->_stop; }
    bool Delete() { return this->_delete; }
    std::string& ProjectPath() { return this->_projectPath; }
    std::string& ProjectName() { return this->_projectName; }

private:
    bool _ok;
    bool _stop;
    bool _delete;
    std::string _projectPath;
    std::string _projectName;

};

#endif // ARGUMENTS_H
