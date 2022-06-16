//
// Created by GI-Loaner-05 on 6/15/22.
//

#ifndef CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H
#define CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include "gbwtgraph/gbwtgraph.h"
#include "gbwtgraph/gfa.h"
#include "gbwtgraph/gbz.h"
#include "gbwt/gbwt.h"
#include <handlegraph/handle_graph.hpp>
#include <handlegraph/mutable_handle_graph.hpp>
#include <handlegraph/mutable_path_deletable_handle_graph.hpp>
#endif //CONSENSUS_DISTANCE_PREFIX_SUM_ARRAY_H

using namespace gbwtgraph;

class PrefixSumArray{


private:
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*> *prefix_sum_array ;

    std::vector<path_handle_t>* get_graph_path_handles(GBWTGraph &g);


    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* get_paths(GBWTGraph gbwtGraph);

public:

    PrefixSumArray();

    PrefixSumArray(GBWTGraph gbwtGraph);


};

