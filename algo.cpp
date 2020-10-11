#include <stdio.h>
#include <string.h>
#include "diff_func.h"
#include "graph.h"
#include "mipc.h"
#include <assert.h>
#include <math.h>
using namespace std;

extern vector<struct el>level_table;
extern char OUTPUT_FILE[30];
extern size_t sample_size, period_T;
extern double budget, delta_c, delta_f, delta_i;

double get_X_cost(struct X x_t){
    if(x_t.id == -1)
        return 0;
    if (x_t.lv == 0){
        return 1;
    }
    else{
        return level_table[x_t.lv].phi_cost * x_t.cost;
    }
}

void print_list(vector<struct X> list){
    for(size_t i=0;i<list.size();i++){
        cout<<list[i].t<<" "<<list[i].id<<" "<<list[i].one_dim_id<<endl;
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

// this can handle the group with empty element
double get_group_cost(vector<struct X>& group){
    double cost = 0;
    for(size_t t=0;t<group.size();t++){
        cost += get_X_cost(group[t]);
    }
    return cost;
}

vector<struct X> get_sublist(vector<struct X>& list, size_t i_day){
    vector<struct X> sublist;
    if(list.size() <= i_day){
        sublist = list;
        return sublist;
    };
    assert(i_day >= 0);
    for(size_t i=0;i<=i_day;i++)
        sublist.push_back(list[i]);
    return sublist;
}

//For C set, i_day+1 modify to i_day
vector<struct X> get_candidate_i(vector<vector<struct X>> list, size_t i_day){
    assert(list.size() >= i_day);
    vector<struct X> empty;
    if(i_day == 0)
        return empty;
    assert(i_day > 0);
    i_day--;
    return list[i_day];
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
        if(tmp.one_dim_id == -1)
            continue;
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

void PSPD_get_C_i(vector<struct X>& C_per_t, Graph& g, vector<struct X>& B, vector<struct X>& A, bool* X_in_set_B[], int i_day){
    int one_dim_idx = 0;
    
    // c("B"i U {X})
    vector<struct X> B_i = B;
    // c("A"i+1)
    vector<struct X> A_i = get_sublist(A, i_day+1);
    double cost_subA = min((1 + delta_c)* get_group_cost(A_i), budget);
    struct X a_X = A_i[i_day+1];
    vector<struct X> A_list = get_sublist(A, i_day);
    double IR_A_i_1 = (1 - delta_i) * IR(a_X, one_to_two_dim(A_list), g);

    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if((*X_in_set_B)[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            struct X u_X = g.U[i][j];
            
            bool better_IR = IR(u_X, one_to_two_dim(B_i), g) >= (1 - delta_i) * IR_A_i_1;
            B_i.push_back(u_X);
            bool availabel_cost = get_group_cost(B_i) <= cost_subA;
            if(better_IR && availabel_cost){
                C_per_t.push_back(u_X);
            }
            B_i.clear();
            one_dim_idx++;
        }
    }
}

void cost_update_C(vector<struct X>& set_C, Graph& g, vector<struct X>& B, bool* X_in_set_B[], int i_day){
    int one_dim_idx = 0;
    vector<struct X> B_i = B;

    for(size_t i=0;i<g.U.size();i++){ // each X_t in U;
        for(size_t j=0;j<g.U[i].size();j++){ // each X in X_t
            if((*X_in_set_B)[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            struct X u_X = g.U[i][j];
            B_i.push_back(u_X);
            if(get_group_cost(B_i) <= budget){
                set_C.push_back(u_X);
            }
            B_i.pop_back();
            one_dim_idx++;
        }
    }
}

void get_max_idx_from_C(double* max_B_F, int* max_X_idx, vector<struct X>& B_i, vector<struct X>& set_C, Graph& g){
    double max_F = -INT16_MAX;

    vector<vector<struct X>>tmpB = one_to_two_dim(B_i);
    for(size_t i=0;i<set_C.size();i++){
        tmpB[set_C[i].t].push_back(set_C[i]);
        double F = diffusion(tmpB, g);
        if(max_F < F){
            max_F = F;
            *max_X_idx = i;
        }
        //For removing current X
        tmpB[set_C[i].t].pop_back();
    }
    *max_B_F = max_F;
}

                
void migrate_strategy(vector<struct X>& B, vector<struct X>& C_per_t, int max_X_idx_in_C, bool* X_in_set_B[]){
    (*X_in_set_B)[C_per_t[max_X_idx_in_C].one_dim_id] = true;
    B.push_back(C_per_t[max_X_idx_in_C]);
    C_per_t.erase(C_per_t.begin() + max_X_idx_in_C);
}

bool is_out_of_cost(vector<struct X>& B, vector<vector<struct X>>& U, bool* X_in_set_B[]){
    int one_dim_idx = 0;
    vector<struct X> tmpB = B;
    for(size_t i=0;i<U.size();i++){ // each X_t in U;
        for(size_t j=0;j<U[i].size();j++){ // each X in X_t
            if((*X_in_set_B)[one_dim_idx]){
                one_dim_idx++;
                continue;
            }
            struct X u_X = U[i][j];
            tmpB.push_back(u_X);        
            if(get_group_cost(tmpB) < budget){
                one_dim_idx++;
                return false;
            }
            tmpB.pop_back();        
            one_dim_idx++;
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

    cout<<"A: "<<max_A_F<<endl;
    print_list(A);
    cout<<"cost: "<<get_group_cost(A)<<"\n============\n";

    cout<<"B: "<<max_B_F<<endl;
    print_list(B);
    cout<<"cost: "<<get_group_cost(B)<<"\n============\n";

    cout<<"X: "<<max_X_F<<endl;
    print_list(X_list);
    cout<<"cost: "<<get_group_cost(X_list)<<"\n============\n";

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

    assert(e_denominator != 0);
    for(size_t i=0;i<c_obj_list.size();i++)
        c_obj_list[i].softmax_value = exp(c_obj_list[i].diff) /e_denominator;
    
}

void clear_list_after_k(vector<struct X>& B, vector<vector<struct X>>& C, bool* X_in_set_B[], size_t k_day, int U_LENGTH){
    assert(B.size() == C.size()+1);
    memset((*X_in_set_B), false, U_LENGTH * sizeof(bool));
    for(size_t i=0;i<B.size();i++){
        if(B[i].id == -1)
            continue;
        if(i <= k_day-1){ // remains the list before k-1 day(include)
            (*X_in_set_B)[B[i].one_dim_id] = true;
        }else{
            B.erase(B.begin() + i);
            i--;
        }
    }
    for(size_t i=0;i<C.size();i++){
        if(i < k_day){ // C[0] means 1st day candidates, so kth day candidates means we should keep the list before C[k-1]
            continue;
        }else{
            C.erase(C.begin() + i);
            i--;
        }
    }
}

// i_day starts from 0, i_day means to redesign B[i_day+2] element
// Notice the length of B & C is i, not i+1
size_t RCR(vector<struct X>& A, vector<struct X>& B,vector<vector<struct X>>& C, int j_day, int i_day, Graph& g, bool* X_in_set_B[]){
    // assert(j_day < i_day);
    vector<struct CRObj> c_obj_list;

    bool X_in_set_C[g.U_LENGTH];
    memset(X_in_set_C, false, g.U_LENGTH * sizeof(bool));
    // k starts from 1

    for(int k=j_day+1;k<=i_day;k++){
        vector<struct X> A_i = get_sublist(A, k);
        vector<struct X> B_i = get_sublist(B, k-1);
        vector<struct X> C_i = get_candidate_i(C, k);
 
        if(C_i.size() == 0)
            continue;
        srand(time(0));

        for(size_t x=0;x<C_i.size();x++){
            B_i.push_back(C_i[x]);
            if(diffusion(one_to_two_dim(B_i), g) < (1 - delta_f) * diffusion(one_to_two_dim(A_i), g)){
                C_i.erase(C_i.begin() + x);
                x--;
            }else{
                if(!X_in_set_C[C_i[x].one_dim_id]){
                    X_in_set_C[C_i[x].one_dim_id] = true;
                    vector<struct X> B_k = get_sublist(B, k);
                    vector<struct X> B_list = get_sublist(B, k-1);
                    struct CRObj c_obj;
                    c_obj.c_X = C_i[x];
                    c_obj.k_day = k;
                    // CR=0: 
                    //    case1. there's no any v[t1] in h_prob(), so h_prob() will return 1
                    //           get_H_u will become 0 -> P_S_t=0 -> sum_value in CR = 0
                    c_obj.diff = CR(C_i[x], one_to_two_dim(B_i), g) - CR(B_k[B_k.size()-1], one_to_two_dim(B_i), g);
                    c_obj_list.push_back(c_obj);
                }
            }
            B_i.pop_back();
        }
    }

    if(c_obj_list.size()!=0){
        update_softmax_value(c_obj_list);
        bool find_redisign_C = false;
        while(!find_redisign_C){
            int rand_idx = (rand() % c_obj_list.size()); //here: 0;
            double rand_r = (rand() % 100) / 100.0;

            if(rand_r < c_obj_list[rand_idx].softmax_value){
                find_redisign_C = true;
                // Not yet implement removing element from C_l, but I think it's not really necessary to do it?
                // Becaurse j+1 will never less than l
                int l = c_obj_list[rand_idx].k_day; // keep the list before C_x_k(include k) and remove list after day k.
                clear_list_after_k(B, C, X_in_set_B, l, g.U_LENGTH);
                B.push_back(c_obj_list[rand_idx].c_X);
                (*X_in_set_B)[c_obj_list[rand_idx].c_X.one_dim_id] = true;
                assert(C.size()+1 == B.size());
                assert(l == (int)C.size());
                // cout<<l<<"~~"<<C.size()<<"--"<<B.size()<<" "<<i_day<<endl;
                return l-1;
            }
        }
    }else{
        vector<struct X> C_per_t ;
        // Notice that X_in_set_B.i+1 day true ? false ?
        cost_update_C(C_per_t, g, B, X_in_set_B, i_day);
        int max_X_idx_in_C;

        // max_B_F = F("B"_i U {B_i+1})
        double max_B_F;
        get_max_idx_from_C(&max_B_F, &max_X_idx_in_C, B, C_per_t, g);
        migrate_strategy(B, C_per_t, max_X_idx_in_C, X_in_set_B);
        C.push_back(C_per_t);// C_i -> C_i+1

        return i_day;
    }
    return 0;
}

