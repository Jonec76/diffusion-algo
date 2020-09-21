#ifndef FUNC_H
#define FUNC_H

#include<vector>
#include<iostream>
#include "data.h"

using namespace std;
double get_contagion(int);
void migrate(vector<node*>*, vector<node*>*, struct node*);
void printVec(vector<struct node>);
void print_group(vector<vector<struct node*>*> v);
void self_transmission_process(vector<vector<struct node*>*>, vector<vector<struct node*>*>, struct node*);
void tmp_push_back(vector<vector<struct node*>*>, vector<vector<struct node*>*>);
void addEdge(vector<struct edge>*, int , int, double) ;
void infection_process(vector<struct edge>* e, vector<node*> N, vector<struct node*>* from, vector<struct node*>* to, struct node* v, vector<struct X> x);
double objective_at_t(vector<vector<struct node*>*> health_group, vector<struct X> X_t);
double diffusion(vector<vector<struct X>> Strategy, int q_period_T, int sample_size);
#endif 