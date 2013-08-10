cmake_minimum_required (VERSION 2.6)

project (httpd2go)

include_directories("${CMAKE_SOURCE_DIR}/../common/")

set(src_app
        arguments.cpp
        default.conf.cpp
        environment.cpp
        main.cpp
        php.ini.cpp
        webserver.cpp
        ../common/tokenizer.cpp
        ../common/log.cpp
)

add_executable(httpd2go ${src_app})

#target_link_libraries (httpd2go Psapi.lib)
