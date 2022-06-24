//
// Created by GI-Loaner-05 on 6/24/22.
//

#include "../include/consensus_distance/pruned_graph.h"

PrunedGraph::PrunedGraph(GBWTGraph g){
    prefix_sum_arrays = new PathsPrefixSumArrays(g);
}