#include <stdio.h>
#include <string.h>
#include "diff_func.h"
#include "graph.h"
#include <assert.h>
#include <math.h>
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

vector<struct X> get_sublist(vector<struct X>& list, size_t i_day){
    vector<struct X> sublist;
    for(size_t i=0;i<i_day;i++)
        sublist.push_back(list[i]);
    return sublist;
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
vector<vector<struct X>> one_to_two_dim(vector<struct X> & A){
    vector<vector<struct X>> A_two_dim;
    init_strategy(A_two_dim);
    for(size_t i=0;i<A.size();i++){
        struct X tmp = A[i];
        A_two_dim[tmp.t].push_back(tmp);
    }
    return A_two_dim;
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
            vector<vector<struct X> > tmpA = one_to_two_dim(A);
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

double CR_diff(){
    return 0.3;
}

void PSPD_update_A(Graph& g, vector<struct X>& A, double* diff_baseline_table[], bool* X_in_set_A[], double* prev_best_A){
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

void PSPD_update_C(vector<struct X>& set_C, Graph& g, vector<struct X>& B, vector<struct X>& A, bool* X_in_set_B[], int i_day){
    int one_dim_idx = 0;
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if(X_in_set_B[one_dim_idx]){
                one_dim_idx++;
                continue;
            }

            struct X u_X = g.U[i][j];
            vector<struct X> tmpB = B;
            vector<struct X> subA = get_sublist(A, i_day+1);
            tmpB.push_back(u_X);
            bool better_IR = IR() >= (1 - delta_i) * IR();
            bool availabel_cost = get_group_cost(tmpB) <= min((1 + delta_c)* get_group_cost(subA), budget);
            if(better_IR && availabel_cost){
                set_C.push_back(u_X);
            }
        }
    }
}

void cost_update_C(vector<struct X>& set_C, Graph& g, vector<struct X>& B, bool* X_in_set_B[]){
    int one_dim_idx = 0;
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if(X_in_set_B[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            struct X u_X = g.U[i][j];
            vector<struct X>  tmpB = B;
            tmpB.push_back(u_X);

            if(get_group_cost(tmpB) <= budget){
                set_C.push_back(u_X);
            }
        }
    }
}

void get_max_idx_from_C(int max_X_idx, vector<struct X>& B, vector<struct X>& set_C, Graph& g){
    double max_F = -INT16_MAX;
    vector<vector<struct X>>tmpB = one_to_two_dim(B);
    for(size_t i=0;i<set_C.size();i++){
        tmpB[set_C[i].t].push_back(set_C[i]);

        double F = diffusion(tmpB, g);
        if(max_F < F){
            max_F = F;
            max_X_idx = i;
        }
        //For removing current X
        tmpB[set_C[i].t].pop_back();
    }
}

                
void migrate_strategy(vector<struct X>& B, vector<struct X>& set_C_i_next_day, int max_X_idx, bool* X_in_set_B[]){
    B.push_back(set_C_i_next_day[max_X_idx]);
    set_C_i_next_day.erase(set_C_i_next_day.begin() + max_X_idx);
    (*X_in_set_B)[set_C_i_next_day[max_X_idx].one_dim_id] = true;
}

bool is_out_of_cost(vector<struct X>& B, vector<vector<struct X>>& U, bool* X_in_set_B[]){
    int one_dim_idx = 0;
    for(size_t i=0;i<U.size();i++){ // each X_t in U;
        for(size_t j=0;j<U[i].size();j++){ // each X in X_t
            if(X_in_set_B[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            struct X u_X = U[i][j];
            vector<struct X> tmpB = B;    
            tmpB.push_back(u_X);        
            if(get_group_cost(tmpB) < budget){
                return false;
            }
        }
    }
    return true;
};

void get_X_max_F(double* max_X_F, vector<struct X>& max_X_list, Graph& g){
    double max_F = -INT16_MAX;
    struct X max_X;
    vector<vector<struct X>> tmpX;
    init_strategy(tmpX);
    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            struct X u_X = g.U[i][j];
            if(get_X_cost(u_X) <= budget){
                tmpX[i].push_back(u_X);
                double tmp_F = diffusion(tmpX, g);
                if(tmp_F > max_F){
                    max_F = tmp_F;
                    max_X = u_X;
                }
                tmpX[i].clear();
            }
        }
    }  
    *max_X_F= max_F;
    max_X_list.push_back(max_X);
} 

void get_argmax_strategy(vector<vector<struct X>> &S, vector<struct X>&A, vector<struct X>&B, Graph& g){
    double max_A_F=0;
    double max_B_F=0;
    double max_X_F=0;
    
    vector<struct X> X_list;
    max_A_F = diffusion(one_to_two_dim(A), g);
    max_B_F = diffusion(one_to_two_dim(B), g);
    get_X_max_F(&max_X_F, X_list, g);


    if(max_A_F > max_B_F){
        if(max_A_F > max_X_F){
            S = one_to_two_dim(A);
        }else{
            S = one_to_two_dim(X_list);
        }
    }else{
        if(max_B_F > max_X_F){
            S = one_to_two_dim(B);
        }else{
            S = one_to_two_dim(X_list);
        }
    }
}

void update_softmax_value(vector<struct CRObj>& c_obj_list){
    double e_denominator=0;
    for(size_t i=0;i<c_obj_list.size();i++)
        e_denominator += exp(c_obj_list[i].diff);

    for(size_t i=0;i<c_obj_list.size();i++)
        c_obj_list[i].softmax_value = exp(c_obj_list[i].diff) / e_denominator;
    
}

void clear_list_before_k(vector<struct X>& B, bool* X_in_set_B[], size_t k_day, int U_LENGTH){
    memset((*X_in_set_B), false, U_LENGTH * sizeof(bool));
    for(size_t i=0;i<B.size();i++){
        if(i < k_day){
            *X_in_set_B[B[i].one_dim_id] = true;
        }else{
            B.erase(B.begin() + i);
            i--;
        }
    }
}

// Notice the length of B & C is i, not i+1
size_t RCR(vector<struct X>& A, vector<struct X>& B,vector<vector<struct X>>& C, int j_day, int i_day, Graph& g, bool* X_in_set_B[]){
    assert(j_day < i_day);
    vector<struct CRObj> c_obj_list;
    srand(time(0));
    
    for(int k=j_day+1;k<=i_day;k++){
        if(C[k].size() == 0)
            continue;
        for(size_t ck=0;ck<C[k].size();ck++){
            vector<struct X> tmpA = get_sublist(A, k+1);
            vector<struct X> tmpB = get_sublist(B, k);
            tmpB.push_back(C[k][ck]);
            if(diffusion(one_to_two_dim(tmpB), g) < (1 - delta_f) * diffusion(one_to_two_dim(tmpA), g)){
                C.erase(C.begin() + ck);
                ck--;
            }else{
                struct CRObj c_obj;
                c_obj.c_X = C[k][ck];
                c_obj.k_day = k;
                c_obj.diff = CR_diff();
                c_obj_list.push_back(c_obj);
            }
        }
    }

    update_softmax_value(c_obj_list);
    if(c_obj_list.size()!=0){
        bool find_redisign_C = false;
        while(!find_redisign_C){
            int rand_idx = (rand() % c_obj_list.size()); //here: 0;
            double rand_r = (rand() % 100) / 100.0;
            if(rand_r < c_obj_list[rand_idx].softmax_value){
                find_redisign_C = true;

                // Not yet implement removing element from C_l, but I think it's not really necessary to do it?
                // Becaurse j+1 will never less than l
                clear_list_before_k(B, X_in_set_B, c_obj_list[rand_idx].k_day, g.U_LENGTH);
                B.push_back(c_obj_list[rand_idx].c_X);
                *X_in_set_B[c_obj_list[rand_idx].c_X.one_dim_id] = true;
                return B.size();
            }
        }
    }else{
        // vector<struct X> set;
        // C.push_back(set);
        // // Notice that X_in_set_B.i+1 day true ? false ?
        // cost_update_C(C[C.size()-1], g, B, X_in_set_B);
        // for(size_t i=0;i<C[C.size()-1].size();i++){
        //     // Find argmax B
        // }
    }
}

