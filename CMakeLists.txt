cmake_minimum_required(VERSION 3.8)
project(AAProject)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp PEA_FIND_SCC3/Pea_find_scc3.cpp PEA_FIND_SCC3_OLD/Pea_find_scc3_old.cpp NUUT_FIND_SCC2/Nuut_find_scc2.cpp TARJ_FIND_SCC1/Tarj_find_scc1.cpp)

include_directories(utilities)
include_directories(PEA_FIND_SCC3)
include_directories(PEA_FIND_SCC3_OLD)
include_directories(NUUT_FIND_SCC2)
include_directories(TARJ_FIND_SCC1)

# The environment variable "BOOST_ROOT=/usr/local/boost_1_67_0" needs to be set
find_package(Boost 1.67.0 COMPONENTS timer graph)
if (Boost_FOUND)
    message(STATUS "Boost include directory: " ${Boost_INCLUDE_DIRS})
    include_directories(${Boost_INCLUDE_DIRS})
else ()
    message(FATAL_ERROR "Boost not found!")
endif ()

add_executable(AAProject ${SOURCE_FILES})

add_executable(graphGenerator utilities/graph_generator.cpp)

add_executable(nuut_find_scc2_main NUUT_FIND_SCC2/nuut_find_scc2_main.cpp NUUT_FIND_SCC2/Nuut_find_scc2.cpp)
add_executable(pea_find_scc3_main PEA_FIND_SCC3/pea_find_scc3_main.cpp PEA_FIND_SCC3/Pea_find_scc3.cpp)
add_executable(pea_find_scc3_old_main PEA_FIND_SCC3_OLD/Pea_find_scc3_old_main.cpp PEA_FIND_SCC3_OLD/Pea_find_scc3_old.cpp)
add_executable(tarj_find_scc1_main TARJ_FIND_SCC1/tarj_find_scc1_main.cpp TARJ_FIND_SCC1/Tarj_find_scc1.cpp)

target_link_libraries(AAProject ${Boost_LIBRARIES})

target_link_libraries(graphGenerator ${Boost_LIBRARIES})

target_link_libraries(nuut_find_scc2_main ${Boost_LIBRARIES})
target_link_libraries(pea_find_scc3_main ${Boost_LIBRARIES})
target_link_libraries(tarj_find_scc1_main ${Boost_LIBRARIES})