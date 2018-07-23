cmake_minimum_required(VERSION 3.8)
project(AAProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PEA_FIND_SCC3/Pea_find_scc3.cpp NUUT_FIND_SCC2/Nuut_find_scc2.cpp TARJ_FIND_SCC1/Tarj_find_scc1.cpp)

include_directories(utilities)
include_directories(PEA_FIND_SCC3)
include_directories(NUUT_FIND_SCC2)
include_directories(TARJ_FIND_SCC1)

# The environment variable "BOOST_ROOT=/usr/local/boost_1_67_0" needs to be set
find_package(Boost 1.67.0 COMPONENTS timer)
if (Boost_FOUND)
    message(STATUS "Boost include directory: " ${Boost_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
else ()
    messsage(FATAL_ERROR "Boost not foud!")
endif ()

add_executable(AAProject ${SOURCE_FILES})

target_link_libraries(AAProject ${Boost_LIBRARIES})