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
        main.c VM/Ft_array.h VM/Ft_array.c VM/Ft_stdlib.c VM/Ft_stdlib.h VM/Ft_object.c VM/Ft_object.h VM/Ft_env.c VM/Ft_env.h VM/Ft_class.c VM/Ft_class.h VM/Ft_str.c VM/Ft_str.h VM/Ft_interpreter.c VM/Ft_interpreter.h FastTalk.h VM/Ft_msg.c VM/Ft_msg.h
        VM/Ft_MemPool.c VM/Ft_MemPool.h
        NativeObjects/Ft_IntObj.c NativeObjects/Ft_IntObj.h
        tinycthread/tinycthread.c tinycthread/tinycthread.h)

#INCLUDE(FindPkgConfig)
#
#PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
#
#INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS})
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LDFLAGS})
