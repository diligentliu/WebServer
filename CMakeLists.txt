cmake_minimum_required(VERSION 3.22)
project(WebServer)

set(CMAKE_CXX_STANDARD 20)

set(INCLUDE_DIRS
        "code/pool"
        "code/buffer"
        "code/log"
        "code/server"
        "code/timer"
        "code/http"
        mysqlclient
)

set(SOURCE_FILES
        code/main.cpp
        code/pool/threadpool.h
        code/buffer/buffer.cpp
        code/buffer/buffer.h
        code/server/epoller.h
        code/server/epoller.cpp
        code/log/blockdeque.h
        code/log/log.cpp
        code/log/log.h
        code/timer/heaptimer.cpp
        code/timer/heaptimer.h
        code/pool/sqlconnpool.cpp
        code/pool/sqlconnpool.h
        code/pool/sqlconnRAII.h
        code/http/httpconn.cpp
        code/http/httpconn.h
        code/http/httprequest.cpp
        code/http/httprequest.h
        code/http/httpresponse.cpp
        code/http/httpresponse.h
        code/server/webserver.cpp
        code/server/webserver.h
)

link_directories(/usr/lib/x86_64-linux-gnu/)
link_libraries(mysqlclient)
include_directories(/usr/include/mysql)
add_executable(WebServer ${SOURCE_FILES})
target_include_directories(WebServer PRIVATE ${INCLUDE_DIRS})
