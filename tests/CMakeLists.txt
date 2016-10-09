cmake_minimum_required(VERSION 2.8.12)
project(tests)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES
        TestInterface.cpp
        ../file.cpp
        )

add_executable(tests ${SOURCE_FILES})
