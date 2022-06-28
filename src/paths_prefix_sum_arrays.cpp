//
// Created by GI-Loaner-05 on 6/15/22.
//

#include "../include/consensus_distance/paths_prefix_sum_arrays.h"
#include "sdsl/util.hpp"
#include <bits/stdc++.h> // To sort

PathsPrefixSumArrays::PathsPrefixSumArrays(): psa(nullptr){}


PathsPrefixSumArrays::PathsPrefixSumArrays(GBWTGraph gbwtGraph) {
    // Create the prefix sum array
    psa = new std::map<gbwt::size_type , sdsl::sd_vector<>*>();

    std::cout << std::endl;

    for(gbwt::size_type i = 0; i < gbwtGraph.index->sequences(); i += 2) {
        // += 2 because the id of the paths is multiple of two, every path has its reverse path and in GBWTGraph this
        // is the representation

        auto path = gbwtGraph.index->extract(i);

        size_t offset = 0;
        for(gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            std::cout<< length_of_node << " ";
            offset += length_of_node;
        }

        sdsl::bit_vector psa_temp(offset+1,0);
        std::cout << std::endl;

        offset =0;
        for(gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset += length_of_node;
            psa_temp[offset] = 1;
        }

        (*psa)[i] = new sdsl::sd_vector<>(psa_temp);
    }

    //create the fast locate
    fast_locate = new gbwt::FastLocate(*gbwtGraph.index);
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path(size_t pos_node_1, size_t pos_node_2, size_t path_id){
    size_t distance = 0;

    // Distance between the same node
    if(pos_node_1 == pos_node_2)
        return distance;

    // Initialize select operation (see select/rank)
    sdsl::sd_vector<>::select_1_type sdb_sel((*(psa))[path_id]);

    if(pos_node_2 > (*(psa))[path_id]->size() || pos_node_1 > (*(psa))[path_id]->size()){
        return 0; // You can't compute the distance between two node where at least one doesn't exist
    }else if (pos_node_1 < pos_node_2) {
        distance = get_distance_between_positions_in_path_aux(pos_node_1, pos_node_2, sdb_sel);
    } else {
        distance = get_distance_between_positions_in_path_aux(pos_node_2, pos_node_1, sdb_sel);
    }

    return distance;
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path_aux(size_t pos_node_1, size_t pos_node_2, sdsl::sd_vector<>::select_1_type &sdb_sel){
    size_t node_before_bigger_node_offset = sdb_sel(pos_node_2);
    size_t node_1_offset = sdb_sel(pos_node_1 + 1);
    return node_before_bigger_node_offset - node_1_offset;
}


PathsPrefixSumArrays::~PathsPrefixSumArrays() {
    // Delete memory fast locate
    delete fast_locate;
    fast_locate = nullptr;

    // Delete map memory
    std::map<gbwt::size_type , sdsl::sd_vector<>*>::iterator it;

    for (it = psa->begin(); it != psa->end(); it++){
        delete it->second;
        it->second = nullptr;
    }

    psa->clear();
    delete psa;
    psa = nullptr;
}


std::string PathsPrefixSumArrays::toString_sd_vectors(){
    std::string temp="";
    auto iterator = (*psa).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*psa).end())
    {
        temp +=  "\n| ";

        // Accessing KEY from element pointed by it.
        temp += std::to_string(iterator->first) + "::\t[";

        // Accessing VALUE from element pointed by it.
        for(int i=0; i< iterator->second->size(); ++i){
            temp += std::to_string((*(iterator->second))[i] );
            if(i!=iterator->second->size()-1)
            temp += ", ";
        }
        temp += "]";
        // Increment the Iterator to point to next entry
        iterator++;
    }
    return temp;
}


std::string PathsPrefixSumArrays::toString(){
    std::string temp="";
    auto iterator = (*psa).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*psa).end())
    {
        temp +=  "\n| ";

        // Accessing KEY from element pointed by it.
        temp += std::to_string(iterator->first) + "::\t[";

        // Accessing VALUE from element pointed by it.
        for(int i=0; i< iterator->second->size(); ++i){
            if((*(iterator->second))[i] == 1) {
                temp += std::to_string(i);
                if (i != iterator->second->size() - 1)
                    temp += ", ";
            }
        }
        temp += "]";
        // Increment the Iterator to point to next entry
        iterator++;
    }
    return temp;
}


std::vector<size_t>* PathsPrefixSumArrays::get_all_nodes_distances_in_path( gbwt::node_type node_1,
                                                                            gbwt::node_type node_2,
                                                                            size_t path_id){
    // Used to compute the number of nodes inside a path
    auto zeros = sdsl::sd_vector<>::rank_0_type(&(*(*psa)[path_id]))(psa[path_id].size());
    auto ones = ((*psa)[path_id])->size() - zeros;

    // Get nodes positions within a path, a node in a loop can occurr several times
    std::vector<size_t>* node_1_positions = get_positions_of_a_node_in_path(path_id, node_1, ones);
    std::vector<size_t>* node_2_positions = get_positions_of_a_node_in_path(path_id, node_2, ones);

    // Sort the position nodes in each vector
    std::sort(node_1_positions->begin(), node_1_positions->end());
    std::sort(node_2_positions->begin(), node_2_positions->end());

    int pivot_1 = -1, pivot_2 = -1;
    int i, j, end;

    bool iterate_on_node_2_positions;

    std::vector<size_t>* distances = new std::vector<size_t>();

    /**
     * Explanation of the algorithm: the idea is to check which of the two first positions (in the vector of positions)
     * is greater and iterate on the greater one.
     *
     * For instance: if the first item of the vector node_1_positions is the less, we set a index position i on the pivot_1,
     * we set j to pivot_2 and we set as end position the node_2_position.size(). We increment the pivot_1 because it
     * will be used in the next iteration.
     *
     * The pivot_1 is the index of the position for which have to compute the distances during a iteration, this index
     * refers to node_1_postions vector (pivot_2 refers to node_2_positions).
     *
     * Based on the boolean flag iterate_on_node_2_positions we iterate (j) on the first or the second vector of positions
     * to compute the distance with the fixed one (i) in that iteration.
     */

    do{
        if(node_1_positions->at(pivot_1) < node_2_positions->at(pivot_2)){
            j = pivot_2;
            end = node_2_positions->size();
            i = pivot_1;
            ++ pivot_1;
            iterate_on_node_2_positions = true;
        }else{
            j = pivot_1;
            end = node_1_positions->size();
            i = pivot_2;
            ++ pivot_2;
            iterate_on_node_2_positions = false;
        }

        while(j < end){
            if(iterate_on_node_2_positions) {
                (*distances).push_back(PathsPrefixSumArrays::get_distance_between_positions_in_path(node_1_positions->at(i),
                                                                                                    node_2_positions->at(j),
                                                                                                    path_id));
            }else{
                (*distances).push_back(PathsPrefixSumArrays::get_distance_between_positions_in_path(node_1_positions->at(j),
                                                                                                    node_2_positions->at(i),
                                                                                                    path_id));
            }
            ++ j;
        }
    }while(pivot_1 < node_1_positions->size() && pivot_2 < node_2_positions->size());


    // Deleting memory
    node_1_positions->clear();
    node_2_positions->clear();
    node_1_positions->shrink_to_fit();
    node_2_positions->shrink_to_fit();

    delete node_1_positions;
    delete node_2_positions;
    node_1_positions = nullptr;
    node_2_positions = nullptr;

    return distances;
};


std::vector<size_t>* PathsPrefixSumArrays::get_positions_of_a_node_in_path(size_t path_id, gbwt::node_type node, size_t &ones){
    auto node_visits = fast_locate->decompressSA(node);

    std::vector<size_t>* node_positions = new std::vector<size_t>();

    for (int i = 0; i < node_visits.size() ; ++i) {
        if(fast_locate->seqId(node_visits[i]) == path_id){
            node_positions->push_back(ones - fast_locate->seqOffset(node_visits[i]));
        }
    }

    return node_positions;
}


