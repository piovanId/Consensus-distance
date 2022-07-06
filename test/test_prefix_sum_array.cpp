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

namespace pathsprefixsumarrays {

    class PrefixSumArraysTest : public ::testing::Test {
        // We need this due to memory problem in FastLocate
        std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>> *gfa_parses;
    protected:
        // Class members declared here can be used by all tests in the test suite
        // Vector of prefix sums arrays for each graph, prefix_sums_arrays[i] is referred to the graph i in
        // gfa_files_path.
        std::vector<PathsPrefixSumArrays*> *prefix_sums_arrays;

        PrefixSumArraysTest() {
            prefix_sums_arrays = new std::vector<PathsPrefixSumArrays*>();
            gfa_parses = new std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>>();

            std::unique_ptr<gbwtgraph::GBWTGraph> graph;

            // Name of the graph examples for testing
            std::vector<std::string> gfa_files_paths = {"../test/one_node_acyclic.gfa",
                                                        "../test/one_node_cyclic.gfa",
                                                        "../test/acyclic_graph_even_paths.gfa",
                                                        "../test/acyclic_graph_odd_paths.gfa",
                                                        "../test/cyclic_graph_even_paths.gfa",
                                                        "../test/cyclic_graph_odd_paths.gfa"};

            // Creating vector of prefix sum array for each graph
            for (int i = 0; i < gfa_files_paths.size(); ++i) {
                (*gfa_parses).push_back(std::move(gbwtgraph::gfa_to_gbwt(gfa_files_paths[i])));
                graph.reset(new gbwtgraph::GBWTGraph(*((*gfa_parses)[i].first),
                                                     *((*gfa_parses)[i].second)));

                prefix_sums_arrays->push_back(new PathsPrefixSumArrays(*graph));
            }
        }

        ~PrefixSumArraysTest() override {
            // You can do clean-up work that doesn't throw exceptions here.
            for (auto psa: *prefix_sums_arrays) {
                delete psa;
            }
            prefix_sums_arrays->clear();
        }
    };


    void ASSERT_PSA_MEMBERS_NE_NULLPTR(PathsPrefixSumArrays const &psa) {
        // googletest has the assumption that you put the expected value first
        ASSERT_NE(nullptr, psa.get_fast_locate());
        ASSERT_NE(nullptr, psa.get_prefix_sum_arrays());
    }


    /**
     * Test constructors
     */
    TEST_F(PrefixSumArraysTest, CreationPrefixSumArrayTest) {
        std::unique_ptr<PathsPrefixSumArrays> psa_default = std::unique_ptr<PathsPrefixSumArrays>(
                new PathsPrefixSumArrays());
        ASSERT_EQ(nullptr, psa_default->get_fast_locate());
        ASSERT_EQ(nullptr, psa_default->get_prefix_sum_arrays());

        // The prefix sums array is already been created in the constructor of the test.
        for (auto psa: *prefix_sums_arrays) {
            ASSERT_PSA_MEMBERS_NE_NULLPTR(*psa);
        }
    }

    TEST_F(PrefixSumArraysTest, GetAllNodeDistanceInAPath) {
        /*std::vector<size_t>* distances = (*prefix_sums_arrays)[0]->get_all_nodes_distances_in_path( gbwt::node_type node_1,
                                                                                    gbwt::node_type node_2,
                                                                                    size_t path_id)*/

        // test nome nodi
        //std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>> gfa_parse;
        //std::unique_ptr<gbwtgraph::GBWTGraph> graph;
        //std::vector<gbwt::size_type> nodes of the graph

        //gfa_parse = std::move(gbwtgraph::gfa_to_gbwt("../test/one_node_acyclic.gfa"));

        //graph.reset(new gbwtgraph::GBWTGraph(*(gfa_parse.first), *(gfa_parse.second)));

        /*
         * All possible nodes in the test files are:
         * 2 = AGTCATACGG
         * 4 = C
         * 6 = G
         * 8 = AAAA
         * 10 = AG
         * 12 = T
         * In the one node graph:
         * 2 = G
         */

        // One node acyclic graph, one path, all distances
        //PathsPrefixSumArrays* psa = new PathsPrefixSumArrays(*graph);
        //std::vector<size_t>* distances = psa->get_all_nodes_distances_in_path(2,2,0);

        //std::vector<size_t>* distances = (*(*prefix_sums_arrays)[0]).get_all_nodes_distances_in_path(2,2,0);
        gbwt::node_type n1 = 2;
        gbwt::node_type n2 = 2;

        //PROBLEMA NELLA FUNZIONE QUANDO CHIAMA get_positions_of_a_node_in_path
        auto distances = (*(*prefix_sums_arrays)[0]).get_all_nodes_distances_in_path(n1,n2,0);

        //(*prefix_sums_arrays)[i]->get_distance_between_positions_in_path(0,3,0));

        //ASSERT_EQ(1, distances->size()); // only one distance computable because there is only one path of length 1
        //ASSERT_EQ(0, distances->at(0)); // distances between two same positions in a path is 0

        //distances->clear();

        /*for (gbwt::size_type i = 0; i < (gfa_parse.first)->sequences(); i += 2) {
            // += 2 because the id of the paths is multiple of two, every path has its reverse path and in GBWTGraph this
            // is the representation
            auto path = graph->index->extract(i); // Attention: it's the sequence representation

            graph->index->sigma();

            std::cout << "PATH: " << i << ", NODES: ";
            for (gbwt::size_type j = 0; j < path.size(); ++j) {
                std::cout << " " << path[j];
            }


            std::cout << std::endl;

        }*/
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
