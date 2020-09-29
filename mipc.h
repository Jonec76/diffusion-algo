#ifndef MIPC_H
#define MIPC_H
#include "graph.h"

struct Path { 
    // int curr_node;
    double path_prob;
    bool* visited;
    int neighbor;
    Stage neighbor_stage;
}; 

void algo_mipc(Graph& g);

#endif