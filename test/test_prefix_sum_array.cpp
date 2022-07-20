/**
 * Authors:
 *  - Davide Piovani
 *  - Andrea Mariotti
 *
 *  TODO:
 *      - CHECK MEMORY LEAK WITH VALGRIND
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
     * This structure is used to shrink the code and store parameters to perform different tests in the
     * ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES method.
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
     * This structure is used to shrink the code and store parameters to perform different tests in the
     * ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS method.
     */
    struct parameters_test_graph_vectors{
        std::vector<size_t> positions_node_1;  // First node
        std::vector<size_t> positions_node_2;  // Second node

        int assert_length_distance; // Length of the array that contains all the distances
        int assert_distance; // Distance value to be tested
        size_t path_id;
        int index_distance_test; // Index of the position in distance vector
    };





    class PrefixSumArraysTest : public ::testing::Test {

    protected:
        // Class members declared here can be used by all tests in the test suite
        // Cause the graph stores pointer to FastLocate that can be destroyed, we have to memorize it
        std::vector<std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>> *gfa_parses;


        // Vector of prefix sums arrays for each graph, prefix_sums_arrays[i] is referred to the graph i in
        // gfa_files_path vector.
        std::vector<PathsPrefixSumArrays*> *prefix_sums_arrays;


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
                                                        "../test/cyclic_graph_odd_paths.gfa",
                                                        "../test/cyclic_graph_different_orientation_in_path.gfa"
            };

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

        /**
         * AUX FUNCTIONS FOR TESTS
         */


        void ASSERT_NODE_POSITIONS_IN_PATH_EVEN(PathsPrefixSumArrays* paths_prefix_sum_arrays,
                                           std::vector<std::vector<std::vector<size_t>>> &params,
                                           gbwt::node_type node,
                                           int i){
            auto psa=(*paths_prefix_sum_arrays).get_prefix_sum_arrays();

            auto iterator = (*psa).begin();
            int test_path_index=0;
            while (iterator != (*psa).end())
            {
                size_t  path_id = iterator->first;

                // Accessing VALUE from element pointed by it.
                size_t ones;



                if(psa->find(path_id) != psa->end()  && (path_id % 2) == 0 ){
                    ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa).at(path_id)))(((*psa).at(path_id))->size());
                    std::vector<size_t>* node_positions = paths_prefix_sum_arrays->get_positions_of_a_node_in_path(path_id, node, ones);
                    ASSERT_EQ(params[i][test_path_index],*node_positions);
                }

                // Increment the Iterator to point to next entry
                iterator++;
                test_path_index++;
            }
        }


        void ASSERT_NODE_POSITIONS_IN_PATH_EVEN_ODD(PathsPrefixSumArrays* paths_prefix_sum_arrays,
                                           std::vector<std::vector<std::vector<size_t>>> &params,
                                           gbwt::node_type node,
                                           int i){
            auto psa=(*paths_prefix_sum_arrays).get_prefix_sum_arrays();

            auto iterator = (*psa).begin();
            int test_path_index=0;
            while (iterator != (*psa).end())
            {
                size_t  path_id = iterator->first+1;

                // Accessing VALUE from element pointed by it.
                size_t ones;


                 if((psa->find(path_id-1)!= psa->end() && (path_id % 2) != 0 )){
                    ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa).at(path_id-1)))(((*psa).at(path_id-1))->size());
                    std::vector<size_t>* node_positions = paths_prefix_sum_arrays->get_positions_of_a_node_in_path(path_id, node, ones);

                    ASSERT_EQ(params[i][test_path_index],*node_positions);
                }

                // Increment the Iterator to point to next entry
                iterator++;
                test_path_index++;
            }
        }

        void ASSERT_NODE_POSITIONS_WITH_WRONG_PATH(PathsPrefixSumArrays* paths_prefix_sum_arrays,
                                                   gbwt::node_type node,
                                                   size_t path_id,
                                                   int gfa_file_index){
            size_t fake_ones = 42;
            auto positions = (*prefix_sums_arrays)[gfa_file_index]->get_positions_of_a_node_in_path(path_id,
                                                                                                    node,
                                                                                                    fake_ones);
            ASSERT_EQ(positions->empty(), true);
        }


        /**
         * Used to remove duplicated code from the test GetAllNodeDistanceInAPathNodeInInputVersion, given the params compute
         * all the distance between two nodes and assert the number of compute distances and the distances.
         * @param psa the one created in the constructor of the fixture class PrefixSumArraysTest.
         * @param params is a vector of structures parameters_test_graph.
         * @param gfa_file_index index of the gfa_file in the vector gfa_files_paths defined in the constructor of PrefixSumArraysTest.
         */
        void ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(std::vector<PathsPrefixSumArrays*> const psa,
                                                       std::vector<parameters_test_graph> params,
                                                       int gfa_file_index){
            int number_of_tests_on_graph = params.size();
            for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){

                std::unique_ptr<std::vector<size_t>> distances;
                distances.reset((*psa[gfa_file_index]).
                        get_all_nodes_distances_in_path(params[index_test].n1,
                                                        params[index_test].n2,
                                                        params[index_test].path_id));
                ASSERT_EQ(params[index_test].assert_length_distance, distances->size());
                if (!distances->empty())
                    ASSERT_EQ(params[index_test].assert_distance,
                              distances->at(params[index_test].index_distance_test));
            }
        }


        /**
         * Used to remove duplicated code from the test GetAllNodeDistanceInAPathVectorInInputVersion, given the params compute
         * all the distance between two vector of positions of two nodes and assert the number of compute distances and the distances.
         * @param psa the one created in the constructor of the fixture class PrefixSumArraysTest.
         * @param params is a vector of structures parameters_test_graph_vectors.
         * @param gfa_file_index index of the gfa_file in the vector gfa_files_paths defined in the constructor of PrefixSumArraysTest.
         */
        void ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(std::vector<PathsPrefixSumArrays*> const psa,
                                                                     std::vector<parameters_test_graph_vectors> params,
                                                                     int gfa_file_index){
            int number_of_tests_on_graph = params.size();
            for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){

                std::unique_ptr<std::vector<size_t>> distances;
                distances.reset((*psa[gfa_file_index]).
                        get_all_nodes_distances_in_path(&params[index_test].positions_node_1,
                                                        &params[index_test].positions_node_2,
                                                        params[index_test].path_id));
                ASSERT_EQ(params[index_test].assert_length_distance, distances->size());
                if (!distances->empty())
                    ASSERT_EQ(params[index_test].assert_distance,
                              distances->at(params[index_test].index_distance_test));
            }
        }

        /**
         * Used to remove duplicated code from the test GetAllNodeDistanceInAPathVectorInInputVersion, given the params compute
         * all the distance between two vector of positions of two nodes and assert the number of compute distances and the distances.
         * Test the cases where a OutOfBoundsPositionInPathException is thrown.
         * @param psa the one created in the constructor of the fixture class PrefixSumArraysTest.
         * @param params is a vector of structures parameters_test_graph_vectors.
         * @param gfa_file_index index of the gfa_file in the vector gfa_files_paths defined in the constructor of PrefixSumArraysTest.
         */
        void ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(std::vector<PathsPrefixSumArrays*> const psa,
                                                                                                        std::vector<parameters_test_graph_vectors> params,
                                                                                                        int gfa_file_index){
            int number_of_tests_on_graph = params.size();
            for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){

                EXPECT_THROW((*psa[gfa_file_index]).
                        get_all_nodes_distances_in_path(&params[index_test].positions_node_1,
                                                        &params[index_test].positions_node_2,
                                                        params[index_test].path_id),OutOfBoundsPositionInPathException);
            }
        }

        void ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES_OF_POSITIONS_OutOfBoundsPositionInPathException(std::vector<PathsPrefixSumArrays*> const psa,
                                                                                                        std::vector<parameters_test_graph> params,
                                                                                                        int gfa_file_index){
            int number_of_tests_on_graph = params.size();
            for(int index_test = 0; index_test < number_of_tests_on_graph; ++index_test){

                EXPECT_THROW((*psa[gfa_file_index]).
                        get_all_nodes_distances_in_path(params[index_test].n1,
                                                        params[index_test].n2,
                                                        params[index_test].path_id),OutOfBoundsPositionInPathException);
            }
        }
    };


    /**
     * START OF TESTS
     */


    /**
     * Test the constructors
     */
    TEST_F(PrefixSumArraysTest, creation_prefixsumarray_test) {
        std::unique_ptr<PathsPrefixSumArrays> psa_default = std::unique_ptr<PathsPrefixSumArrays>(
                new PathsPrefixSumArrays());
        ASSERT_EQ(nullptr, psa_default->get_fast_locate());
        ASSERT_EQ(nullptr, psa_default->get_prefix_sum_arrays());

        // The prefix sums array is already been created in the constructor of the test.
        for (auto psa: *prefix_sums_arrays) {
            ASSERT_PSA_MEMBERS_NE_NULLPTR(*psa);
        }

        int gfa_file_index = 0;

        //(*prefix_sums_arrays)[gfa_file_index]->get_prefix_sum_arrays()
    }


    /**
     * Test the method PathsPrefixSumArrays::get_all_nodes_distances_in_path(std::vector<size_t>* node_1_positions,
     *                                                                     std::vector<size_t>* node_2_positions,
     *                                                                     size_t path_id)
     */
    TEST_F(PrefixSumArraysTest, get_all_node_distances_in_a_path_vector){
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
        int gfa_file_index = 0;

        std::vector<parameters_test_graph_vectors> parameters_first_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},

                {{420000000}, {0}, 0, 0, 1, 0},
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{0}, {420000000}, 0, 0, 1, 0},
        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_first_graph_OutOfBounds, gfa_file_index);


        std::vector<parameters_test_graph_vectors> parameters_first_graph ={ {{0}, {0}, 0, 0, 0, 0},
                                                                             {{}, {0}, 0, 0, 0, 0},
                                                                             {{0}, {}, 0, 0, 0, 0},
                                                                             {{2342346}, {}, 0, 0, 0, 0},
                                                                             {{}, {2342346}, 0, 0, 0, 0},

                                                                             {{0}, {0}, 0, 0, 1, 0},
                                                                             {{}, {0}, 0, 0, 1, 0},
                                                                             {{0}, {}, 0, 0, 1, 0},
                                                                             {{}, {2342346}, 0, 0, 1, 0}
                                                                             };
       /* std::unique_ptr<std::vector<size_t>> distances = std::unique_ptr<std::vector<size_t>>(
                (*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(&parameters_first_graph.positions_node_1,
                                                                                         &parameters_first_graph.positions_node_2,
                                                                                         parameters_first_graph.path_id));*/
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameters_first_graph, gfa_file_index);



        // only one distance computable because there is only one path of length 1
       // ASSERT_EQ(parameters_first_graph.assert_length_distance, distances->size());

        /**
         * One node cyclic graph, one path (0), all distances between 2 and 2
         */
        gfa_file_index = 1;

        std::vector<parameters_test_graph_vectors>  parameters_second_graph = {{{0, 1}, {0, 1}, 1, 0, 0, 0},
                                                                                {{0}, {0}, 0, 0, 0, 0},
                                                                                {{}, {0}, 0, 0, 0, 0},
                                                                                {{0}, {}, 0, 0, 0, 0},
                                                                                {{2342346}, {}, 0, 0, 0, 0},
                                                                                {{}, {2342346}, 0, 0, 0, 0},

                                                                                {{1}, {1}, 0, 0, 1, 0},
                                                                               {{0, 1}, {0, 1}, 1, 0, 1,0 },
                                                                               {{}, {1}, 0, 0, 1, 0},
                                                                                {{1}, {}, 0, 0, 1, 0},
                                                                                {{}, {2342346}, 0, 0, 1, 0}};



        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameters_second_graph, gfa_file_index);

        std::vector<parameters_test_graph_vectors> parameters_second_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_second_graph_OutOfBounds, gfa_file_index);




        /*// 1 distances because d(1,1)=0 not computed, d(1,2)=0, d(2,2)=0 not computed
        ASSERT_EQ(parameters_second_graph.assert_length_distance, distances->size());
        ASSERT_EQ(parameters_second_graph.assert_distance, distances->at(parameters_second_graph.index_distance_test));*/

        /**
         * Acyclic graph, even paths (4), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         */

        gfa_file_index = 2;
        std::vector<parameters_test_graph_vectors> parameter_third_graph_vector = {{{0}, {3}, 1, 2, 0, 0},
                                                                                   {{0}, {4}, 1, 4, 0, 0},
                                                                                   {{4}, {0}, 1, 4, 0, 0},
                                                                                   {{0}, {3}, 1, 3, 6, 0},

                                                                                   {{0}, {3}, 1, 3, 7, 0},
                                                                                   {{2}, {3}, 1, 0, 7, 0},
                                                                                   {{2}, {1}, 1, 0, 7, 0},
                                                                                   {{0}, {2}, 1, 2, 7, 0},

                                                                           }; // 2, 12

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameter_third_graph_vector, gfa_file_index);



        std::vector<parameters_test_graph_vectors> parameters_third_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
                {{3,4}, {420000000}, 0, 0, 4, 0},
        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_third_graph_OutOfBounds, gfa_file_index);


        /**
         * Acyclic graph, odd paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         */
        gfa_file_index = 3;
        std::vector<parameters_test_graph_vectors> parameter_fourth_graph_vector = {{{0}, {3}, 1, 2, 0, 0},
                                                                                    {{0}, {}, 0, 0, 2, 0},
                                                                                    {{0}, {}, 0, 0, 4, 0},
                                                                                    {{0},{},0,0,0,0},
                                                                                    {{},{0},0,0,0,0},
                                                                                    {{}, {1}, 0, 0, 0, 0},
                                                                                    {{0}, {2}, 1, 1, 2, 0},
                                                                                    {{2}, {0}, 1, 1, 2, 0},
                                                                                    {{},{},0,0,0,0},
                                                                                    {{},{},0,0,7,0},
                                                                                    {{},{},0,0,6,0},
                                                                                    {{0},{},0,0,2,0},
                                                                                    {{2},{0},1,1,0,0},
                                                                                    {{2},{1},1,0,3,0},
                                                                                    {{2},{0},1,1,3,0},
                                                                                    {{2},{2},0,1,3,0},
                                                                                    {{0},{0},0,1,3,0},
                                                                                    {{},{},0,0,7,0},
                                                                                    {{},{},0,0,3,0},
                                                                                    {{},{1},0,0,3,0},
                                                                                    {{1},{},0,0,3,0},




        };      //6,2


        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameter_fourth_graph_vector, gfa_file_index);

        std::vector<parameters_test_graph_vectors> parameters_fourth_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
                {{3,4}, {420000000}, 0, 0, 4, 0},

        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_fourth_graph_OutOfBounds, gfa_file_index);

        /**
         * Cyclic graph, even paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         * Path: 8 - Nodes:  2 4 6 2 6 6 8
         * Path: 10 - Nodes:  2 6 6 10 6
         */

        gfa_file_index = 4;
        std::vector<parameters_test_graph_vectors> parameter_fifth_graph_vector = {{{0}, {3}, 1, 2, 10, 0},
                                                                                   {{0,3}, {2,4,5}, 6, 1, 8, 0},
                                                                                   {{0,3}, {2,4,5}, 6, 0, 8, 3},
                                                                                   {{2,4,5}, {0,3}, 6, 0, 8, 4},
                                                                                   {{2,4,5}, {0,3}, 6, 1, 8, 5},
                                                                                   {{0,3}, {6}, 2, 14, 8, 0},
                                                                                   {{0,3}, {6}, 2, 2, 8, 1},
                                                                                   {{0,3}, {}, 0, 0, 8, 0},
                                                                                   {{0}, {4,1,2}, 3, 0, 9, 0},
                                                                                   {{0}, {4,1,2}, 3, 1, 9, 1},
                                                                                   {{0}, {4,1,2}, 3, 12, 9, 2},



        };

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameter_fifth_graph_vector, gfa_file_index);

        std::vector<parameters_test_graph_vectors> parameters_fifth_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
                {{3,4}, {420000000}, 0, 0, 4, 0},
                {{1,2,4}, {420000000}, 0, 0, 9, 0},
                {{420000000}, {1,2,4}, 0, 0, 9, 0},


        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_fifth_graph_OutOfBounds, gfa_file_index);



        /**
         * Cyclic graph, even paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         * Path: 8 - Nodes:  2 4 6 2 6 6 8
         */

        gfa_file_index = 5;
        std::vector<parameters_test_graph_vectors> parameter_sixth_graph_vector = {{{0,3}, {2,4,5}, 6, 1, 8, 0},
                                                                                   {{0,3}, {2,4,5}, 6, 12, 8, 1},
                                                                                   {{0,3}, {2,4,5}, 6, 13, 8, 2},
                                                                                   {{0,3}, {2,4,5}, 6, 0, 8, 3},
                                                                                   {{2,4,5}, {0,3}, 6, 0, 8, 4},
                                                                                   {{2,4,5}, {0,3}, 6, 1, 8, 5},
                                                                                   {{0,3}, {6}, 2, 14, 8, 0},
                                                                                   {{0,3}, {6}, 2, 2, 8, 1},
                                                                                   {{0,3}, {}, 0, 0, 8, 0},

                                                                                   {{0}, {2}, 1, 1, 5, 0},
                                                                                   {{2}, {0}, 1, 1, 5, 0},
                                                                                   {{2}, {1}, 1, 0, 5, 0},
                                                                                   {{2}, {2}, 0, 0, 5, 0},
                                                                                   {{0}, {3}, 1, 2, 5, 0},

        };

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameter_sixth_graph_vector, gfa_file_index);


        std::vector<parameters_test_graph_vectors> parameters_sixth_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
                {{3,4}, {420000000}, 0, 0, 4, 0},
                {{1,2,4}, {420000000}, 0, 0, 9, 0},
                {{420000000}, {1,2,4}, 0, 0, 9, 0},


        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_sixth_graph_OutOfBounds, gfa_file_index);

        /**
        *6th graph:
            2[10], 5[1], 6[1], 11[2], 12[1],
            3[10], 7[1], 9[5],
            2[10], 4[1], 6[1], 8[5],
            3[10], 6[1], 10[2], 13[1],
            2[10], 4[1], 7[1], 2[10], 6[1], 6[1], 8[5],
         */

        gfa_file_index = 6;
        std::vector<parameters_test_graph_vectors> parameter_seventh_graph_vector = {{{0}, {1}, 1, 0, 2, 0},
                                                                                     {{0}, {0}, 0, 0, 2, 0},
                                                                                     {{1}, {1}, 0, 0, 2, 0},
                                                                                     {{0}, {2}, 1, 1, 2, 0},
                                                                                     {{0}, {2}, 1, 1, 0, 0},
                                                                                     {{0}, {3}, 1, 2, 0, 0},
                                                                                     {{3}, {0}, 1, 2, 0, 0},
                                                                                     {{2}, {0,3}, 2, 1, 8, 0},
                                                                                     {{2}, {0,3}, 2, 0, 8, 1},
                                                                                     {{0,3}, {0,3}, 1, 2, 8, 0},
                                                                                     {{0,3}, {6}, 2, 14, 8, 0},
                                                                                     {{0,3}, {6}, 2, 2, 8, 1},         //    {{0}, {3}, 1, 2, 8, 0},            //    {{0}, {6}, 1, 14, 8, 0},
                                                                                     //reverse
                                                                                     {{0}, {5}, 1, 13, 9, 0},
                                                                                     {{5}, {0}, 1, 13, 9, 0},
                                                                                     {{4}, {0}, 1, 12, 9, 0},
                                                                                     {{0}, {4}, 1, 12, 9, 0},
                                                                                     {{0}, {3}, 1, 3, 1, 0},
                                                                                     {{0}, {2}, 1, 1, 3, 0},
                                                                                     {{2}, {2}, 0, 0, 3, 0},


        };

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS(*prefix_sums_arrays, parameter_seventh_graph_vector, gfa_file_index);


        std::vector<parameters_test_graph_vectors> parameters_seventh_graph_OutOfBounds ={
                {{0}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {420000000}, 0, 0, 0, 0},
                {{420000000}, {0}, 0, 0, 0, 0},
                {{420000000}, {1}, 0, 0, 0, 1},
                {{0, 2346}, {1}, 1, 0, 1,0 },
                {{0}, {1,2346}, 1, 0, 0,0 },
                {{0}, {1,2346}, 1, 0, 1,0 },
                {{0}, {2346,1}, 1, 0, 1,0 },
                {{420000000}, {420000000}, 0, 0, 1, 0},
                {{1}, {420000000}, 0, 0, 1, 0},
                {{3,4}, {420000000}, 0, 0, 4, 0},
                {{1,2,4}, {420000000}, 0, 0, 9, 0},
                {{420000000}, {1,2,4}, 0, 0, 9, 0},


        };
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_VECTOR_OF_POSITIONS_OutOfBoundsPositionInPathException(*prefix_sums_arrays,parameters_seventh_graph_OutOfBounds, gfa_file_index);

    }


    /**
     * Test the method PathsPrefixSumArrays::get_all_nodes_distances_in_path( gbwt::node_type node_1,
     *                                                                        gbwt::node_type node_2,
     *                                                                        size_t path_id )
     */
    TEST_F(PrefixSumArraysTest, get_all_nodes_distances_in_path_node_IDs) {
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
        int gfa_file_index = 0;
        std::vector<parameters_test_graph> parameters_first_graph = {
                                                                    {2, 2, 0, 0, 0, 0},
                                                                    {3, 3, 0, 0, 1, 0},
                                                                    {2, 9, 0, 0, 0, 0},
                                                                    {7, 3, 0, 0, 1, 0}
                                                                    };

        // only one distance computable because there is only one path of length 1
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameters_first_graph, gfa_file_index);
        size_t param0 = 0; // needed since c++ confuses 0 with a nullptr and the function result ambiguous
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(param0, param0, 2), PathNotInGraphException);


        /**
         * One node cyclic graph, one path (0), all distances between 2 and 2
         */
        gfa_file_index = 1;
        std::vector<parameters_test_graph> parameters_second_graph = {{2, 2, 1, 0, 0, 0},
                                                                      {3, 3, 1, 0, 1, 0},
                                                                      {2, 9, 0, 0, 0, 0},
                                                                      {7, 3, 0, 0, 1, 0}
                                                                      };

        // 1 distances because d(1,1)=0 not computed, d(1,2)=0, d(2,2)=0 not computed
        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameters_second_graph, gfa_file_index);


        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(0, 2, 2), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(0, 2, 3), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 0, 3), PathNotInGraphException);

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
                                                                            {2, 12, 1, 3, 6, 0},
                                                                            {7, 13, 1, 2, 7, 0},
                                                                            {3, 13, 1, 3, 7, 0},
                                                                            {1, 1, 0, 0, 7, 0}};



        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameter_third_graph_vector, gfa_file_index);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 8), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 9), PathNotInGraphException);



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
                                                                           {16,2,0,0,0,0},
                                                                           {8, 4, 0, 0, 0, 0},
                                                                           {2, 8, 1, 1, 2, 0},
                                                                           {8, 2, 1, 1, 2, 0},
                                                                           {16,16,0,0,0,0},
                                                                           {2,5,0,0,2,0},
                                                                           {6,2,1,1,0,0},
                                                                           {3, 9, 1, 1, 3, 0},
                                                                           {9, 3, 1, 1, 3, 0},
                                                                           {9, 3, 1, 2, 5, 0},
                                                                           {5, 7, 1, 0, 5, 0}};


        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameter_fourth_graph_vector, gfa_file_index);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 8), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 9), PathNotInGraphException);

        /**
         * Cyclic graph, even paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         * Path: 8 - Nodes:  2 4 6 2 6 6 8
         * Path: 10 - Nodes:  2 6 6 10 6
         */

        gfa_file_index = 4;
        std::vector<parameters_test_graph> parameter_fifth_graph_vector = {{2, 10, 1, 2, 10, 0},
                                                                           {2, 6, 6, 1, 8, 0},
                                                                           {2, 6, 6, 12, 8, 1},
                                                                           {2, 6, 6, 13, 8, 2},
                                                                           {2, 6, 6, 0, 8, 3},
                                                                           {6, 2, 6, 0, 8, 4},
                                                                           {6, 2, 6, 1, 8, 5},
                                                                           {2, 8, 2, 14, 8, 0},
                                                                           {2, 8, 2, 2, 8, 1},
                                                                           {2, 19, 0, 0, 8, 0},
                                                                           {9, 3, 2, 2, 9, 0},
                                                                           {9, 3, 2, 14, 9, 1},
                                                                           {9, 9, 0, 0, 9, 0}};

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameter_fifth_graph_vector, gfa_file_index);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 12), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 13), PathNotInGraphException);


        /**
         * Cyclic graph, even paths (3), all distances
         * Path: 0 - Nodes:  2 4 6 10 12
         * Path: 2 - Nodes:  2 6 8
         * Path: 4 - Nodes:  2 4 6 8
         * Path: 6 - Nodes:  2 6 10 12
         * Path: 8 - Nodes:  2 4 6 2 6 6 8
         */


        gfa_file_index = 5;
        std::vector<parameters_test_graph> parameter_sixth_graph_vector = {{2, 6, 6, 1, 8, 0},
                                                                           {2, 6, 6, 12, 8, 1},
                                                                           {2, 6, 6, 13, 8, 2},
                                                                           {2, 6, 6, 0, 8, 3},
                                                                           {6, 2, 6, 0, 8, 4},
                                                                           {6, 2, 6, 1, 8, 5},
                                                                           {2, 8, 2, 14, 8, 0},
                                                                           {2, 8, 2, 2, 8, 1},
                                                                           {2, 19, 0, 0, 8, 0},
                                                                           {9, 9, 0, 0, 9, 0},
                                                                           {3, 5, 2, 0, 9, 1},
                                                                           {3, 5, 2, 1, 9, 0}};

        ASSERT_PSA_ALL_DISTANCE_BETWEEN_TWO_NODES(*prefix_sums_arrays, parameter_sixth_graph_vector, gfa_file_index);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 12), PathNotInGraphException);
        EXPECT_THROW((*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 13), PathNotInGraphException);

       try{
            (*(*prefix_sums_arrays)[gfa_file_index]).get_all_nodes_distances_in_path(2, 4, 13);
        }catch(PathNotInGraphException ex){
            std::string msg_exception = std::string(ex.what());
            ASSERT_EQ(msg_exception, std::string("Error in 'get_all_nodes_distances_in_path': the inserted path 13 doesn't exist inside the graph."));
        }
    }


    TEST_F(PrefixSumArraysTest, get_distance_between_positions_in_path) {
        /**
         * reverse paths (existing, non existing with existing positions and not existing ones)
         */
        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            //testing the outer function
            EXPECT_THROW(temp->get_distance_between_positions_in_path(0, 0, 0), NotExistentDistanceException);
            if (i > 1) {
                ASSERT_EQ(2, temp->get_distance_between_positions_in_path(0, 3, 0));
                //testing the aux function
                auto tempsa = (*temp).psa;
                sdsl::sd_vector<>::select_1_type sdb_sel(tempsa->at(0));
                ASSERT_EQ(1, temp->get_distance_between_positions_in_path_aux(1, 3, sdb_sel));
                ASSERT_EQ(3, temp->get_distance_between_positions_in_path_aux(1, 4, sdb_sel));

                if(i>3) {
                    ASSERT_EQ(12, temp->get_distance_between_positions_in_path(0, 4, 8));
                    ASSERT_EQ(14, temp->get_distance_between_positions_in_path(0, 6, 8));
                }
            }

            // Non existing path, with existing and non existing nodes in the path
            try {
                temp->get_distance_between_positions_in_path(0, 2, 16);
            }catch(PathNotInGraphException &ex){
                std::string s(ex.what());
                ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the path_id 16 doesn't exist in the graph.\n"));
            }

            try {
                temp->get_distance_between_positions_in_path(0, 16, 16);
            }catch(PathNotInGraphException &ex){
                std::string s(ex.what());
                ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the path_id 16 doesn't exist in the graph.\n"));
            }

            try {
                temp->get_distance_between_positions_in_path(16, 16, 16);
            }catch(NotExistentDistanceException &ex){
                std::string s(ex.what());
                ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the distance between two equal positions (16, 16) doesn't exist.\n"));
            }

            try {
                temp->get_distance_between_positions_in_path(16, 0, 16);
            }catch(PathNotInGraphException &ex){
                std::string s(ex.what());
                ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the path_id 16 doesn't exist in the graph.\n"));
            }
        }

        struct parameters_test_graph_get_distance_between_positions_in_path {
            size_t gfa_file_index;
            size_t assert_distance; // Value of the expected distance
            size_t pos_node_1;
            size_t pos_node_2;
            size_t path_id;
        };

        std::vector<parameters_test_graph_get_distance_between_positions_in_path> params_exception = {
                // Existing path, with existing and non existing nodes in the path
                {2,0,0,5,0},
                };
        for(int test_index = 0; test_index < params_exception.size(); ++test_index){
            try {
                (*prefix_sums_arrays)[params_exception[test_index].gfa_file_index]
                ->get_distance_between_positions_in_path(params_exception[test_index].pos_node_1,
                                                         params_exception[test_index].pos_node_2,
                                                         params_exception[test_index].path_id);
            }catch(OutOfBoundsPositionInPathException &ex){
                std::string s(ex.what());
                ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position " +
                std::to_string(params_exception[test_index].pos_node_2) + " is outside the boundaries of the path [0:4]\n"));
            }
        }


        // Existing path, with existing and non existing nodes in the path


        try {
            (*prefix_sums_arrays)[3]->get_distance_between_positions_in_path(0, 4, 2);
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 4 is outside the boundaries of the path [0:2]\n"));
        }

        try {
            (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(2, 5, 0);
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 5 is outside the boundaries of the path [0:4]\n"));
        }

        (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(2, 4, 0);
        (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(0, 4, 0);
        (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(0, 1, 0);

        try {
            (*prefix_sums_arrays)[2]->get_distance_between_positions_in_path(2, 16, 0);
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 16 is outside the boundaries of the path [0:4]\n"));
        }


        // Reverse paths test cyclic
        ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(0, 4, 9));
        ASSERT_EQ(4, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(0, 4, 1));
        ASSERT_EQ(1, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(1, 3, 1));

        // Reverse paths test acyclic
        ASSERT_EQ(2, (*prefix_sums_arrays)[3]->get_distance_between_positions_in_path(1, 4, 1));

        // Reverse paths test exception
        try{
            ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(0, 7, 9));
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 7 is outside the boundaries of the path [0:6]\n"));
        }

        try{
            ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(0, 37, 9));
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 37 is outside the boundaries of the path [0:6]\n"));
        }

        try{
            ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(3, 37, 9));
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 37 is outside the boundaries of the path [0:6]\n"));
        }

        try{
            ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(3, 7, 9));
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 7 is outside the boundaries of the path [0:6]\n"));
        }

        try{
            ASSERT_EQ(12, (*prefix_sums_arrays)[5]->get_distance_between_positions_in_path(3, 7, 9));
        }catch(OutOfBoundsPositionInPathException &ex){
            std::string s(ex.what());
            ASSERT_EQ(s, std::string("Error in 'get_distance_between_positions_in_path': the second position 7 is outside the boundaries of the path [0:6]\n"));
        }
    }

    /*6th graph:
    2[10], 5[1], 6[1], 11[2], 12[1],
    3[10], 7[1], 9[5],
    2[10], 4[1], 6[1], 8[5],
    3[10], 6[1], 10[2], 13[1],
    2[10], 4[1], 7[1], 2[10], 6[1], 6[1], 8[5],
    1 0 1 12 13 0 1
    */
    TEST_F(PrefixSumArraysTest, get_all_nodes_distances) {
        size_t A = 2;
        size_t B = 6;

        std::vector<std::vector<size_t>> check = {{},
                                                  {},
                                                  {1, 0, 1, 0},
                                                  {1, 0, 1},
                                                  {1, 0, 1, 0, 1, 12, 13, 0, 0, 1, 0, 1},
                                                  {1, 0, 1, 0, 1, 12, 13, 0, 0, 1},
                                                  {1, 0, 1, 12, 13, 0, 1 }};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }

        A = 2;
        B = 12;
        check = {{},
                 {},
                 {4,3},
                 {4},
                 {4,3,4},
                 {4,3},
                 {4, 3}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }




        A = 12;
        B = 6;
        check = {{},
                 {},
                 {2,2},
                 {2},
                 {2,2,3,2},
                 {2,2},
                 {2}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }

        A = 2;
        B = 2;
        check = {{},
                 {0},
                 {},
                 {},
                 {2},
                 {2},
                 {2}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }

        // Reversed paths
        A = 3;
        B = 7;
        check = {{},
                 {},
                 {1, 0, 1, 0},
                 {1, 0, 1},
                 {1, 0, 1, 0, 1, 13, 0, 12, 0, 1, 1, 0},
                 { 1, 0, 1, 0, 1, 13, 0, 12, 0, 1 },
                 { 1, 0, 1, 1, 13, 0, 12 }
                 };


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));

        }

        A = 3;
        B = 3;
        check = {{},
                 {0},
                 {},
                 {},
                 {2},
                 {2},
                 {2}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }

        A = 13;
        B = 7;
        check = {{},
                 {},
                 {2,2},
                 {2},
                 {2,2,2,3},
                 {2,2},
                 {2}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }

        A = 3;
        B = 13;
        check = {{},
                 {},
                 {4,3},
                 {4},
                 {4,3,4},
                 {4,3},
                 {4, 3}};


        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto distance_vector = temp->get_all_nodes_distances(A, B);
            ASSERT_EQ((*distance_vector), check.at(i));
        }
    }


    TEST_F(PrefixSumArraysTest, get_all_node_positions) {
/*
        std::vector<std::vector<std::vector<size_t>>> check = {{{0}},
                                                               {{0,1}},
                                                               {{0},{0},{0},{0}},
                                                               {{0},{0},{0}},
                                                               {{0},{0},{0},{0},{0,3},{0}},
                                                               {{0},{0},{0},{0},{0,3}}};

        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto positions = temp->get_all_node_positions(2);
            auto iterator = (*positions).begin();

            // Iterate over the map using Iterator till end.
            int j=0;
            while (iterator != (*positions).end())
            {

                // Accessing VALUE from element pointed by it.
                for(int k=0; k< iterator->second->size(); ++k){
                    ASSERT_TRUE((*iterator->second)[k]==check[i][j][k]);
                }
                // Increment the Iterator to point to next entry
                iterator++;
                j++;
            }
        }
        check = {{{}},
                 {{}},
                 {{4},{3}},
                 {{4}},
                 {{4},{3},{4}},
                 {{4},{3}}};

        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto positions = temp->get_all_node_positions(12);
            auto iterator = (*positions).begin();

            // Iterate over the map using Iterator till end.
            int j=0;
            while (iterator != (*positions).end())
            {
                // Accessing VALUE from element pointed by it.
                for(int k=0; k< iterator->second->size(); ++k){
                    ASSERT_TRUE((*iterator->second)[k]==check[i][j][k]);
                }
                // Increment the Iterator to point to next entry
                iterator++;
                j++;
            }
        }

/* //DEBUG PRINT
        for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
            std::cout << "\n\n$psa"<<std::to_string(i);
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
            auto positions = temp->get_all_node_positions(12);
            auto iterator = (*positions).begin();

            // Iterate over the map using Iterator till end.
            int j=0;
            while (iterator != (*positions).end())
            {
                std::cout << "\n>" ;
                // Accessing VALUE from element pointed by it.
                for(int k=0; k< iterator->second->size(); ++k){
                    std::cout << (*iterator->second)[k]<<", ";
                }
                // Increment the Iterator to point to next entry
                iterator++;
                j++;
            }
        }
        std::cout << std::endl;
//DEBUG PRINT
*/
    }

/*
    TEST_F(PrefixSumArraysTest,get_positions_of_a_node_in_path){


        std::vector<std::vector<std::vector<std::vector<size_t>>>> check = {
                //node:2
                {{{0}},
                 {{0,1}},
                 {{0},{0},{0},{0}},
                 {{0},{0},{0}},
                 {{0},{0},{0},{0},{0,3},{0}},
                 {{0},{0},{0},{0},{0,3}}},
                //node: 12
                {{{}},
                 {{}},
                 {{4},{},{},{3}},
                 {{4},{},{}},
                 {{4},{},{},{3},{},{4}},
                 {{4},{},{},{3},{}}},
                 //node:15
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                 // node: 0;
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                 // node 6;
                {{{}},
                 {{}},
                 {{2},{1},{2},{1}},
                 {{2},{1},{2}},
                 {{2},{1},{2},{1},{4,2,5},{1,2}},
                 {{2},{1},{2},{1},{4,2,5}}},
                //node 7
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                //node 13
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                //node 1
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}}
        };

        std::vector<std::vector<std::vector<std::vector<size_t>>>> check_disp = {

                //node 7
                {{{}},
                 {{}},
                 {{2},{1},{1},{2}},
                 {{2},{1},{1}},
                 {{2},{1},{1},{2},{4,2,1},{3,2}},
                 {{2},{1},{1},{2},{4,2,1}}},
                //node 4
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                 //node 2
                {{{}},
                 {{}},
                 {{},{},{},{}},
                 {{},{},{}},
                 {{},{},{},{},{},{}},
                 {{},{},{},{},{}}},
                //node 3
                {{{0}},
                 {{0,1}},
                 {{4},{2},{3},{3}},
                 {{4},{2},{3}},
                 {{4},{2},{3},{3},{6,3},{4}},
                 {{4},{2},{3},{3},{6,3}}},
                //node 13
                {{{}},
                 {{}},
                 {{0},{},{},{0}},
                 {{0},{},{}},
                 {{0},{},{},{0},{},{0}},
                 {{0},{},{},{0},{}}},
        };


        std::vector<size_t> node_ids ={2,12,15,0,6,7,13,1};
        for (int check_index = 0; check_index < check.size(); ++check_index) {

            for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
                PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
                auto psa=(*temp).get_prefix_sum_arrays();
                ASSERT_NODE_POSITIONS_IN_PATH_EVEN(temp,check[check_index],node_ids[check_index],i);
            }

        }

        std::vector<size_t> node_ids_disp ={7,4,2,3,13};
        for (int check_index = 0; check_index < check_disp.size(); ++check_index) {

            for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
                PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
                auto psa=(*temp).get_prefix_sum_arrays();
                ASSERT_NODE_POSITIONS_IN_PATH_EVEN_ODD(temp,check_disp[check_index],node_ids_disp[check_index],i);
            }

        }

        std::vector<size_t> node_ids_out ={0,17,2};
        for (int check_index = 0; check_index < check_disp.size(); ++check_index) {

            for (int i = 0; i < prefix_sums_arrays->size(); ++i) {
                PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[i];
                auto psa=(*temp).get_prefix_sum_arrays();
                ASSERT_NODE_POSITIONS_IN_PATH_EVEN_ODD(temp,check_disp[check_index],node_ids_disp[check_index],i);
            }

        }


        // Non existing nodes and non existing paths tests
        std::vector<std::pair<size_t,  gbwt::node_type>> test = {{2,17},
                                                                 {1, 17},
                                                                 {3, 17},
                                                                 {12, 17},
                                                                 {13, 17},
                                                                 {6, 17},
                                                                 {20, 20},
                                                                 {20, 0},
                                                                 {20, 12},
                                                                 {20, 10}
                                                                };

        for(int gfa_file_index=0; gfa_file_index <  prefix_sums_arrays->size(); ++gfa_file_index){
            PathsPrefixSumArrays *temp = (*prefix_sums_arrays)[gfa_file_index];

            for(int test_index = 0; test_index < test.size(); ++test_index){
                ASSERT_NODE_POSITIONS_WITH_WRONG_PATH(temp,  test[test_index].first,test[test_index].second, gfa_file_index);
            }
        }
    }*/
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


/*
    TEST_F(PrefixSumArraysTest, printall) {
        std::unique_ptr<gbwtgraph::GBWTGraph> graph;
        // Name of the graph examples for testing
        std::vector<std::string> gfa_files_paths = {"../test/one_node_acyclic.gfa",
                                                    "../test/one_node_cyclic.gfa",
                                                    "../test/acyclic_graph_even_paths.gfa",
                                                    "../test/acyclic_graph_odd_paths.gfa",
                                                    "../test/cyclic_graph_even_paths.gfa",
                                                    "../test/cyclic_graph_odd_paths.gfa",
                                                    "../test/cyclic_graph_different_orientation_in_path.gfa"
        };
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

} // End namespace
int main(int argc, char **argv)  {
    std::cout << "MAIN RUN ALL TESTS." << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
