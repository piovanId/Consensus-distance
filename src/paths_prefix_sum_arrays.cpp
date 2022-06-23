//
// Created by GI-Loaner-05 on 6/15/22.
//

#include "../include/consensus_distance/paths_prefix_sum_arrays.h"

PathsPrefixSumArrays::PathsPrefixSumArrays(): prefix_sum_arrays(nullptr){}


PathsPrefixSumArrays::PathsPrefixSumArrays(GBWTGraph gbwtGraph) {
    /*
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* paths =  get_paths(gbwtGraph);

    // Create a map iterator and point to beginning of map
    auto iterator = (*paths).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*paths).end())
    {
        // Accessing KEY from element pointed by it.
        std::string path_name = gbwtGraph.get_path_name(iterator->first);
        // Accessing VALUE from element pointed by it.
        std::vector<std::pair<handle_t ,int>>* nodes = iterator->second;
        if((*nodes).size()>1){
            for(int i=1; i< (*nodes).size(); ++i){
                (*nodes)[i].second=(*nodes)[i].second+(*nodes)[i-1].second;
            }
        }
        // Increment the Iterator to point to next entry
        iterator++;
    }

    prefix_sum_arrays = paths;
    paths = nullptr;

    */

    std::map<gbwt::size_type , sdsl::sd_vector<>>* paths ;
    psa = new std::map<gbwt::size_type , sdsl::sd_vector<>*>();
    for(gbwt::size_type i = 0; i < gbwtGraph.index->sequences(); i += 2) {
        auto path = gbwtGraph.index->extract(i);

        //test print
        for (int j = 0; j < path.size() ; ++j) {
            std::cout << path[j] << " ";
        }
        std::cout <<std::endl;

        size_t offset =0;

        for (int j = 0; j < path.size(); ++j) {
            int length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset+=length_of_node;
        }
        sdsl::bit_vector psa_temp(offset+1,0);

        offset =0;
        for (int j = 0; j < path.size(); ++j) {
            int length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset+=length_of_node;
            psa_temp[offset]=1;
        }
        sdsl::sd_vector<>* vector = new sdsl::sd_vector<>(psa_temp);
        (*psa)[i]= vector;
    }

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


std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* PathsPrefixSumArrays::get_paths(GBWTGraph gbwtGraph){
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*> *paths_steps_length = new std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>();

    auto path_handles = get_graph_path_handles(gbwtGraph);

    //OCHO ALLA MEMORIA QUA
    for (auto path_handle : (*path_handles)) {
        (*paths_steps_length).insert({path_handle, new std::vector<std::pair<handle_t , int>>()});

        gbwtGraph.for_each_step_in_path(path_handle,[&](const step_handle_t step_handle) {
            auto handle = gbwtGraph.get_handle_of_step(step_handle);

            (*paths_steps_length)[path_handle]->push_back(std::pair<handle_t , int>(handle, gbwtGraph.get_length(handle)));
        }); // end of lambda expression));
    }

    // Deleting path_handles memory
    path_handles->clear();
    delete path_handles;
    path_handles = nullptr;

    return paths_steps_length;
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
        temp +=  "\n | ";

        // Accessing KEY from element pointed by it.
        temp += std::to_string(iterator->first) + " :: [";

        // Accessing VALUE from element pointed by it.
        for(int i=0; i< iterator->second->size(); ++i){
            temp += std::to_string((*(iterator->second))[i] );
        }

        // Increment the Iterator to point to next entry
        iterator++;
    }
    return temp;
}

std::vector<std::pair<handle_t , int>> PathsPrefixSumArrays::get_prefsum_of_path(path_handle_t path_handle){
    return *prefix_sum_arrays->at(path_handle);
}

std::string PathsPrefixSumArrays::print_prefsum_of_path(path_handle_t path_handle){
    std::string temp;

    temp += std::to_string(handlegraph::as_integer(path_handle)) + " :: ";


    std::vector<std::pair<handle_t ,int>> nodes = *prefix_sum_arrays->at(path_handle);
    for(int i=0; i< nodes.size(); ++i){
        temp += std::to_string(nodes[i].second) + "  ";
    }
    return temp;
}
