//
// Created by GI-Loaner-05 on 6/15/22.
//

#include "../include/consensus_distance/paths_prefix_sum_arrays.h"
#include "sdsl/util.hpp"
#include <bits/stdc++.h> // To sort

PathsPrefixSumArrays::PathsPrefixSumArrays(): psa(nullptr){}


PathsPrefixSumArrays::PathsPrefixSumArrays(GBWTGraph gbwtGraph) {

    //create the prefix sum array
    std::map<gbwt::size_type , sdsl::sd_vector<>>* paths;
    psa = new std::map<gbwt::size_type , sdsl::sd_vector<>*>();
    for(gbwt::size_type i = 0; i < gbwtGraph.index->sequences(); i += 2) {
        auto path = gbwtGraph.index->extract(i);

        //test print
    /*    for (int j = 0; j < path.size() ; ++j) {
            std::cout << path[j] << " ";
        }
*/
    std::cout <<std::endl;

        size_t offset =0;

        for (gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            std::cout<< length_of_node << " ";
            offset+=length_of_node;
        }
        sdsl::bit_vector psa_temp(offset+1,0);

        offset =0;
        for (gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset+=length_of_node;
            psa_temp[offset]=1;
        }
        sdsl::sd_vector<>* vector = new sdsl::sd_vector<>(psa_temp);
        (*psa)[i]= vector;
    }

    //create the fast locate
    fast_locate = gbwt::FastLocate(*gbwtGraph.index);

}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path(size_t pos_node_1, size_t pos_node_2, size_t path_id){
    size_t distance = 0;

    // Distance between the same node
    if(pos_node_1 == pos_node_2)
        return distance;

    sdsl::sd_vector<>::select_1_type sdb_sel((*(psa))[path_id]); // Initialize select

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

}


std::vector<path_handle_t>* PathsPrefixSumArrays::get_graph_path_handles(GBWTGraph &g){
    std::vector<path_handle_t> *path_handles = new std::vector<path_handle_t>();
    g.for_each_path_handle([&](const path_handle_t path_handle) {
        (*path_handles).push_back(path_handle);
    }); // end of lambda expression)
    return path_handles;
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
    auto zeros = sdsl::sd_vector<>::rank_0_type(&(*(*psa)[path_id]))(psa[path_id].size());
    auto ones = ((*psa)[path_id])->size() - zeros;

    std::vector<size_t>* node_1_positions = get_visits_in_path(path_id, node_1, ones);
    std::vector<size_t>* node_2_positions = get_visits_in_path(path_id, node_2, ones);

    std::sort(node_1_positions->begin(), node_1_positions->end());
    std::sort(node_2_positions->begin(), node_2_positions->end());



    int pivot_1 = 0, pivot_2 = 0;

    int i, j, end;

    std::vector<size_t>* distances = new std::vector<size_t>();

    bool iterate_on_node_2_positions;

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
    }while(pivot_1 < node_1_positions->size() &&
            pivot_2 < node_2_positions->size());


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


std::vector<size_t>* PathsPrefixSumArrays::get_visits_in_path(size_t path_id, gbwt::node_type node, size_t &ones){
    auto node_visits = fast_locate.decompressSA(node);

    std::vector<size_t>* node_positions = new std::vector<size_t>();

    std::cout << "\nSEQTHINGS: ";
    for (int i = 0; i < node_visits.size() ; ++i) {
        if(fast_locate.seqId(node_visits[i]) == path_id){
            std::cout<< "SeqOffset: " << fast_locate.seqOffset(node_visits[i]);
            std::cout<< " - SeqId: " << fast_locate.seqId(node_visits[i]);
            node_positions->push_back(ones - fast_locate.seqOffset(node_visits[i]));

        }
    }

    std::cout << std::endl;

    return node_positions;
}


