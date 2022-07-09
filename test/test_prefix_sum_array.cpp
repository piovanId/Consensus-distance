/**
 * Authors:
 *  - Davide Piovani
 *  - Andrea Mariotti
 *
 *  TODO:
 *      - CONTROLLARE MEMORY LEAK, PRIMA A MANO E POI SE TI PERDI QUALCOSA CON VALGRIND
 *      - CHECK THE TRY CATCH FOR DELETING MEMORY AND USE THE TRY CATCH WHERE YOU USE A FUNCTION THAT HAS A THROW
 *      - CHECK HOW TO USE PROPERLY THE THROW (IN THE METHODS)
 *      - CHECK THE CONSTANT ERROR OF THE THROW THAT DAVIDE SENT YOU
 *      - GO ON WITH THE TESTS
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
#include <gbwt/gbwt.h>

// Sdsl
#include <sdsl/sd_vector.hpp>

// Custom
#include "../include/consensus_distance/pruned_graph.h"

// test
#include <gtest/gtest.h>



/**
 * This namespace contains all the unit tests
 */
namespace pathsprefixsumarrays {

    class PrefixSumArraysTest : public ::testing::Test {
    protected:
        // Class members declared here can be used by all tests in the test suite

        // Vector of prefix sums arrays for each graph, prefix_sums_arrays[i] is referred to the graph i in
        // gfa_files_path vector.
        std::vector<PathsPrefixSumArrays*> *prefix_sums_arrays;

        // Cause the graph stores pointer to FastLocate that can be destroyed, we have to memorize it
        std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>> *gfa_parses;

        PrefixSumArraysTest() {
            prefix_sums_arrays = new std::vector<PathsPrefixSumArrays*>();
            gfa_parses = new std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>>();

            std::unique_ptr<gbwtgraph::GBWTGraph> graph;

            // Name of the graph examples for testing
            // ATTENTION: don't change the order of this name files because the tests are based on that order.
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


    /**
     * Used to remove duplicated code, assert that all the psa memebers are not equal to NULLPTR
     * @param psa
     */
    void ASSERT_PSA_MEMBERS_NE_NULLPTR(PathsPrefixSumArrays const &psa) {
        // googletest has the assumption that you put the expected value first
        ASSERT_NE(nullptr, psa.get_fast_locate());
        ASSERT_NE(nullptr, psa.get_prefix_sum_arrays());
    }


    /**
     * Test the constructors
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


    /**
     * Test the method PathsPrefixSumArrays::get_all_nodes_distances_in_path( gbwt::node_type node_1,
     *                                                                        gbwt::node_type node_2,
     *                                                                        size_t path_id )
     */
    TEST_F(PrefixSumArraysTest, GetAllNodeDistanceInAPath) {
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

        struct parameters_test_graph{
            gbwt::node_type n1;  // First node
            gbwt::node_type n2;  // Second node

            int assert_length_distance; // Length of the array that contains all the distances
            int assert_distance; // Distance value to be tested
            size_t path_id;
            int index_distance_test; // Index of the position in distance vector
        };

        /**
         * One node acyclic graph, one path (0), all distances between 2 and 2
         */
        int gfa_file_index = 0;

        parameters_test_graph parameters_first_graph = {2, 2, 1, 0, 0, 0};

        std::unique_ptr<std::vector<size_t>> distances = std::unique_ptr<std::vector<size_t>>(
                (*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(parameters_first_graph.n1,
                                                                                         parameters_first_graph.n2,
                                                                                         parameters_first_graph.path_id));
        // only one distance computable because there is only one path of length 1
        ASSERT_EQ(parameters_first_graph.assert_length_distance, distances->size());
        // distances between two same positions in a path is 0
        ASSERT_EQ(parameters_first_graph.assert_distance, distances->at(parameters_first_graph.index_distance_test));

        /**
         * One node cyclic graph, one path (0), all distances between 2 and 2
         */
        gfa_file_index = 1;
        parameters_test_graph parameters_second_graph = {2, 2, 3, 0, 0, 0};

        distances.reset((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(parameters_second_graph.n1,
                                                                                                 parameters_second_graph.n2,
                                                                                                 parameters_second_graph.path_id));
        ASSERT_EQ(parameters_second_graph.assert_length_distance, distances->size()); // 3 distances because d(1,1)=0, d(1,2)=0, d(2,2)=0
        for(int distance=0; distance < distances->size(); ++distance){
            ASSERT_EQ(parameters_second_graph.assert_distance, distances->at(parameters_second_graph.index_distance_test));
        }

        /**
         * Acyclic graph, even paths (4), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         */

        gfa_file_index = 2;
        std::vector<parameters_test_graph> parameter_third_graph_vector = {{2, 10, 1, 2, 0, 0},
                                                                            {2, 12, 1, 4, 0, 0},
                                                                            {2, 12, 1, 3, 6, 0}};

        int number_of_tests_on_graph = parameter_third_graph_vector.size(); // redundant but help the readability
        for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){
            distances.reset((*(*prefix_sums_arrays)[gfa_file_index]).
            get_all_nodes_distances_in_path(parameter_third_graph_vector[index_test].n1,
                                            parameter_third_graph_vector[index_test].n2,
                                            parameter_third_graph_vector[index_test].path_id));
            ASSERT_EQ(parameter_third_graph_vector[index_test].assert_length_distance, distances->size());
            ASSERT_EQ(parameter_third_graph_vector[index_test].assert_distance,
                      distances->at(parameter_third_graph_vector[index_test].index_distance_test));
        }

        /**
         * Acyclic graph, odd paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         */

        gfa_file_index = 3;
        std::vector<parameters_test_graph> parameter_fourth_graph_vector = {{2, 10, 1, 2, 0, 0},
                                                                           {2, 10, 0, 0, 2, 0},
                                                                           {2, 10, 0, 0, 4, 0},
                                                                           {2,16,0,0,0,0},
                                                                           {8, 4, 0, 0, 0, 0}
                                                                            };


        number_of_tests_on_graph = parameter_fourth_graph_vector.size();


        for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){
            //auto temp = (*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 10, 2);



            //auto temp1 = (*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(parameter_fourth_graph_vector[index_test].n1, parameter_fourth_graph_vector[index_test].n2, parameter_fourth_graph_vector[index_test].path_id);

            try {
                distances.reset((*(*prefix_sums_arrays)[gfa_file_index]).
                        get_all_nodes_distances_in_path(parameter_fourth_graph_vector[index_test].n1,
                                                        parameter_fourth_graph_vector[index_test].n2,
                                                        parameter_fourth_graph_vector[index_test].path_id));
                ASSERT_EQ(parameter_fourth_graph_vector[index_test].assert_length_distance, distances->size());
                if (!distances->empty())
                    ASSERT_EQ(parameter_fourth_graph_vector[index_test].assert_distance,
                              distances->at(parameter_fourth_graph_vector[index_test].index_distance_test));
            }catch(NodeNotInPathsException &ex){
                EXPECT_EQ(ex.what(), "NodeNotInPathsException: The node used doesn't occur in any path.");
            }
        }
    }

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
            try {
                PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
                auto distance_vector = temp->get_all_nodes_distances(A, B);
                ASSERT_EQ((*distance_vector), check.at(i));
            }catch(NodeNotInPathsException &ex){
                ASSERT_TRUE(i == 0 || i == 1);
                EXPECT_EQ(ex.what(), "NodeNotInPathsException: The node used doesn't occur in any path.");
            }

        }


    }

/*    TEST_F(PrefixSumArraysTest, printall) {
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
} // End namespace
int main(int argc, char **argv)  {
    std::cout << "Hello, Santa Cruzsdd!" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
