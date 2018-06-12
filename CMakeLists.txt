cmake_minimum_required(VERSION 3.8)
project(AAProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PEA_FIND_SCC3/Pea_find_scc3.cpp)

include_directories(DoubleStack.hpp)
include_directories(PEA_FIND_SCC3/Pea_find_scc3.hpp)
include_directories(typedefs.hpp)

find_package(Boost 1.67.0 COMPONENTS timer)
if (Boost_FOUND)
    message(STATUS "Boost include directory: " ${Boost_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
else ()
    messsage(FATAL_ERROR "Boost not foud!")
endif ()

add_executable(AAProject ${SOURCE_FILES})

target_link_libraries(AAProject ${Boost_LIBRARIES})