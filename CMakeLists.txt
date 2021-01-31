cmake_minimum_required(VERSION 3.17)
project(FastTalk)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Threads)

add_executable(FastTalk
        CMakeLists.txt
        main.c VM/FST_array.h VM/FST_array.c VM/FST_stdlib.c VM/FST_stdlib.h VM/FST_object.c VM/FST_object.h VM/FST_env.c VM/FST_env.h VM/FST_class.c VM/FST_class.h VM/FST_str.c VM/FST_str.h VM/FST_interpreter.c VM/FST_interpreter.h FST.h VM/FST_msg.c VM/FST_msg.h
        VM/Ft_MemPool.c VM/Ft_MemPool.h
        NativeObjects/Ft_IntObj.c NativeObjects/Ft_IntObj.h
        tinycthread/tinycthread.c tinycthread/tinycthread.h)

#INCLUDE(FindPkgConfig)
#
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LDFLAGS})
