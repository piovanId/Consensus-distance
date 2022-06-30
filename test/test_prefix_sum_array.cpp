/**
 * Authors:
 *  - Davide Piovani
 *  - Andrea Mariotti
 */

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
#include "../include/consensus_distance/pruned_graph.h"
#include "../include/consensus_distance/paths_prefix_sum_arrays.h"

// test
#include <gtest/gtest.h>


namespace _test_paths_prefix_sum_arrays {
    using namespace gbwtgraph;
    class PrefixSumArraysTest : public ::testing::Test {
    protected:
        PrefixSumArraysTest(): cyclic_graph(nullptr) {
            /*std::string path_gfa_acyclic = "/home/andrea/vg/test/graphs/gfa_with_reference.gfa";
            auto gfa_parse_acyclic = gfa_to_gbwt(path_gfa_acyclic);
            acyclic_graph = new GBWTGraph(*(gfa_parse_acyclic.first), *(gfa_parse_acyclic.second));*/
            std::string path_gfa_cyclic = "/home/andrea/vg/test/graphs/gfa_with_reference.gfa";
            auto gfa_parse_cyclic = gbwtgraph::gfa_to_gbwt(path_gfa_cyclic);
            cyclic_graph = new GBWTGraph(*(gfa_parse_cyclic.first), *(gfa_parse_cyclic.second));
        }

        ~PrefixSumArraysTest() override {
            // You can do clean-up work that doesn't throw exceptions here.
            /*delete acyclic_graph;
            acyclic_graph = nullptr;*/
            delete cyclic_graph;
            cyclic_graph = nullptr;
        }

        // Class members declared here can be used by all tests in the test suite
        gbwtgraph::GBWTGraph* cyclic_graph;
        //gbwtgraph::GBWTGraph* acyclic_graph;
    };



    TEST_F(PrefixSumArraysTest, CreationPrefixSumArrayTest) {
        /*PathsPrefixSumArrays* psa_default = new PathsPrefixSumArrays();
        ASSERT_EQ(psa_default->get_fast_locate(), nullptr);
        ASSERT_EQ(psa_default->get_prefix_sum_arrays(), nullptr);*/


        //PathsPrefixSumArrays* psa_cyclic = new PathsPrefixSumArrays(*cyclic_graph);
        std::cerr << "AFTER INITIALIZED gbwtgraph node count: " << std::to_string((*cyclic_graph).get_node_count()) << std::endl;
        PathsPrefixSumArrays* psa_cyclic = new PathsPrefixSumArrays(*cyclic_graph);

        //std::cerr << "[     ohhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh     ]" << std::endl;
        /*ASSERT_NE(psa_cyclic->get_fast_locate(), nullptr);
        ASSERT_NE(psa_cyclic->get_prefix_sum_arrays(), nullptr);

        PathsPrefixSumArrays* psa_acyclic = new PathsPrefixSumArrays(*acyclic_graph);
        //ASSERT_NE(psa_acyclic->get_fast_locate(), nullptr);
        //pippobaudo(psa_acyclic);
        ASSERT_NE(psa_acyclic->get_prefix_sum_arrays(), nullptr);*/
    }
}





int main(int argc, char *argv[])  {
    std::cout << "Hello, Santa Cruzsdd!" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();


    /*PathsPrefixSumArrays *prefixSumArrays = new PathsPrefixSumArrays(graph);
    gbwt::FastLocate*  localfastlocate = new gbwt::FastLocate(*graph.index);

    auto posizioninodo18 = prefixSumArrays->get_all_node_positions(12);


    auto temp =localfastlocate->decompressSA(12);

    auto distances = prefixSumArrays->get_all_nodes_distances_in_path(18, 2, 0);

    auto position_in_all_sequences = prefixSumArrays->get_all_node_positions(2);

    auto distances_in_graph = prefixSumArrays->get_all_nodes_distances(18,2);


    delete prefixSumArrays;
    prefixSumArrays = nullptr;
    delete distances;
    distances = nullptr;*/
    return 0;
}
