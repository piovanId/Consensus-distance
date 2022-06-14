cmake_minimum_required(VERSION 3.22)
project(Consensus_distance)

set(CMAKE_CXX_STANDARD 14)

add_library(Consensus_distance library.cpp test/test.cpp)


find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Consensus_distance PUBLIC OpenMP::OpenMP_CXX)
endif()

include_directories(~/include)

link_directories(~/lib)




target_link_libraries(Consensus_distance PRIVATE  $ENV{HOME}/lib/libgbwtgraph.a  $ENV{HOME}/lib/libgbwt.a $ENV{HOME}/lib/libhandlegraph.a $ENV{HOME}/lib/libsdsl.a)