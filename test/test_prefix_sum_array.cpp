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

namespace my{
namespace _test_paths_prefix_sum_arrays{
    namespace  {
        class PrefixSumArraysTest : public ::testing::Test {
        private:
            // We need this to not loose the index and the sequence source because the gfa_parse_acyclic is on the stack
            // and after the constructor is called it is destroyed and with it the pointers.
            std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<SequenceSource>> gfa_parse_cyclic;
            std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<SequenceSource>> gfa_parse_acyclic;

        protected:
            // Class members declared here can be used by all tests in the test suite
            std::unique_ptr<GBWTGraph> cyclic_graph;
            std::unique_ptr<GBWTGraph> acyclic_graph;

            PrefixSumArraysTest() {
                // Cyclic graph
                std::string path_gfa_cyclic = "../test/gfa_with_reference.gfa";
                gfa_parse_cyclic = std::move(gbwtgraph::gfa_to_gbwt(path_gfa_cyclic));
                cyclic_graph.reset(new GBWTGraph(*(gfa_parse_cyclic.first), *(gfa_parse_cyclic.second)));

                // Acyclic graph
                std::string path_gfa_acyclic = "../test/gfa_with_reference.gfa";
                gfa_parse_acyclic = std::move(gfa_to_gbwt(path_gfa_acyclic));
                acyclic_graph.reset(new GBWTGraph(*(gfa_parse_acyclic.first), *(gfa_parse_acyclic.second)));
            }

            ~PrefixSumArraysTest() override {
                // You can do clean-up work that doesn't throw exceptions here.
                // Acyclic delete
            }
        };


        TEST_F(PrefixSumArraysTest, CreationPrefixSumArrayTest) {
            PathsPrefixSumArrays* psa_default = new PathsPrefixSumArrays());
            ASSERT_EQ(psa_default->get_fast_locate(), nullptr);
            ASSERT_EQ(psa_default->get_prefix_sum_arrays(), nullptr);


            PathsPrefixSumArrays *psa_cyclic = new PathsPrefixSumArrays((*cyclic_graph));
            ASSERT_NE(psa_cyclic->get_fast_locate(), nullptr);
            ASSERT_NE(psa_cyclic->get_prefix_sum_arrays(), nullptr);

            PathsPrefixSumArrays* psa_acyclic = new PathsPrefixSumArrays(*acyclic_graph);
            ASSERT_NE(psa_acyclic->get_fast_locate(), nullptr);
            ASSERT_NE(psa_acyclic->get_prefix_sum_arrays(), nullptr);

            delete psa_default;
            psa_default = nullptr;

            delete psa_cyclic;
            psa_cyclic = nullptr;

            delete psa_acyclic;
            psa_acyclic = nullptr;
        }
    }
}
}



int main(int argc, char **argv)  {
    /**
     * std::unique_ptr<PathsPrefixSumArrays> psa_default(new PathsPrefixSumArrays());
     * THIS LINE DOESN'T WORK, DON'T DELETE ASK ADAM ON ADAM OFFICE HOURS HOW TO SOLVE IT
     */


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
