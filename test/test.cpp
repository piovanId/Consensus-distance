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
using namespace gbwtgraph;


std::vector<std::string> get_graph_path_names(GBWTGraph g){
    std::vector<std::string> names;

    g.for_each_path_handle([&](const path_handle_t t) {
        names.push_back(g.get_path_name(t));
    }); // end of lambda expression)
    return names;
}
std::vector<path_handle_t> get_graph_path_handles(GBWTGraph g){
    std::vector<path_handle_t> path_handles;
    g.for_each_path_handle([&](const path_handle_t t) {
        path_handles.push_back(t);
    }); // end of lambda expression)
    return path_handles;
}


int main() {
    std::cout << "Hello, World!" << std::endl;

    std::map<std::string, std::vector<int>> paths_steps_length;
    std::vector<int> empty;
    //auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/GBWTgraph stuff/gbwtgraph/tests/gfas/default.gfa");
    auto gfa_parse = gfa_to_gbwt("/Users/gi-loaner-05/tesi/vg/test/graphs/components_paths_walks.gfa");

    const gbwt::GBWT& index = *(gfa_parse.first);
    GBWTGraph graph(*(gfa_parse.first), *(gfa_parse.second));

    auto names = get_graph_path_names(graph);
    auto handles = get_graph_path_handles(graph);

    for ( auto name : names )
        paths_steps_length.insert({name, empty});


    for ( auto path_handle : handles )
        graph.for_each_step_in_path(path_handle,[&](const step_handle_t t) {
            auto handle_step = graph.get_handle_of_step(t);
            paths_steps_length[graph.get_path_name(path_handle)].push_back(graph.get_length( handle_step));
        }); // end of lambda expression));

    // Create a map iterator and point to beginning of map
    std::map<std::string, std::vector<int>>::iterator iterator = paths_steps_length.begin();

    // Iterate over the map using Iterator till end.
    while (iterator != paths_steps_length.end())
    {
        // Accessing KEY from element pointed by it.
        std::string word = iterator->first;
        // Accessing VALUE from element pointed by it.
        std::vector<int> count = iterator->second;
        std::cout << word << " :: ";
        for(auto i : paths_steps_length[word]){
            std::cout<< i << " ";
        }
        std::cout << std::endl;
        // Increment the Iterator to point to next entry
        iterator++;
    }

    return 0;
}
