cmake_minimum_required(VERSION 3.8)

set(this utf8)

set(CMAKE_CXX_STANDARD 11)

set(Inc
        inc/utf8/utf8.hpp
    )

set(Src
        src/utf8.cpp
    )

add_library(${this} STATIC ${Src})

add_subdirectory(tests)

