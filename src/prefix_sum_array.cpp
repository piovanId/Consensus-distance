//
// Created by GI-Loaner-05 on 6/15/22.
//

#include "../include/consensus_distance/prefix_sum_array.h"

std::vector<path_handle_t>* PrefixSumArray::get_graph_path_handles(GBWTGraph &g){
    std::vector<path_handle_t> *path_handles = new std::vector<path_handle_t>();
    g.for_each_path_handle([&](const path_handle_t path_handle) {
        (*path_handles).push_back(path_handle);
    }); // end of lambda expression)
    return path_handles;
}

std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>* PrefixSumArray::get_paths(GBWTGraph gbwtGraph){
    std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*> *paths_steps_length = new std::map<path_handle_t , std::vector<std::pair<handle_t , int>>*>();

    //VA ELIMINATA LA MEMORIA QUA dopo che lo usi
    auto path_handles = get_graph_path_handles(gbwtGraph);

    //OCHO ALLA MEMORIA QUA
    for (auto path_handle : (*path_handles)) {
        (*paths_steps_length).insert({path_handle, new std::vector<std::pair<handle_t , int>>()});

        gbwtGraph.for_each_step_in_path(path_handle,[&](const step_handle_t step_handle) {
            auto handle = gbwtGraph.get_handle_of_step(step_handle);

            (*paths_steps_length)[path_handle]->push_back(std::pair<handle_t , int>(handle, gbwtGraph.get_length(handle)));
        }); // end of lambda expression));
    }

    return paths_steps_length;
}

PrefixSumArray::PrefixSumArray(){

}

PrefixSumArray::PrefixSumArray(GBWTGraph gbwtGraph) {
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

    iterator = (*paths).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*paths).end())
    {
        // Accessing KEY from element pointed by it.
        std::string path_name = gbwtGraph.get_path_name(iterator->first);
        // Accessing VALUE from element pointed by it.
        std::vector<std::pair<handle_t ,int>> nodes = *(iterator->second);
        std::cout << path_name << " :: ";
        if(nodes.size()>1){
            for(int i=0; i< nodes.size(); ++i){
                std::cout << nodes[i].second << " ";
            }
        }

        std::cout << std::endl;
        // Increment the Iterator to point to next entry
        iterator++;
    }

    this->prefix_sum_array = paths;
}