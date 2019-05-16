cmake_minimum_required(VERSION 3.14)
project(FastTalk)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(FastTalk
        CMakeLists.txt
        main.c FST_array.h FST_array.c FST_stdlib.c FST_stdlib.h FST_object.c FST_object.h FST_env.c FST_env.h FST_class.c FST_class.h FST_str.c FST_str.h FST_interpreter.c FST_interpreter.h FST_val.c FST_val.h FST.h FST_msg.c FST_msg.h)

#INCLUDE(FindPkgConfig)
#
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LDFLAGS})
