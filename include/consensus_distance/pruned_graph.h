/**
 * Authors:
 *  - Andrea Mariotti
 *  - Davide Piovani
 */

#ifndef CONSENSUS_DISTANCE_PRUNED_GRAPH_H
#define CONSENSUS_DISTANCE_PRUNED_GRAPH_H

// GBWT
#include <gbwt/gbwt.h>

// GBWTGraph
#include <gbwtgraph/gbwtgraph.h>

// Custom
#include "paths_prefix_sum_arrays.h"



class PrunedGraph {
    pathsprefixsumarrays::PathsPrefixSumArrays *prefix_sum_arrays;

public:
    PrunedGraph();
    PrunedGraph(gbwtgraph::GBWTGraph g);



};


#endif //CONSENSUS_DISTANCE_PRUNED_GRAPH_H
