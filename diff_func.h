#ifndef FUNC_H
#define FUNC_H
#define a_v 0.4

#include<vector>
#include<iostream>
#include "data.h"
#include "graph.h"

using namespace std;
double diffusion(vector<vector<struct X> > Strategy, int , Graph& g);
double diffusion_greedy(vector<vector<struct X> > Strategy, int sam_size, Graph& g);
size_t get_positive_count(vector<vector<struct node*>*> positive_group);
double get_contagion(int);
void migrate(vector<node*>*, vector<node*>*, struct node*);
void printVec(vector<struct node>);
void print_group(vector<vector<struct node*>*> v);
void self_transmission_process(vector<vector<struct node*>*> from, vector<vector<struct node*>*> to, struct node* v);
void tmp_push_back(vector<vector<struct node*>*>, vector<vector<struct node*>*>);
void addEdge(vector<struct edge>*, int , int, double) ;
void infection_process(Graph& g, vector<struct node*>& from, vector<struct node*>& to, struct node* v, vector<struct X> X_t);
double objective_at_t(vector<vector<struct node*>*> health_group, vector<struct X> X_t, int v, vector<struct node*>& N);
#endif 