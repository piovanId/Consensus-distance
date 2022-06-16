//
// Created by GI-Loaner-05 on 6/15/22.
//
// TODO: implement a method that take the path_handle and return directly its prefix sum array.
// TODO: implement a method "build_prexif_sum_arrays" (we can discuss about the name)
//  to get the constructor nice and clean (remember memory leaks).
// TODO: implement anothe to_stirng that takes as a parameter a single path_handle, if you want to see just one

#ifndef CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H
#define CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H

// Standard
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

// GBWT
#include "gbwt/gbwt.h"

// GBWTGraph
#include "gbwtgraph/gbwtgraph.h"
#include "gbwtgraph/gfa.h"
#include "gbwtgraph/gbz.h"

// HandleGraph
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>

using namespace gbwtgraph;

/**
 * This class represent the paths' prefix sum arrays.
 */
class PathsPrefixSumArrays{

private:
    // Every vector has as a key its path_handle_t, each entry in the vector is stored with the related handle_t
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*> *prefix_sum_arrays;

    /**
     * Get the all the path handles in the graph, which is a reference to a path (opaque 64-bit identifier).
     * @param g the graph on which we are getting the path_handles.
     * @return a pointer to vector of path_handle_t type.
     */
    std::vector<path_handle_t>* get_graph_path_handles(GBWTGraph &g);

    /**
     * Get all the paths from a GBWTGraph.
     * @param gbwtGraph
     * @return a map of vectors (path_handle_t as key). Each vector, in each entru has the length of the node and the
     * relative handle_t.
     */
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* get_paths(GBWTGraph gbwtGraph);


public:
    /**
     * Default constructor
     */
    PathsPrefixSumArrays();

    /**
     *
     * @param gbwtGraph
     */
    PathsPrefixSumArrays(GBWTGraph gbwtGraph);

    /**
     * Get the prefix sum arrays.
     * @return prefix sum arrays in a map
     */
    const std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* get_prefix_sum_arrays() const;

    /**
     * Destroyer
     */
    ~PathsPrefixSumArrays();

    /**
     * Get a string with all the prefix sum arrays.
     * @return a string containing the prefix sum arrays.
     */
    std::string toString();
};

#endif //CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H