cmake_minimum_required (VERSION 2.6)

project (httpd2go)

set(src_app
    common/tokenizer.cpp
    common/log.cpp
    arguments.cpp
    default.conf.cpp
    environment.cpp
    main.cpp
    php.ini.cpp
    systray.cpp
    webserver.cpp
)

add_executable(httpd2go ${src_app})

#target_link_libraries (httpd2go Psapi.lib)
