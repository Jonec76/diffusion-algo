#ifndef FUNC_H
#define FUNC_H

#include<vector>
#include<iostream>
#include "data.h"
#include "graph.h"

using namespace std;
void diffusion(vector<vector<struct X>> Strategy, Graph g, double* f, size_t num_threads);
void *parallel(double* value, size_t num_threads, vector<vector<struct X>> &Strategy, Graph g);

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