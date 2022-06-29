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

    std::cout << "Hello, Santa Cruz!" << std::endl;



     auto gfa_parse = gfa_to_gbwt("/home/andrea/vg/test/graphs/gfa_with_reference.gfa");
   // auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/tiny/tiny.gfa");
    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/cactus-BRCA2.gfa");
    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference.gfa");
    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference_cycle.gfa");



    const gbwt::GBWT& index = *(gfa_parse.first);
    GBWTGraph graph(*(gfa_parse.first), *(gfa_parse.second));

    PathsPrefixSumArrays *prefixSumArrays = new PathsPrefixSumArrays(graph);
    gbwt::FastLocate*  localfastlocate = new gbwt::FastLocate(*graph.index);

    auto posizioninodo18 = prefixSumArrays->get_all_node_positions(12);


    auto temp =localfastlocate->decompressSA(12);

    auto distances = prefixSumArrays->get_all_nodes_distances_in_path(18, 2, 0);

    auto position_in_all_sequences = prefixSumArrays->get_all_node_positions(2);

    auto distances_in_graph = prefixSumArrays->get_all_nodes_distances(18,2);


    delete prefixSumArrays;
    prefixSumArrays = nullptr;
    delete distances;
    distances = nullptr;
    return 0;
}
