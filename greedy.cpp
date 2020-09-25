#include <stdio.h>
#include <string.h>
#include "diff_func.h"
#include "greedy.h"
#include "graph.h"
using namespace std;

vector<vector<struct X> > greedy_algo(Graph& g){
    vector<vector<struct X>> A, B, S;
    init_strategy(A);
    init_strategy(B);

    int i=0, j=-1;
    double prev_best_A=0;
    double cost_A = get_group_cost(A);
    double cost_B = get_group_cost(B);
    bool* X_in_set_A = (bool*) malloc(U_LENGTH*sizeof(bool));
    memset(X_in_set_A, false, U_LENGTH * sizeof(bool)); // for clearing previous record
    double* diff_baseline_table = (double*) malloc(U_LENGTH*sizeof(double));
    memset(diff_baseline_table, 0, U_LENGTH * sizeof(double)); // for clearing previous record

    while((cost_A < budget) && has_candidate(diff_baseline_table)){
        cout<<"======"<<endl;
        vector<struct X> C;
        calc_baseline(g, A, prev_best_A, cost_A, &diff_baseline_table, X_in_set_A, sample_size);
        PSPD(g, A, &diff_baseline_table, &X_in_set_A, &prev_best_A);
        cost_A = get_group_cost(A);
    }
    return S;
}