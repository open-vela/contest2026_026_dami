cmake_minimum_required(VERSION 3.10)

project(cm_server)

find_package(X11 REQUIRED)

add_executable(cm_server
    server.c
    socket_tools/socket_tools.c
    picture.c
)

target_link_libraries(cm_server PRIVATE X11::X11)

