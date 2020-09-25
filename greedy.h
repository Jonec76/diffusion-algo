#ifndef GREEDY_H
#define GREEDY_H

vector<vector<struct X>> greedy_algo(Graph& g);
double get_X_cost(struct X x_t);
void init_strategy(vector<vector<struct X>>& s);
double get_group_cost(vector<vector<struct X>>& group);
void calc_baseline(Graph& g, vector<vector<struct X>>& A, double prev_best_A, double cost_A, double* diff_baseline_table[U_LENGTH], bool X_in_set_A[U_LENGTH], int sam_size);
void PSPD(Graph& g, vector<vector<struct X>>& A, double* diff_baseline_table[U_LENGTH], bool* X_in_set_A[U_LENGTH], double* prev_best_A);
bool has_candidate(double* diff_baseline_table);

#endif