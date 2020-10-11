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
void algo_mipc(Graph& g, int target_v, vector<vector<Path>>& infection_path);
double h_prob(vector<vector<Path>> infection_path, size_t h_t, vector<vector<X> > S, Graph& g, int TARGET_V);
double CR(struct X C_k_x, vector<vector<struct X>>B_list, Graph& g);
double IR(struct X x, vector<vector<struct X> > Strategy, Graph& g);

#endif