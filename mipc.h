#ifndef MIPC_H
#define MIPC_H
#include "graph.h"

struct Path { 
    double path_prob;

    /**
     * If we use bool* to defien visited, the pointer must be "malloc"
     * when copy from another Path struct object.
     */ 
    vector<bool> visited;
    int neighbor;
    Stage neighbor_stage;
}; 

void algo_mipc(Graph& g, int target_v, double theta);

#endif