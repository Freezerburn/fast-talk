cmake_minimum_required(VERSION 3.14)
project(FastTalk)

set(CMAKE_CXX_STANDARD 14)

add_executable(FastTalk
        CMakeLists.txt
        main.c)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LDFLAGS})
