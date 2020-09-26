#ifndef GRAPH_H
#define GRAPH_H
#include "data.h"

using namespace std;

class Graph{
    public:
        Graph();
        ~Graph();
        void addEdge(int u, int v, double p);
        double get_contagion(int type);
        void print_node();
        double get_edge_prob(struct node* u, struct node* v);
        void push_U(struct X x);
        void set_node_lv(vector<struct X> x_t);
        void init_graph(int V_, int E_, int U_LENGTH_);
        size_t V, E, U_LENGTH;
        vector<struct node*> N;
        vector<vector<struct edge> > adj;
        vector<vector<struct X> > U;
};
#endif
