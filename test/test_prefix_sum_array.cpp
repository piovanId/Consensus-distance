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
        protected:
            // Class members declared here can be used by all tests in the test suite
            // Vector of prefix sums arrays for each graph, prefix_sums_arrays[i] is referred to the graph i in
            // gfa_files_path.
            std::vector<PathsPrefixSumArrays*>* prefix_sums_arrays = new std::vector<PathsPrefixSumArrays*>();

            PrefixSumArraysTest() {
                std::unique_ptr<GBWTGraph> graph;
                std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<SequenceSource>> gfa_parse;

                // Name of the graph examples for testing
                std::vector<std::string> gfa_files_paths = {"../test/one_node_acyclic.gfa",
                                                            "../test/one_node_cyclic.gfa",
                                                            "../test/acyclic_graph_even_paths.gfa",
                                                            "../test/acyclic_graph_odd_paths.gfa",
                                                            "../test/cyclic_graph_even_paths.gfa",
                                                            "../test/cyclic_graph_odd_paths.gfa"};

                // Creating vector of prefix sum array for each graph
                for(int i=0; i < gfa_files_paths.size(); ++i){
                    gfa_parse = std::move(gbwtgraph::gfa_to_gbwt(gfa_files_paths[i]));
                    graph.reset(new GBWTGraph(*(gfa_parse.first),
                                              *(gfa_parse.second)));
                    prefix_sums_arrays->push_back(new PathsPrefixSumArrays(*graph));
                }
            }

            ~PrefixSumArraysTest() override {
                // You can do clean-up work that doesn't throw exceptions here.
                for(auto psa : *prefix_sums_arrays){
                    delete psa;
                }
                prefix_sums_arrays->clear();
            }
        };


        void ASSERT_PSA_MEMBERS_NE_NULLPTR(PathsPrefixSumArrays const& psa) {
            // googletest has the assumption that you put the expected value first
            ASSERT_NE(nullptr, psa.get_fast_locate());
            ASSERT_NE(nullptr, psa.get_prefix_sum_arrays());
        }


        /**
         * Test constructors
         */
        TEST_F(PrefixSumArraysTest, CreationPrefixSumArrayTest) {
            std::unique_ptr<PathsPrefixSumArrays>  psa_default = std::unique_ptr<PathsPrefixSumArrays>(new PathsPrefixSumArrays());
            ASSERT_EQ(nullptr, psa_default->get_fast_locate());
            ASSERT_EQ(nullptr, psa_default->get_prefix_sum_arrays());

            // The prefix sums array is already been created in the constructor of the test.
            for(auto psa : *prefix_sums_arrays){
                ASSERT_PSA_MEMBERS_NE_NULLPTR(*psa);
            }
        }


        TEST_F(PrefixSumArraysTest, GetAllNodeDistanceInAPath){
            /*std::vector<size_t>* distances = (*prefix_sums_arrays)[0]->get_all_nodes_distances_in_path( gbwt::node_type node_1,
                                                                                        gbwt::node_type node_2,
                                                                                        size_t path_id)*/

            // test nome nodi
            std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<SequenceSource>> gfa_parse;
            std::unique_ptr<GBWTGraph> graph;
            std::unique_ptr<std::vector<P>>

            gfa_parse = std::move(gbwtgraph::gfa_to_gbwt("../test/acyclic_graph_odd_paths.gfa"));

            graph.reset(new GBWTGraph(*(gfa_parse.first),*(gfa_parse.second)));

            for(gbwt::size_type i = 0; i < (gfa_parse.first)->sequences(); i += 2) {
                // += 2 because the id of the paths is multiple of two, every path has its reverse path and in GBWTGraph this
                // is the representation
                auto path = graph->index->extract(i); // Attention: it's the sequence representation

                graph->index->sigma();

                std::cout << "PATH: " << i << ", NODES: ";
                for(gbwt::size_type j = 0; j < path.size(); ++j) {
                    std::cout << " " << path[j];
                }


                std::cout << std::endl;

            }
        }


    }
}
}



int main(int argc, char **argv)  {
    /**
     * std::unique_ptr<PathsPrefixSumArrays> psa_ldefault(new PathsPrefixSumArrays());
     * THIS LINE DOESN'T WORK, DON'T DELETE ASK ADAM ON ADAM OFFICE HOURS HOW TO SOLVE IT
     */
    
    std::cout << "Hello, Santa Cruzsdd!" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
