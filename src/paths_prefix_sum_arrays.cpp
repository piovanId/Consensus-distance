//
// Created by GI-Loaner-05 on 6/15/22.
//

#include "../include/consensus_distance/paths_prefix_sum_arrays.h"
#include "sdsl/util.hpp"

PathsPrefixSumArrays::PathsPrefixSumArrays(): prefix_sum_arrays(nullptr){}


PathsPrefixSumArrays::PathsPrefixSumArrays(GBWTGraph gbwtGraph) {
    std::map<gbwt::size_type , sdsl::sd_vector<>>* paths ;
    psa = new std::map<gbwt::size_type , sdsl::sd_vector<>*>();
    for(gbwt::size_type i = 0; i < gbwtGraph.index->sequences(); i += 2) {
        auto path = gbwtGraph.index->extract(i);

        //test print
    /*    for (int j = 0; j < path.size() ; ++j) {
            std::cout << path[j] << " ";
        }
*/      std::cout <<std::endl;

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

}


size_t PathsPrefixSumArrays::get_distance_in_a_path(size_t pos_node_1, size_t pos_node_2, size_t path_id){
    size_t distance = 0;

    // Distance between the same node
    if(pos_node_1 == pos_node_2)
        return distance;

    sdsl::sd_vector<>::select_1_type sdb_sel((*(psa))[path_id]); // Initialize select

    if(pos_node_2 > (*(psa))[path_id]->size() || pos_node_1 > (*(psa))[path_id]->size()){
        return 0; // You can't compute the distance between two node where at least one doesn't exist
    }else if (pos_node_1 < pos_node_2) {
        distance = compute_node_distance(pos_node_1, pos_node_2, sdb_sel);
    } else {
        distance = compute_node_distance(pos_node_2, pos_node_1, sdb_sel);
    }

    return distance;
}


size_t PathsPrefixSumArrays::compute_node_distance(size_t pos_node_1, size_t pos_node_2, sdsl::sd_vector<>::select_1_type &sdb_sel){
    size_t node_before_bigger_node_offset = sdb_sel(pos_node_2);
    size_t node_1_offset = sdb_sel(pos_node_1 + 1);
    return node_before_bigger_node_offset - node_1_offset;
}




PathsPrefixSumArrays::~PathsPrefixSumArrays() {
    auto iterator = (*prefix_sum_arrays).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*prefix_sum_arrays).end())
    {
        // Delete the vector object
        (*(iterator->second)).clear();
        delete iterator->second;
        iterator->second = nullptr;

        // Increment the Iterator to point to next entry
        iterator++;
    }

    // Deleting the map
    prefix_sum_arrays->clear();
    delete prefix_sum_arrays;
    prefix_sum_arrays = nullptr;
}


std::vector<path_handle_t>* PathsPrefixSumArrays::get_graph_path_handles(GBWTGraph &g){
    std::vector<path_handle_t> *path_handles = new std::vector<path_handle_t>();
    g.for_each_path_handle([&](const path_handle_t path_handle) {
        (*path_handles).push_back(path_handle);
    }); // end of lambda expression)
    return path_handles;
}






//ocho memoria
const std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* PathsPrefixSumArrays::get_prefix_sum_arrays() const{
    return PathsPrefixSumArrays::prefix_sum_arrays;
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

std::vector<std::pair<handle_t , int>> PathsPrefixSumArrays::get_prefsum_of_path(path_handle_t path_handle){
    return *prefix_sum_arrays->at(path_handle);
}

