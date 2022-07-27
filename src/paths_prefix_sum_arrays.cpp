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
    // TO BE DELETED AFTER REFACTORINGS, START
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
    // TO BE DELETED AFTER REFACTORINGS, END

    // Create prefix sum array new data structure
    //std::vector<std::shared_ptr<sdsl::sd_vector<>>> prefix_sum_arrays;
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

        prefix_sum_arrays.push_back(std::shared_ptr<sdsl::sd_vector<>>{new sdsl::sd_vector<>(psa_temp)});
    }

    // Create the fast locate
    fast_locate = new gbwt::FastLocate(*gbwtGraph.index);
}


const gbwt::FastLocate* PathsPrefixSumArrays::get_fast_locate() const {
    return fast_locate;
}

const  std::map<gbwt::size_type, std::shared_ptr<const sdsl::sd_vector<>>>* PathsPrefixSumArrays::get_prefix_sum_arrays_map() const {
    std::map<gbwt::size_type, std::shared_ptr<sdsl::sd_vector<>>>* map = new std::map<gbwt::size_type, std::shared_ptr<sdsl::sd_vector<>>>();

    if (!prefix_sum_arrays.empty()) {
        for (int i = 0; i < prefix_sum_arrays.size(); ++i) {
            (*map)[i * 2] = prefix_sum_arrays[i];
        }
    }else
        return nullptr;

    return reinterpret_cast<const std::map<gbwt::size_type, std::shared_ptr<const sdsl::sd_vector<>>> *>(map);
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path(size_t pos_node_1, size_t pos_node_2, size_t path_id) const { // fully tested
    size_t distance = 0;
    size_t ones;
    sdsl::sd_vector<>::select_1_type sdb_sel;

    bool reversed_path = false;

    // Distance between the same node
    if (pos_node_1 == pos_node_2){
        std::string msg_exception = "Error in 'get_distance_between_positions_in_path': the distance between two equal positions (" +
                std::to_string(pos_node_1) + ", " + std::to_string(pos_node_2) + ") doesn't exist.\n";
        throw NotExistentDistanceException(msg_exception);
    }



    if((path_id % 2 == 0 && get_prefix_sum_array_of_path(path_id) == nullptr )|| (path_id % 2 != 0 && get_prefix_sum_array_of_path(path_id-1) == nullptr )){
        std::string error = "Error in 'get_distance_between_positions_in_path': the path_id " + std::to_string(path_id)
                            + " doesn't exist in the graph.\n";

        throw pathsprefixsumarrays::PathNotInGraphException(error);
    }


    // If the path is reversed (odd) we don't have it memorized, so we use the even one
    if (path_id % 2 != 0 && get_prefix_sum_array_of_path(path_id-1) != nullptr) {
        reversed_path = true;
        --path_id;

        // Compute ones == compute number of nodes in the path
        ones = sdsl::sd_vector<>::rank_1_type(&(*get_prefix_sum_array_of_path(path_id)))((get_prefix_sum_array_of_path(path_id))->size());

        //TODO: try to find a way to avoid duplicated code
        //TODO: try to memorize this for every path and see if it's faster or not and how much memory it consumes with big graphs
        if(pos_node_1 < ones && pos_node_2 < ones){
            pos_node_1 = ones - pos_node_1 - 1;
            pos_node_2 = ones - pos_node_2 - 1;
        }
    }

    // Compute ones == compute number of nodes in the path
    if(!reversed_path)
        ones = sdsl::sd_vector<>::rank_1_type(&(*get_prefix_sum_array_of_path(path_id)))((get_prefix_sum_array_of_path(path_id))->size());

    // Initialize select operation (see select/rank)
    //todo capire bene
    sdb_sel = sdsl::sd_vector<>::select_1_type( get_prefix_sum_array_of_path(path_id).get());


    if (pos_node_2 >= ones) {
        std::string error =
                "Error in 'get_distance_between_positions_in_path': the second position " + std::to_string(pos_node_2) +
                " is outside the boundaries of the path [0:" + std::to_string(ones - 1) + "]" + "\n";

        throw pathsprefixsumarrays::OutOfBoundsPositionInPathException(error);

    } else if (pos_node_1 >= ones) {
        std::string error =
                "Error in 'get_distance_between_positions_in_path': the first position " + std::to_string(pos_node_1) +
                " is outside the boundaries of the path [0:" + std::to_string(ones - 1) + "]" + "\n";

        throw pathsprefixsumarrays::OutOfBoundsPositionInPathException(error);

    } else {
        distance = get_distance_between_positions_in_path_aux(pos_node_1, pos_node_2, sdb_sel);
    }

    return distance;
}


size_t PathsPrefixSumArrays::get_distance_between_positions_in_path_aux(size_t pos_node_1, size_t pos_node_2, sdsl::sd_vector<>::select_1_type &sdb_sel) const {
    if(pos_node_1 > pos_node_2){
        std::swap(pos_node_1, pos_node_2);
    }

    size_t node_before_bigger_node_offset = sdb_sel(pos_node_2);
    size_t node_1_offset = sdb_sel(pos_node_1 + 1);

    return node_before_bigger_node_offset - node_1_offset;
}


void PathsPrefixSumArrays::clear() {
    // Deleting memory fast_locate
    if(fast_locate != nullptr){
        delete fast_locate;
        fast_locate = nullptr;
    }


    // TO BE DELETED AFTER REFACTORINGS, START
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
    // TO BE DELETED AFTER REFACTORINGS, END

    // Delete prefix sum array memory
    prefix_sum_arrays.clear();
}


PathsPrefixSumArrays::~PathsPrefixSumArrays() {
    clear();
}


std::string PathsPrefixSumArrays::toString_sd_vectors() const {
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


std::string PathsPrefixSumArrays::toString() const {
    std::string temp="";

    auto iterator = (*psa).begin();
    for(int half_path_id = 0; half_path_id < prefix_sum_arrays.size(); ++half_path_id){
        temp +=  "\n| ";
        temp += std::to_string(half_path_id * 2) + "::\t[";

        // Accessing VALUE from element pointed by it.
        for(int index_prefix_sum = 0; index_prefix_sum < prefix_sum_arrays[half_path_id]->size(); ++index_prefix_sum){
            if((*(prefix_sum_arrays[half_path_id]))[index_prefix_sum] == 1) {
                temp += std::to_string(index_prefix_sum);
                if (index_prefix_sum < prefix_sum_arrays[half_path_id]->size() - 1)
                    temp += ", ";
            }
        }
        temp += "]";
    }

    return temp;
}


std::unique_ptr<std::vector<size_t>> PathsPrefixSumArrays::get_all_nodes_distances_in_path( gbwt::node_type node_1,
                                                                            gbwt::node_type node_2,
                                                                            size_t path_id) const {


    // If the path doesn't exist
    if ((path_id%2 == 0 && psa->find(path_id) == psa->end()) ||(path_id%2 != 0 && psa->find(path_id-1) == psa->end())) {
        std::string error = "Error in 'get_all_nodes_distances_in_path': the inserted path " + std::to_string(path_id) +
                " doesn't exist inside the graph.";
        throw PathNotInGraphException(error);
    }

    size_t ones;
    if (path_id%2==0) {
         // Ones: the number of ones in the sd_vector correspond to the number of nodes inside a path
         ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id]))(((*psa)[path_id])->size());
    }else
        ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id-1]))(((*psa)[path_id-1])->size());

    // Get nodes positions within a path, a node in a loop can occurr several times
    std::unique_ptr<std::vector<size_t>> node_1_positions = get_positions_of_a_node_in_path(path_id, node_1, ones);

    std::unique_ptr<std::vector<size_t>> node_2_positions = get_positions_of_a_node_in_path(path_id, node_2, ones);

    return get_all_nodes_distances_in_path(std::move(node_1_positions),
                                           std::move(node_2_positions),
                                           path_id);
}


std::unique_ptr<std::vector<size_t>> PathsPrefixSumArrays::get_positions_of_a_node_in_path(size_t path_id, gbwt::node_type node, size_t &ones) const {
    auto node_visits = fast_locate->decompressSA(node);
    bool reverse  = false;
    std::unique_ptr<std::vector<size_t>> node_positions(new std::vector<size_t>());

    // If the node is 0 is not in the path || no node visits || path_id doesn't exist in the psa
    // todo node_visits.empty()  should not be needed
    if(node == 0 || node_visits.empty() || (path_id%2==0 && get_prefix_sum_array_of_path(path_id) == nullptr )|| ( path_id%2!= 0 && get_prefix_sum_array_of_path(path_id-1) == nullptr )){
        return node_positions;
    }

    for (int i = 0; i < node_visits.size() ; ++i) {
        if(fast_locate->seqId(node_visits[i]) == path_id){
            node_positions->push_back(ones - fast_locate->seqOffset(node_visits[i]) - 1);
        }
    }

    return node_positions;
}


// TODO: it could be helpful know at which path every distances belongs to
/*
 * put positions in shared ptr and pass them to the function with std::move and delete the code of deleting memory
 */
std::vector<size_t>* PathsPrefixSumArrays::get_all_nodes_distances(gbwt::node_type node_1, gbwt::node_type node_2) const {
    std::vector<size_t> *distances = new std::vector<size_t>();

    //std::map<size_t, std::vector<size_t> *> *positions_node_1 = get_all_node_positions(node_1);
    //std::map<size_t, std::vector<size_t> *> *positions_node_2 = get_all_node_positions(node_2);

    std::unique_ptr<std::map<size_t,std::shared_ptr<std::vector<size_t>>>> positions_node_1 = get_all_node_positions(node_1);
    std::unique_ptr<std::map<size_t,std::shared_ptr<std::vector<size_t>>>> positions_node_2 = get_all_node_positions(node_2);


    if (positions_node_1->empty() || positions_node_2->empty()) {
        return distances;
    }

    auto iterator = (*positions_node_1).begin();

    // Iterate over the map using Iterator till end.
    while (iterator != (*positions_node_1).end())
    {
        size_t key = iterator->first; // sequence id
        std::unique_ptr<std::vector<size_t>> distances_in_path;

        if((*positions_node_2).find(key)  != (*positions_node_2).end()) {
            //se il path_ley è dispari modifico le posizioni togliendo ogni posizione da (ones-1) e abbasso di 1 la key
            distances_in_path = get_all_nodes_distances_in_path((*positions_node_1)[key],
                                                                (*positions_node_2)[key],
                                                                key);

            distances->insert(distances->end(),distances_in_path->begin(), distances_in_path->end());
        }


        // Increment the Iterator to point to next entry
        iterator++;

    }



    // Deleting memory positions_node_1 and positions_node_2
    positions_node_1->erase(positions_node_1->begin(), positions_node_1->end());
    positions_node_2->erase(positions_node_2->begin(), positions_node_2->end());

    /*
     * TODO: help I don't understand why this is not working
     * auto it = positions_node_1->begin();
      for (it = positions_node_1->begin(); it != positions_node_1->end(); it++){
        std::cerr << "Size vettore: " <<  it->second->size() << "\n";
        (*positions_node_1).erase(it);
        //positions_node_1->erase(it);
    }*/

    return distances;
}


std::unique_ptr<std::map<size_t,std::shared_ptr<std::vector<size_t>>>> PathsPrefixSumArrays::get_all_node_positions(gbwt::node_type node) const {
    auto node_visits = fast_locate->decompressSA(node);

    std::unique_ptr<std::map<size_t,std::shared_ptr<std::vector<size_t>>>> node_positions( new std::map<size_t,std::shared_ptr<std::vector<size_t>>>());

    if(node == 0 || node_visits.empty()){
        return node_positions;
    }

    for (int i = 0; i < node_visits.size() ; ++i) {
        gbwt::size_type path_id =fast_locate->seqId(node_visits[i]); // Sequence id

        // Todo: we could optimize this operation by memorizing the ones, try to find out if it is a good way.
        size_t ones;
        if(path_id%2==0){
             ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id]))(((*psa)[path_id])->size());
        } else{
             ones = sdsl::sd_vector<>::rank_1_type(&(*(*psa)[path_id-1]))(((*psa)[path_id-1])->size());
        }
        //initialize che map if it's not been initialized before;
        if((*node_positions)[path_id] == nullptr){
            (*node_positions)[path_id].reset(new std::vector<size_t>());
        }
        ((*node_positions)[path_id])->push_back(ones - fast_locate->seqOffset(node_visits[i]) - 1);

    }
    
    return node_positions;
}


/*
 * put smart pointers shared_ptr
 */

std::unique_ptr<std::vector<size_t>> PathsPrefixSumArrays::get_all_nodes_distances_in_path(std::shared_ptr<std::vector<size_t>> node_1_positions,
                                                                           std::shared_ptr<std::vector<size_t>> node_2_positions,
                                                                           size_t path_id) const{
    std::unique_ptr<std::vector<size_t>> distances(new std::vector<size_t>());

    if(node_1_positions->empty() || node_2_positions->empty() || (path_id%2==0 &&get_prefix_sum_array_of_path(path_id)== nullptr) || (path_id%2!=0 && get_prefix_sum_array_of_path(path_id-1)== nullptr )) {
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
            if(pivot_2 == node_2_positions->size() - 1) {
                exit = true;
                size_t ones;
                if (path_id % 2 == 0) {
                    ones = sdsl::sd_vector<>::rank_1_type(&(*get_prefix_sum_array_of_path(path_id)))((get_prefix_sum_array_of_path(path_id))->size());
                } else
                    ones = sdsl::sd_vector<>::rank_1_type(&(*get_prefix_sum_array_of_path(path_id-1)))((get_prefix_sum_array_of_path(path_id-1))->size());
                if (node_1_positions->at(pivot_1) >= ones || node_2_positions->at(pivot_2) >= ones) {
                    std::string error =
                            "Error in 'get_all_nodes_distances_in_path': position " +
                            std::to_string(node_2_positions->at(pivot_2)) +
                            " is outside the boundaries of the path [0:" + std::to_string(ones - 1) + "]" + "\n";

                    throw pathsprefixsumarrays::OutOfBoundsPositionInPathException(error);
                }
            }
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


        while(!exit && j < end){

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
    }while(pivot_1 < node_1_positions->size() && pivot_2 < node_2_positions->size() && !exit);

    return distances;
}


std::shared_ptr<const sdsl::sd_vector<>> PathsPrefixSumArrays::get_prefix_sum_array_of_path(size_t path_id) const {
    if(path_id%2 != 0 || path_id/2 >= prefix_sum_arrays.size())
        return nullptr;
    return prefix_sum_arrays[path_id/2];
}






