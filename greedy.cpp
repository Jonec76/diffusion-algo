#include <stdio.h>
#include <string.h>
#include "diff_func.h"
using namespace std;

double get_X_cost(struct X group){
    return 0;
}

double get_group_cost(vector<vector<struct X>>& group){
    return 0;
}

void calc_baseline(Graph& g, vector<vector<struct X>>& A, double diff_A_value, double cost_A, double* baseline_table[U_LENGTH], bool X_in_set_A[U_LENGTH], int T, int sample_size){
    int one_dim_idx=0;
    for(int i=0;i<g.U.size();i++){ // each X_t in U;
        for(int j=0;j<g.U[i].size();j++){ // each X in X_t
            if(X_in_set_A[one_dim_idx])
                continue;
            struct X u_X = g.U[i][j];
            vector<vector<struct X>> tmpA = A;
            tmpA[u_X.t].push_back(u_X);

            if(cost_A + get_X_cost(u_X) > budget){
                (*baseline_table)[one_dim_idx] = out_of_cost;
                one_dim_idx++;
                continue;
            }

            (*baseline_table)[one_dim_idx] = diffusion(tmpA, T, sample_size, g) - diff_A_value;
            one_dim_idx++;
        }
    }

}

vector<vector<struct X>> greedy_algo(Graph& g){
    vector<vector<struct X>> A, B;
    // init A, B

    int i=0, j=-1, T, sample_size;
    Graph g = Graph(5, 4);

    double cost_A = get_group_cost(A);
    double cost_b = get_group_cost(B);
    bool X_in_set_A[U_LENGTH];
    memset(X_in_set_A, false, U_LENGTH * sizeof(bool)); // for clearing previous record
    double* baseline_table = (double*)malloc(U_LENGTH * sizeof(double));
    double diff_A_value = 0;

    while(cost_A < budget || cost_b < budget){
        vector<struct X> C;
        calc_baseline(g, A, diff_A_value, cost_A, &baseline_table, X_in_set_A, T, sample_size);
        // PSPD
    }
}