cmake_minimum_required(VERSION 3.8)

set(this runTests)

set(CMAKE_CXX_STANDARD 11)

set(Src
        utf8-tests.cpp
    )

# Locate GTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(${this} ${Src})

# Link runTests with what we want to test and the GTest and pthread library
target_link_libraries(${this} ${GTEST_LIBRARIES} pthread)


