cmake_minimum_required(VERSION 3.17)
project(FastTalk)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(FastTalk
        CMakeLists.txt
        main.c FST_array.h FST_array.c FST_stdlib.c FST_stdlib.h FST_object.c FST_object.h FST_env.c FST_env.h FST_class.c FST_class.h FST_str.c FST_str.h FST_interpreter.c FST_interpreter.h FST.h FST_msg.c FST_msg.h
        NativeObjects/Ft_IntObj.c NativeObjects/Ft_IntObj.h)

#INCLUDE(FindPkgConfig)
#
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LDFLAGS})
