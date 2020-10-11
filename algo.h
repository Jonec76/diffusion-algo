#ifndef ALGO_H
#define ALGO_H

#include<vector>
#include<iostream>
#include "graph.h"

void algo_main(Graph& g);
void print_list(vector<struct X> list);
void fprint_list(FILE** fp, vector<struct X> list);
vector<vector<struct X> > algo_greedy(Graph& g);
double get_X_cost(struct X x_t);
void init_strategy(vector<vector<struct X> >& s);
void init_positive_group(vector<vector<struct node> >& s);
void calc_greedy(vector<vector<struct X> >& S, Graph& g, bool* X_in_set_S[], double* prev_greedy_S, bool* has_better_group);
double get_group_cost(vector<vector<struct X> >& group);
double get_group_cost(vector<struct X>& group);
void calc_main_A(Graph& g, vector<struct X>& A, double prev_best_A, double cost_A, double* diff_baseline_table[], bool X_in_set_A[]);
bool has_candidate_A(double* diff_baseline_table, int U_LENGTH);
vector<vector<struct X>> one_to_two_dim(vector<struct X> & A);
void PSPD_update_A(Graph& g, vector<struct X>& A, double* diff_baseline_table[], bool* X_in_set_A[], double* prev_best_A);
void PSPD_get_C_i(vector<struct X>& set_C, Graph& g, vector<struct X>& B, vector<struct X>& A, bool* X_in_set_B[], int i_day);
void cost_update_C(vector<struct X>& set_C, Graph& g, vector<struct X>& B, bool* X_in_set_B[], int i_day);
void get_max_idx_from_C(double* max_B_F, int* max_X_idx, vector<struct X>& B, vector<struct X>& set_C, Graph& g);
vector<struct X> get_sublist(vector<struct X>& list, size_t i_day);
void migrate_strategy(vector<struct X>& B, vector<struct X>& C_per_t, int max_X_idx_in_C, bool* X_in_set_B[]);
bool is_out_of_cost(vector<struct X>& B, vector<vector<struct X>>& U, bool* X_in_set_B[]);
void get_X_max_F(double* X_max_F, vector<struct X>& X_max, Graph& g);
void get_argmax_strategy(vector<vector<struct X>> &S, vector<struct X>&A, vector<struct X>&B, Graph& g);
vector<struct X> get_candidate_i(vector<vector<struct X>> list, size_t i_day);
size_t RCR(vector<struct X>& A, vector<struct X>& B, vector<vector<struct X>>& C, int j_day, int i_day, Graph& g, bool* X_in_set_B[]);
#endif