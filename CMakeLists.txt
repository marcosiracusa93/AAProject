cmake_minimum_required(VERSION 3.8)
project(AAProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PEA_FIND_SCC3/Pea_find_scc3.cpp)

set(Boost_INCLUDE_DIRS /usr/local/boost_1_67_0)

include_directories(DoubleStack.hpp)
include_directories(PEA_FIND_SCC3/Pea_find_scc3.hpp)
include_directories(typedefs.hpp)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(AAProject ${SOURCE_FILES})