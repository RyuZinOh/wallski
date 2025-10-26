cmake_minimum_required(VERSION 4.1.2 )
project(wallski C)

set(CMAKE_C_STANDARD 99)
include_directories(include)

add_executable(wallski
    src/main.c
    src/wlr-layer-shell-unstable-v1-client-protocol.c
    src/xdg-shell-client-protocol.c
)

target_link_libraries(wallski
    wayland-client
    wayland-egl
    EGL
    GLESv2
    m
)



add_executable(wallski-daemon
    src/wallski-daemon.c
    src/wlr-layer-shell-unstable-v1-client-protocol.c
    src/xdg-shell-client-protocol.c
)

target_link_libraries(wallski-daemon
    wayland-client
    wayland-egl
    EGL
    GLESv2
    m
)



