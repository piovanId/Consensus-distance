// Standard
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

// GBWTGraph
#include <gbwtgraph/gbwtgraph.h>
#include <gbwtgraph/gfa.h>

// GBWT
#include <gbwt/fast_locate.h>
#include <gbwt/gbwt.h>

// Sdsl
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/sd_vector.hpp>

// Custom
#include "include/consensus_distance/pruned_graph.h"
#include "include/consensus_distance/paths_prefix_sum_arrays.h"

// test
#include <gtest/gtest.h>

/**
0th graph:
2[1],

----------------------
1th graph:
2[1], 2[1],

----------------------
2th graph:
2[10], 4[1], 6[1], 10[2], 12[1],
2[10], 6[1], 8[5],
2[10], 4[1], 6[1], 8[5],
2[10], 6[1], 10[2], 12[1],

----------------------
3th graph:
2[10], 4[1], 6[1], 10[2], 12[1],
2[10], 6[1], 8[5],
2[10], 4[1], 6[1], 8[5],

----------------------
4th graph:
2[10], 4[1], 6[1], 10[2], 12[1],
2[10], 6[1], 8[5],
2[10], 4[1], 6[1], 8[5],
2[10], 6[1], 10[2], 12[1],
2[10], 4[1], 6[1], 2[10], 6[1], 6[1], 8[5],
2[10], 6[1], 6[1], 10[2], 12[1],

----------------------
5th graph:
2[10], 4[1], 6[1], 10[2], 12[1],
2[10], 6[1], 8[5],
2[10], 4[1], 6[1], 8[5],
2[10], 6[1], 10[2], 12[1],
2[10], 4[1], 6[1], 2[10], 6[1], 6[1], 8[5],

----------------------
*/
namespace _test_paths_prefix_sum_arrays {
    class PrefixSumArraysTest : public ::testing::Test {
    protected:
        GBWTGraph* cyclic_graph;
        GBWTGraph* acyclic_graph;

        PrefixSumArraysTest(): acyclic_graph(nullptr), cyclic_graph(nullptr) {}

        ~PrefixSumArraysTest() override {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        // If the constructor and destructor are not enough for setting up
        // and cleaning up each test, you can define the following methods:

        void SetUp() override {
            // Code here will be called immediately after the constructor (right
            // before each test).


                auto gfa_parse = gfa_to_gbwt("/home/andrea/vg/test/graphs/gfa_with_reference.gfa");

                        //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/tiny/tiny.gfa");
                    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/cactus-BRCA2.gfa");
                    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference.gfa");
                    auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference_cycle.gfa");

        }

        void TearDown() override {
            // Code here will be called immediately after each test (right
            // before the destructor).
        }

        // Class members declared here can be used by all tests in the test suite
        // for Foo.
    };
}
int main() {
    // Choosing the file based on the system
    #ifdef __linux__
        auto gfa_parse = gfa_to_gbwt("/home/andrea/vg/test/graphs/gfa_with_reference.gfa");
    #else
        //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/tiny/tiny.gfa");
        //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/cactus-BRCA2.gfa");
        //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference.gfa");
        auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/gfa_with_reference_cycle.gfa");
    #endif


    std::cout << "Hello, Santa Cruz!" << std::endl;





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
