#include <stdio.h>
#include <string.h>
#include "diff_func.h"
#include "graph.h"
using namespace std;

// extern struct el el0 ={0, 0}, el1 ={0.3, 0.3}, el2 ={0.7, 0.7}, el3 ={1, 1};
extern vector<struct el>level_table;

double get_X_cost(struct X x_t){
    return level_table[x_t.lv].phi_cost * x_t.cost;
}

double get_group_cost(vector<vector<struct X>>& group){
    double cost = 0;
    for(size_t t=0;t<group.size();t++){
        for(size_t x=0;x<group[t].size();x++){
            cost += get_X_cost(group[t][x]);
        }
    }
    return cost;
}


void init_strategy(vector<vector<struct X>>& s){
    for(int i=0;i<period_T;i++){
        vector<struct X> x_t;
        s.push_back(x_t);
    }
}

void calc_baseline(Graph& g, vector<vector<struct X>>& A, double diff_A_value, double cost_A, double* baseline_table[U_LENGTH], bool X_in_set_A[U_LENGTH], int sam_size){
    int one_dim_idx=0;
    for(int i=0;i<g.U.size();i++){ // each X_t in U;
        for(int j=0;j<g.U[i].size();j++){ // each X in X_t
            if(X_in_set_A[one_dim_idx]){
                (*baseline_table)[one_dim_idx] = has_in_set;
                one_dim_idx++;
                continue;
            }

            struct X u_X = g.U[i][j];
            vector<vector<struct X>> tmpA = A;
            if(cost_A + get_X_cost(u_X) > budget){
                (*baseline_table)[one_dim_idx] = out_of_cost;
                one_dim_idx++;
                continue;
            }
            tmpA[u_X.t].push_back(u_X);
            (*baseline_table)[one_dim_idx] = diffusion(tmpA, sample_size, g) - diff_A_value;
            one_dim_idx++;
        }
    }
}

void PSPD(Graph& g, vector<vector<struct X>>& A, double* baseline_table[U_LENGTH]){

}

vector<vector<struct X> > greedy_algo(Graph& g){
    vector<vector<struct X>> A, B, S;

    init_strategy(A);
    init_strategy(B);

    int i=0, j=-1;

    double cost_A = get_group_cost(A);
    double cost_b = get_group_cost(B);
    bool X_in_set_A[U_LENGTH];
    memset(X_in_set_A, false, U_LENGTH * sizeof(bool)); // for clearing previous record
    double* baseline_table = (double*) malloc(U_LENGTH*sizeof(double));
    memset(baseline_table, 0, U_LENGTH * sizeof(double)); // for clearing previous record
    double diff_A_value = 0;

    while(cost_A < budget || cost_b < budget){
        vector<struct X> C;
        calc_baseline(g, A, diff_A_value, cost_A, &baseline_table, X_in_set_A, sample_size);
        // PSPD(g, A, &baseline_table);
    }
    return S;
}