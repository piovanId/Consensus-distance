/**
 * Authors:
 *  - Andrea Mariotti
 *  - Davide Piovani
 */

#include "../include/consensus_distance/pruned_graph.h"

/**
 * Default constructor.
 */
PrunedGraph::PrunedGraph(): prefix_sum_arrays(nullptr) {}

/**
 * Constructor.
 * @param g graph GBWTGraph.
 */
PrunedGraph::PrunedGraph(gbwtgraph::GBWTGraph g){
    prefix_sum_arrays = new pathsprefixsumarrays::PathsPrefixSumArrays(g);
}