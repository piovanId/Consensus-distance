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

//DEBUG PRINT
void printNodesInPath(std::string file_name){
    auto gfa_parse = std::move(gbwtgraph::gfa_to_gbwt(file_name));
    std::unique_ptr<gbwtgraph::GBWTGraph> graph_ = std::unique_ptr<gbwtgraph::GBWTGraph>(
            new gbwtgraph::GBWTGraph(*gfa_parse.first, *gfa_parse.second));

    /*pathsprefixsumarrays::PathsPrefixSumArrays* psa_ = new pathsprefixsumarrays::PathsPrefixSumArrays(*graph_);

    for(gbwt::size_type i = 0; i < (graph_->index)->sequences(); i += 2) {
        // += 2 because the id of the paths is multiple of two, every path has its reverse path and in GBWTGraph this
        // is the representation
        auto path = graph_->index->extract(i); // Attention: it's the sequence representation

        std::cout << "\nPath: " << std::to_string(i) << " - Nodes: ";
        for (gbwt::size_type j = 0; j < path.size(); ++j) {
            std::cout << " " << path[j] << "(len " << graph_->get_length( graph_->node_to_handle(path[j])) <<")";
        }
        std::cout << std::endl;
    }*/

    // VERIFICA CHE IL PATH SIA OK
    /*pathsprefixsumarrays::PathsPrefixSumArrays p(*graph_);
    auto map_pos = p.get_all_node_positions(2);
    auto pos = (*map_pos)[0];
    std::cout << "PATH 0 posizioni di 2: ";
    for(int i=0; i < pos->size();++i){
        std::cout << " " << (*pos)[i];
    }

    std::cout << std::endl;*/

    /*int offset = 0;
    sdsl::bit_vector psa_temp(16,0);
    auto path = graph_->index->extract(0);
    for(gbwt::size_type j = 0; j < path.size(); ++j) {
        gbwt::size_type length_of_node = graph_->get_length(graph_->node_to_handle(path[j]));
        offset += length_of_node;
        psa_temp[offset] = 1;
    }

    sdsl::sd_vector<>* v = new sdsl::sd_vector<>(psa_temp);
    std::string temp =  "\n| ";

    // Accessing KEY from element pointed by it.
    temp += std::to_string(0) + "::\t[";

    // Accessing VALUE from element pointed by it.
    for(int i=0; i< v->size(); ++i){
        temp += std::to_string((*v)[i]);
        if(i!=v->size()-1)
            temp += ", ";
    }
    temp += "]";

    std::cout << "VETTORE PATH 0: " << temp << "\n";
    delete v;*/
}


namespace pathsprefixsumarrays {

    class PrefixSumArraysTest : public ::testing::Test {

    protected:
        // We need this due to memory problem in FastLocate
        std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>> *gfa_parses;
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
            // PSA
            for (auto psa: *prefix_sums_arrays) {
                delete psa;
            }
            prefix_sums_arrays->clear();
            prefix_sums_arrays = nullptr;

            // gfa_parses
            gfa_parses->clear();
            gfa_parses = nullptr;
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
         *      In the more nodes graphs:
         *          2 = AGTCATACGG
         *          4 = C
         *          6 = G
         *          8 = AAAA
         *          10 = AG
         *          12 = T
         *      In the one node graphs:
         *          2 = G
         */

        /**
         * One node acyclic graph, one path (0), all distances between 2 and 2
         */
        gbwt::node_type n1 = 2;
        gbwt::node_type n2 = 2;

        std::unique_ptr<std::vector<size_t>> distances = std::unique_ptr<std::vector<size_t>>((*(*prefix_sums_arrays)[0]).get_all_nodes_distances_in_path(n1,n2,0));
        ASSERT_EQ(1, distances->size()); // only one distance computable because there is only one path of length 1
        ASSERT_EQ(0, distances->at(0)); // distances between two same positions in a path is 0

        /**
         * One node cyclic graph, one path (0), all distances between 2 and 2
         */
        distances.reset((*(*prefix_sums_arrays)[1]).get_all_nodes_distances_in_path(n1,n2,0));
        ASSERT_EQ(3, distances->size()); // 3 distances because d(1,1)=0, d(1,2)=0, d(2,2)=0
        for(int distance=0; distance < distances->size(); ++distance){
            ASSERT_EQ(0, distances->at(0));
        }

        /**
         * Acyclic graph, even paths (4), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         */

        struct parameters_third_graph{
            gbwt::node_type n1;
            gbwt::node_type n2;

            int assert_length_distance;
            int assert_distance;
            size_t path_id;
        };

        std::vector<parameters_third_graph> parameter_third_graph_vector = {{2,10,1,2,0},
                                                                            {2,12,1,4,0},
                                                                            {2,12,1,3, 6}};

        int number_of_tests_on_third_graph = 3;
        for(int index_test = 0; index_test < number_of_tests_on_third_graph; ++index_test){
            distances.reset((*(*prefix_sums_arrays)[2]).get_all_nodes_distances_in_path(parameter_third_graph_vector[index_test].n1,
                                                                                        parameter_third_graph_vector[index_test].n2,
                                                                                        0));
            ASSERT_EQ(parameter_third_graph_vector[index_test].assert_length_distance, distances->size());
            ASSERT_EQ(parameter_third_graph_vector[index_test].assert_distance, distances->at(0));
        }

        /*n1 = 2;
        n2 = 10;

        printNodesInPath("../test/acyclic_graph_even_paths.gfa");
        distances.reset((*(*prefix_sums_arrays)[2]).get_all_nodes_distances_in_path(n1,n2,0));
        ASSERT_EQ(1, distances->size());
        ASSERT_EQ(2, distances->at(0));

        //START PRINT DEBUG
        /*std::cout << "DISTANZE: ";
        for(auto d : *distances){
            std::cout << " " << d;
        }
        std::cout << "\n";
        //std::cout << "DISTANZA CHE IN TEORIA E' SBAGLIATA: " << (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(1,4,0) << "\n";
        std::cout << "STAMPA PSA: " << (*prefix_sums_arrays)[2]->toString() << "\n";
        std::cout << "STAMPA PSA bitvector: " << (*prefix_sums_arrays)[2]->toString_sd_vectors() << "\n";
        // END PRINT DEBUG

        n1 = 2;
        n2 = 12;

        distances.reset((*(*prefix_sums_arrays)[2]).get_all_nodes_distances_in_path(n1,n2,0));
        ASSERT_EQ(1, distances->size());
        ASSERT_EQ(4, distances->at(0));

        n1 = 2;
        n2 = 12;

        distances.reset((*(*prefix_sums_arrays)[2]).get_all_nodes_distances_in_path(n1,n2,6));
        ASSERT_EQ(1, distances->size());
        ASSERT_EQ(3, distances->at(0));*/

        //





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
/*
    TEST_F(PrefixSumArraysTest, get_distance_between_positions_in_path) {
        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            //testing the outer function
            ASSERT_EQ(0, temp->get_distance_between_positions_in_path(0, 0, 0));
            if (i > 1) {
                ASSERT_EQ(2, temp->get_distance_between_positions_in_path(0, 3, 0));
                //testing the aux function
                auto tempsa = (*temp).psa;
                sdsl::sd_vector<>::select_1_type sdb_sel(tempsa->at(0));
                ASSERT_EQ(1, temp->get_distance_between_positions_in_path_aux(1, 3, sdb_sel));
                ASSERT_EQ(3, temp->get_distance_between_positions_in_path_aux(1, 4, sdb_sel));
            }
        }
    }
*/
    TEST_F(PrefixSumArraysTest, get_all_nodes_distances) {
        size_t A = 2;
        size_t B = 6;

        // print everything



        std::vector<std::vector<size_t>> check = {{},
                                                  {},
                                                  {1, 0, 1, 0},
                                                  {1, 0, 1},
                                                  {1, 0, 1, 0, 1, 12, 13, 0, 0, 1, 0, 1},
                                                  {1, 0, 1, 0, 1, 12, 13, 0, 0, 1}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }


    }
/*
    TEST_F(PrefixSumArraysTest, printall) {
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

            std::cout << i << "th graph:" << std::endl;
            // std::cout << std::to_string(i)<<":"<<std::to_string(graph->min_node_id()) <<"->"<<std::to_string(graph->max_node_id())<<"all"<<std::endl;
            graph->for_each_path_handle([&](const gbwtgraph::path_handle_t path_handle) {

                graph->for_each_step_in_path(path_handle, [&](const gbwtgraph::step_handle_t step_handle) {
                    auto handle = graph->get_handle_of_step(step_handle);
                    std::cout << std::to_string(graph->handle_to_node(handle)) << "["<< graph->get_length(handle)<<"], ";

                }); // end of lambda expression
                std::cout << std::endl;
            });// end of lambda expression
            std::cout << std::endl << "----------------------" << std::endl;
        }
    }*/

/*
    TEST_F(PrefixSumArraysTest, get_all_node_positions) {
        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto positions = temp->get_all_node_positions(2);


        }
    }

*/
}
int main(int argc, char **argv)  {
    std::cout << "Hello, Santa Cruzsdd!" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
