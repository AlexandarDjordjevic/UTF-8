cmake_minimum_required(VERSION 3.8)

set(This utf8)

set(CMAKE_CXX_STANDARD 11)

set(Inc
        include/utf8/utf8.hpp
    )

set(Src
        src/utf8.cpp
    )

add_library(${This} STATIC ${Src})

set_target_properties(${This} PROPERTIES
    FOLDER Libraries
)

target_include_directories(${This} PUBLIC include)

add_subdirectory(tests)