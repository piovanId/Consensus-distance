cmake_minimum_required(VERSION 3.22)
project(Consensus_distance LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(Consensus_distance test/test_prefix_sum_array.cpp include/consensus_distance/consensus_distance.h src/paths_prefix_sum_arrays.cpp include/consensus_distance/paths_prefix_sum_arrays.h src/pruned_graph.cpp include/consensus_distance/pruned_graph.h test/test_prefix_sum_array.cpp)
include_directories(~/include)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(Consensus_distance PUBLIC OpenMP::OpenMP_CXX)
endif()

link_directories(~/lib)


target_link_libraries(Consensus_distance PRIVATE  $ENV{HOME}/lib/libgbwtgraph.a  $ENV{HOME}/lib/libgbwt.a $ENV{HOME}/lib/libhandlegraph.a $ENV{HOME}/lib/libsdsl.a $ENV{HOME}/lib/libgtest.a)




