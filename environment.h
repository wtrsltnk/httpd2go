#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <string>

class Environment
{
public:
    Environment();
    virtual ~Environment();

    bool Ok() { return this->_ok; }
    std::string CreateProject(const std::string& projectFolder, const std::string& projectName);
    bool DeleteProject(const std::string& projectName);

    std::string BuildCommandLine();

    static bool FileExists(std::string& file);
    static bool DirectoryExists(std::string& file);

private:
    bool _ok;
    std::string _approot;
    std::string _httpd;
    std::string _php;
    std::string _phpIni;
    std::string _defaultConf;
    std::string _vhostsDirectory;
    std::string _localConf;

    std::string GrabPort(const std::string& projectName);
    std::string GeneratePort();
};

#endif // ENVIRONMENT_H
