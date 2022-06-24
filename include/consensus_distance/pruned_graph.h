//
// Created by GI-Loaner-05 on 6/24/22.
//

#ifndef CONSENSUS_DISTANCE_PRUNED_GRAPH_H
#define CONSENSUS_DISTANCE_PRUNED_GRAPH_H
#include "paths_prefix_sum_arrays.h"
#include "gbwt/gbwt.h"
#include "gbwtgraph/gbwtgraph.h"



class PrunedGraph {
    PathsPrefixSumArrays *prefix_sum_arrays;

public:
    PrunedGraph();
    PrunedGraph(GBWTGraph g);



};


#endif //CONSENSUS_DISTANCE_PRUNED_GRAPH_H
