#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include "gbwtgraph/gbwtgraph.h"
#include "gbwtgraph/gfa.h"
#include "gbwt/gbwt.h"
#include "include/consensus_distance/paths_prefix_sum_arrays.h"
#include "sdsl/sd_vector.hpp"
#include "sdsl/int_vector.hpp"
#include "sdsl/bit_vectors.hpp"
#include "gbwt/fast_locate.h"
#include "include/consensus_distance/pruned_graph.h"




int main() {
    std::string s1= "c";
    std::cout << "Hello, Santa Cruz!" << std::endl;



    auto gfa_parse = gfa_to_gbwt("/home/andrea/vg/test/graphs/gfa_with_reference.gfa");
   // auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/tiny/tiny.gfa");
    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/cactus-BRCA2.gfa");
   //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference.gfa");


    const gbwt::GBWT& index = *(gfa_parse.first);
    GBWTGraph graph(*(gfa_parse.first), *(gfa_parse.second));

    PathsPrefixSumArrays *a = new PathsPrefixSumArrays(graph);
    auto distances = a->get_all_nodes_distances_in_path(8, 12, 0);


    std::cout << a->toString() << std::endl;

    std::cout << "DISTANCES (size)" << distances->size() << " ----> ";
    for(int i=0; i < distances->size(); ++i){
        std::cout << std::to_string((*distances)[i]) << " ";
    }

    PrunedGraph pruned(graph);
    //std::cout << "Distance: " << std::to_string(a->get_distance_between_positions_in_path(5,6,0));

    delete a;
    a = nullptr;
    delete distances;
    distances = nullptr;
}
