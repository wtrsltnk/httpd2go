#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "environment.h"

class WebServer
{
public:
    WebServer();
    virtual ~WebServer();

    bool Start(Environment& env);
    void Stop(Environment& env);
};

#endif // WEBSERVER_H
