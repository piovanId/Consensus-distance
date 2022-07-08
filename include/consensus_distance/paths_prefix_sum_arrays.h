/**
 * Authors:
 *  - Andrea Mariotti
 *  - Davide Piovani
 */

#ifndef CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H
#define CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H



// Standard
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

// GBWT
#include <gbwt/gbwt.h>
#include <gbwt/fast_locate.h>


// GBWTGraph
#include <gbwtgraph/gbwtgraph.h>
#include <gbwtgraph/gfa.h>
#include <gbwtgraph/gbz.h>

// HandleGraph
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>

#include "gtest/gtest.h"
//TEST
#include "gtest/gtest.h"
/**
 * This class represent the paths' prefix sum arrays.
 */

namespace pathsprefixsumarrays{

class PathsPrefixSumArrays {

private:
    friend class PrefixSumArraysTest;
    FRIEND_TEST(PrefixSumArraysTest, get_distance_between_positions_in_path);
    FRIEND_TEST(PrefixSumArraysTest, get_all_nodes_distances);


    ///friend class my::_test_paths_prefix_sum_arrays::::PrefixSumArraysTest;
    ///TEST_F(PrefixSumArraysTest, GetAllNodeDistanceInAPath);

    std::map<gbwt::size_type, sdsl::sd_vector<> *>* psa; // prefix sum arrays (seq_id, prefix sum array)

    gbwt::FastLocate *fast_locate; // it is needed to perform select operation on sd_vector


    /**
     * Compute distance between two node position in a path, it's an auxiliar method used by the public
     * get_distance_between_positions_in_path.
     * @param pos_node_1
     * @param pos_node_2
     * @param sdb_sel is the select operation.
     * @return the distance between the two positions.
     */
    size_t get_distance_between_positions_in_path_aux(size_t pos_node_1, size_t pos_node_2,
                                                      sdsl::sd_vector<>::select_1_type &sdb_sel);


    /**
     * Get all the positions of a node in a path.
     * @param path_id it's the sequence id (not GBWTGraph representation).
     * @param node
     * @param ones the number of ones inside the sd_vector prefix sum array representation. It is needed to compute the
     * positions.
     * @return a vector of the positions of a node in the path.
     */
    std::vector<size_t>* get_positions_of_a_node_in_path(size_t path_id, gbwt::node_type node, size_t &ones);

public:
    /**
     * Default constructor.
     */
    PathsPrefixSumArrays();

    /**
     * Constructor.
     * @param gbwtGraph
     */
    PathsPrefixSumArrays(gbwtgraph::GBWTGraph &gbwtGraph);


    /**
     * Destructor.
     */
    ~PathsPrefixSumArrays();

    /**
     * Get a string with all the prefix sum arrays as sd_vectors representation (0,1).
     * @return a string containing the prefix sum arrays.
     */
    std::string toString_sd_vectors() const;


    /**
     * Get a string with all the prefix sum arrays as arrays of integers.
     * @return a string representing the prefix sum arrays.
     */
    std::string toString() const;


    /**
     * Get fast locate used in Test.
     * @return fast_locate.
     */
    gbwt::FastLocate* get_fast_locate() const;


    /**
     * Get prefix sum arrays.
     * @return a map in which for each path we have the prefix sum array.
     */
    std::map<gbwt::size_type, sdsl::sd_vector<>*>* get_prefix_sum_arrays() const;


    /**
     * Given the path_id and two position node inside that path, compute the distance between the positions inside the
     * path.
     * @param pos_node_1
     * @param pos_node_2
     * @param path_id
     * @return the distance.
     */
    size_t get_distance_between_positions_in_path(size_t pos_node_1, size_t pos_node_2, size_t path_id);


    /**
     * Get all the distances between two nodes in a path, also takes into account multiple occurences of the same node
     * in a looping path.
     * @param node_1 id of the node.
     * @param node_2 id of the node.
     * @param path_id id of the path (sequence).
     * @return a vector of size_t distances.
     */
    std::vector<size_t> *get_all_nodes_distances_in_path(gbwt::node_type node_1, gbwt::node_type node_2, size_t path_id);


    /**
     * Get all node distance between two nodes.
     * @param node_1
     * @param node_2
     * @return a vector with all the distance between two nodes.
     */
    std::vector<size_t>* get_all_nodes_distances(gbwt::node_type node_1, gbwt::node_type node_2);


    /**
     * Get all node positions in every path that visits the node.
     * @param node
     * @return a map where the key is the path id (sequence id) and the value is a pointer to a vector of positions in
     * that path.
     */
    std::map<size_t,std::vector<size_t>*>* get_all_node_positions(gbwt::node_type node);


    /**
     * Get all distances between two nodes in a path. Each nodes can occur several time in a path in different positions.
     * @param node_1_positions positions of node_1 inside the path (sequence)
     * @param node_2_positions positions of node_1 inside the path (sequence)
     * @param path_id
     * @return a vector of all the distances between the two nodes in a path.
     */
    std::vector<size_t>* get_all_nodes_distances_in_path( std::vector<size_t>* node_1_positions,
                                                                                std::vector<size_t>* node_2_positions,
                                                                                size_t path_id);
};
}// fine del namespace
#endif //CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H