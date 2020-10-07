#ifndef ALGO_H
#define ALGO_H

#include<vector>
#include<iostream>
#include "graph.h"

vector<vector<struct X> > algo_main(Graph& g);
vector<vector<struct X> > algo_greedy(Graph& g);
double get_X_cost(struct X x_t);
void init_strategy(vector<vector<struct X> >& s);
double get_group_cost(vector<vector<struct X> >& group);
void calc_main(Graph& g, vector<vector<struct X> >& A, double prev_best_A, double cost_A, double* diff_baseline_table[], bool X_in_set_A[]);
void calc_greedy(Graph& g, vector<vector<struct X> >& A, double prev_best_A, double cost_A, double* diff_baseline_table[], bool X_in_set_A[]);
void PSPD_main(Graph& g, vector<vector<struct X> >& A, double* diff_baseline_table[], bool* X_in_set_A[], double* prev_best_A);
void PSPD_greedy(Graph& g, vector<vector<struct X> >& A, double* diff_baseline_table[], bool* X_in_set_A[], double* prev_best_A);
bool has_candidate(double* diff_baseline_table, int U_LENGTH);
#endif