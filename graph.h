#ifndef GRAPH_H
#define GRAPH_H
#include "data.h"

using namespace std;

class Graph{
    public:
        Graph(int V_, int E_);
        void addEdge(int u, int v, double p);
        double get_contagion(int type);
        void print_node();
        double get_edge_prob(struct node* u, struct node* v);

        int V=3, E=2;
        vector<struct node*> N;
        vector<vector<struct edge> > adj;
        vector<vector<struct X> > U;
        void free_N(); // desconstructor
};
#endif
