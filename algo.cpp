#include <stdio.h>
#include <string.h>
#include "diff_func.h"
#include "graph.h"
#include <assert.h>

using namespace std;

extern vector<struct el>level_table;
extern char OUTPUT_FILE[30];
extern size_t sample_size, period_T;
extern double budget;

double get_X_cost(struct X x_t){
    if (x_t.lv == 0){
        return 1;
    }
    else{
        return level_table[x_t.lv].phi_cost * x_t.cost;
    }
}

double get_group_cost(vector<vector<struct X> >& group){
    double cost = 0;
    for(size_t t=0;t<group.size();t++){
        for(size_t x=0;x<group[t].size();x++){
            cost += get_X_cost(group[t][x]);
        }
    }
    return cost;
}


bool has_candidate_A(double* diff_baseline_table, int U_LENGTH){
    bool stop = true;
    for(int i=0;i<U_LENGTH;i++){
        stop &= (diff_baseline_table[i] < 0);
    }
    return !stop;
}

double get_group_cost(vector<struct X>& group){
    double cost = 0;
    for(size_t t=0;t<group.size();t++){
        cost += get_X_cost(group[t]);
    }
    return cost;
}

void init_strategy(vector<vector<struct X> >& s){
    for(size_t i=0;i<period_T;i++){
        vector<struct X> x_t;
        s.push_back(x_t);
    }
}

void calc_greedy(vector<vector<struct X> >& S, Graph& g, bool* X_in_set_S[], double* prev_greedy_S, bool* has_better_group){
    int one_dim_idx=0, min_one_dim_idx=0;
    double min_greedy_value = INT32_MAX;
    struct X min_X;
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if((*X_in_set_S)[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            vector<vector<struct X> > tmp = S;
            struct X u_X = g.U[i][j];

            tmp[u_X.t].push_back(u_X);
            double tmp_value = diffusion_greedy(tmp, g);
            if( tmp_value < min_greedy_value){
                min_greedy_value = tmp_value;
                min_X = u_X;
                min_one_dim_idx = one_dim_idx;
            }
            one_dim_idx++;
        }
    }
    if(min_greedy_value > *prev_greedy_S){
        *has_better_group = false;
        return;
    }

    S[min_X.t].push_back(min_X);
    (*X_in_set_S)[min_one_dim_idx] = true;
    *prev_greedy_S = min_greedy_value;


    FILE * pFile;
    pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf (pFile, "\n%-15s :%f\n%-15s :%d_%d\n%-15s :","greedy ", min_greedy_value, "X ",  min_X.t, min_X.id, "S ");
    int tmp_idx = 0;
    for(size_t i=0;i<S.size();i++){
        for(size_t j=0;j<S[i].size();j++){
            fprintf (pFile, "%d_%d ", S[i][j].t, S[i][j].id);
            tmp_idx++;
        }
    }
    fprintf(pFile, "\n");
    fclose (pFile);

    int tmp_one_dim_index = 0;
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if(!(*X_in_set_S)[tmp_one_dim_index] && get_X_cost(g.U[i][j]) > budget - get_group_cost(S)){
                (*X_in_set_S)[tmp_one_dim_index] = out_of_budget;
            }
            tmp_one_dim_index++;
        }
    }
}

void init_positive_group(vector<vector<struct node> >& p){
    for(size_t i=0;i<3;i++){
        vector<struct node> tmp;
        p.push_back(tmp);
    }
}
void one_to_two_dim(vector<struct X> & A, vector<vector<struct X>> & A_two_dim){
    init_strategy(A_two_dim);
    for(size_t i=0;i<A.size();i++){
        struct X tmp = A[i];
        A_two_dim[tmp.t].push_back(tmp);
    }
}

void calc_main_A(Graph& g, vector<struct X>& A, double prev_best_A, double cost_A, double* diff_main_table[], bool X_in_set_A[]){
    int one_dim_idx=0;
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if(X_in_set_A[one_dim_idx]){
                (*diff_main_table)[one_dim_idx] = has_in_set;
                one_dim_idx++;
                continue;
            }

            struct X u_X = g.U[i][j];
            vector<vector<struct X> > tmpA;
            one_to_two_dim(A, tmpA);
            if(cost_A + get_X_cost(u_X) > budget){
                (*diff_main_table)[one_dim_idx] = out_of_cost;
                one_dim_idx++;
                continue;
            }
            tmpA[u_X.t].push_back(u_X);
            (*diff_main_table)[one_dim_idx] = diffusion(tmpA, g) - prev_best_A;
            one_dim_idx++;
        }
    }
}

double IR(){
    return 0.5;
}

void PSPD_main_A(Graph& g, vector<struct X>& A, double* diff_baseline_table[], bool* X_in_set_A[], double* prev_best_A){
    struct X best_X;
    int one_dim_idx_A=0;
    double max_value = -1;
    int max_one_dim_idx = -1;

    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            double baseline_value = (*diff_baseline_table)[one_dim_idx_A];
            struct X u_X = g.U[i][j];
            if(baseline_value < 0){
                one_dim_idx_A++;
                continue;
            }

            //double denominator = (level_table[u_X.lv].phi_cost * u_X.cost);
            double denominator = get_X_cost(u_X);
            assert(denominator != 0);
            if((baseline_value / denominator) > max_value){
                best_X = u_X;
                max_one_dim_idx = one_dim_idx_A;
                max_value = (baseline_value / denominator);
            }
            one_dim_idx_A++;
        }
    }
    if(max_one_dim_idx == -1)return;
    A.push_back(best_X);

    FILE * pFile;
    pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf (pFile, "\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%d_%d_%d\n%-15s :","main: ",  max_value, "F(A U {X}, T)", ((*diff_baseline_table)[max_one_dim_idx] + *prev_best_A), "F(A, T)", *prev_best_A, "/ phi * C(D)", (level_table[best_X.lv].phi_cost * best_X.cost), "X_t_id_OneDim", best_X.t, best_X.id, max_one_dim_idx, "A Strategies");
    int tmp_idx = 0;

    for(size_t i=0;i<A.size();i++){
        fprintf (pFile, "%d_%d ", A[i].t, A[i].id);
        tmp_idx++;
    }
    fprintf(pFile, "\n");
    fclose(pFile);

    (*X_in_set_A)[max_one_dim_idx] = true;
    *prev_best_A = *prev_best_A + (*diff_baseline_table)[max_one_dim_idx];
}
