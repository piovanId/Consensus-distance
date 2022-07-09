/**
 * Authors:
 *  - Andrea Mariotti
 *  - Davide Piovani
 *
 *  ATTENTION:
 *  To convert a GFA file into a gbwt object we use the method gbwtgraph::gfa_to_gbwt. This method returns an object of
 *  type: std::pair<std::unique_ptr<gbwt::GBWT>, std::unique_ptr<gbwtgraph::SequenceSource>>, let's call it gfa_parse.
 *  This type is used to create a GBWTGraph object: new gbwtgraph::GBWTGraph(gfa_parse.first, gfa_parse.second).
 *
 *  Using this class pay attention to not deallocate from stack/heap the gfa_parse used to create the GBWTGraph, because
 *  it will lead to memory errors due to the internal pointers of the GBWTGraph class.
 */


#include <sdsl/util.hpp>
#include <stdexcept>

#include "../include/consensus_distance/paths_prefix_sum_arrays.h"

using namespace pathsprefixsumarrays;
//TODO: remove this method
void stamparrei(std::vector<size_t> mauro){
    for (int i = 0; i < mauro.size(); ++i) {
        std::cout<< mauro[i]<<" ";
    }
    std::cout<<std::endl;
}


PathsPrefixSumArrays::PathsPrefixSumArrays(): psa(nullptr), fast_locate(nullptr) {}


PathsPrefixSumArrays::PathsPrefixSumArrays(gbwtgraph::GBWTGraph &gbwtGraph){
    // Create the prefix sum array
    psa = new std::map<gbwt::size_type , sdsl::sd_vector<>*>();

    // Build prefix sum array for each path (sequence)
    for(gbwt::size_type i = 0; i < (gbwtGraph.index)->sequences(); i += 2) {
        // += 2 because the id of the paths is multiple of two, every path has its reverse path and in GBWTGraph this
        // is the representation
        auto path = gbwtGraph.index->extract(i); // Attention: it's the sequence representation

        size_t offset = 0;
        for(gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset += length_of_node;
        }


        sdsl::bit_vector psa_temp(offset+1,0);

        offset =0;
        for(gbwt::size_type j = 0; j < path.size(); ++j) {
            gbwt::size_type length_of_node = gbwtGraph.get_length( gbwtGraph.node_to_handle(path[j]));
            offset += length_of_node;
            psa_temp[offset] = 1;
        }

        (*psa)[i] = new sdsl::sd_vector<>(psa_temp);
    }

    // Create the fast locate
    fast_locate = new gbwt::FastLocate(*gbwtGraph.index);
}


gbwt::FastLocate* PathsPrefixSumArrays::get_fast_locate() const{
    return fast_locate;
}


std::map<gbwt::size_type, sdsl::sd_vector<>*>* PathsPrefixSumArrays::get_prefix_sum_arrays() const{
    return psa;
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path(size_t pos_node_1, size_t pos_node_2, size_t path_id){
    size_t distance = 0;

    // Distance between the same node
    if(pos_node_1 == pos_node_2)
        return distance;

    // Initialize select operation (see select/rank)
    sdsl::sd_vector<>::select_1_type sdb_sel((*(psa))[path_id]);

    if(pos_node_2 > (*(psa))[path_id]->size() || pos_node_1 > (*(psa))[path_id]->size()){
        return 0; // You can't compute the distance between two position if at least one of them doesn't exist
    } else {
        distance = get_distance_between_positions_in_path_aux(pos_node_1, pos_node_2, sdb_sel);
    }

    return distance;
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path_aux(size_t pos_node_1, size_t pos_node_2, sdsl::sd_vector<>::select_1_type &sdb_sel){
    if(pos_node_1 > pos_node_2){
        std::swap(pos_node_1, pos_node_2);
    }

    size_t node_before_bigger_node_offset = sdb_sel(pos_node_2);
    size_t node_1_offset = sdb_sel(pos_node_1 + 1);

    return node_before_bigger_node_offset - node_1_offset;
}


PathsPrefixSumArrays::~PathsPrefixSumArrays() {
    // Delete memory fast locate
    if(fast_locate != nullptr){
        delete fast_locate;
        fast_locate = nullptr;
    }

    // Delete map memory
    if(psa != nullptr){
        std::map<gbwt::size_type , sdsl::sd_vector<>*>::iterator it;

        for (it = psa->begin(); it != psa->end(); it++){
            delete it->second;
            it->second = nullptr;
        }

        psa->clear();
        delete psa;
        psa = nullptr;
    }
}


std::string PathsPrefixSumArrays::toString_sd_vectors() const{
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


std::string PathsPrefixSumArrays::toString() const{
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

    // Ones: the number of ones in the sd_vector correspond to the number of nodes inside a path
    size_t ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id]))(((*psa)[path_id])->size());


    // Get nodes positions within a path, a node in a loop can occurr several times
    std::vector<size_t>* node_1_positions = get_positions_of_a_node_in_path(path_id, node_1, ones);
    std::vector<size_t>* node_2_positions = get_positions_of_a_node_in_path(path_id, node_2, ones);


    return get_all_nodes_distances_in_path(node_1_positions, node_2_positions, path_id);
}


std::vector<size_t>* PathsPrefixSumArrays::get_positions_of_a_node_in_path(size_t path_id, gbwt::node_type node, size_t &ones){
    auto node_visits = fast_locate->decompressSA(node);

    if(node == 0 || node_visits.empty()){
        throw NodeNotInPathsException();
    }

    std::vector<size_t>* node_positions = new std::vector<size_t>();

    for (int i = 0; i < node_visits.size() ; ++i) {
        if(fast_locate->seqId(node_visits[i]) == path_id){
            node_positions->push_back(ones - fast_locate->seqOffset(node_visits[i]) - 1);
        }
    }

    return node_positions;
}


// TODO: it could be helpful know at which path every distances belongs to
std::vector<size_t>* PathsPrefixSumArrays::get_all_nodes_distances(gbwt::node_type node_1, gbwt::node_type node_2) {
    std::vector<size_t> *distances = new std::vector<size_t>();


    /**
     * try {
        positions_node_1 = get_all_node_positions(node_1);
        positions_node_2 = get_all_node_positions(node_2);
    }catch(NodeNotInPathsException &ex){
        delete distances;
        distances = nullptr;
    }
     */
    //deletare memoria
    std::map<size_t, std::vector<size_t> *> *positions_node_1 = get_all_node_positions(node_1);
    std::map<size_t, std::vector<size_t> *> *positions_node_2 = get_all_node_positions(node_2);

    if (positions_node_1->empty() || positions_node_2->empty()) {
        return distances;
    }

    auto iterator = (*positions_node_1).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*positions_node_1).end())
    {
        size_t key = iterator->first; // sequence id
        std::vector<size_t> *distances_in_path;

        if((*positions_node_2).find(key)  != (*positions_node_2).end()) {
            distances_in_path = get_all_nodes_distances_in_path((*positions_node_1)[key],
                                                                (*positions_node_2)[key],
                                                                key);

            distances->insert(distances->end(),distances_in_path->begin(), distances_in_path->end());
        }


        // Increment the Iterator to point to next entry
        iterator++;

    }
    return distances;
}


std::map<size_t,std::vector<size_t>*>* PathsPrefixSumArrays::get_all_node_positions(gbwt::node_type node){
    auto node_visits = fast_locate->decompressSA(node);

    if(node == 0 || node_visits.empty()){
        throw NodeNotInPathsException();
    }

    std::map<size_t,std::vector<size_t>*> *distances_in_paths = new std::map<size_t,std::vector<size_t>*>();

    for (int i = 0; i < node_visits.size() ; ++i) {
        gbwt::size_type path_id =fast_locate->seqId(node_visits[i]); // Sequence id

        // Todo: we could optimize this operation by memorizing the ones, try to find out if it is a good way.
        size_t ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id]))(((*psa)[path_id])->size());

        if((*distances_in_paths)[path_id] == nullptr){
            (*distances_in_paths)[path_id] = new std::vector<size_t>();
        }

        ((*distances_in_paths)[path_id])->push_back(ones - fast_locate->seqOffset(node_visits[i]) - 1);

    }
    return distances_in_paths;
}


std::vector<size_t>* PathsPrefixSumArrays::get_all_nodes_distances_in_path(std::vector<size_t>* node_1_positions,
                                                                           std::vector<size_t>* node_2_positions,
                                                                           size_t path_id){
    std::vector<size_t>* distances = new std::vector<size_t>();

    if(node_1_positions->empty() || node_2_positions->empty()) {
        return distances;
    }

    // Sort the position nodes in each vector
    std::sort(node_1_positions->begin(), node_1_positions->end());
    std::sort(node_2_positions->begin(), node_2_positions->end());

    int pivot_1 = 0, pivot_2 = 0;
    int i, j, end;

    bool exit=false;
    bool iterate_on_node_2_positions;

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

        if(node_1_positions->at(pivot_1) == node_2_positions->at(pivot_2)){ // because if they are equal you don't have to compute two times the distances
            if(pivot_2==node_2_positions->size()-1)
               exit=true;

                ++ pivot_2;

        }

        if(!exit&&node_1_positions->at(pivot_1) < node_2_positions->at(pivot_2)){
            j = pivot_2;
            end = node_2_positions->size();
            i = pivot_1;
            ++ pivot_1;

            iterate_on_node_2_positions = true;
        }else if(!exit){

            j = pivot_1;
            end = node_1_positions->size();
            i = pivot_2;
            ++ pivot_2;

            iterate_on_node_2_positions = false;
        }


        while( !exit && j < end ){
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
}

