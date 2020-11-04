#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cstdlib>
#include <assert.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
#include "sas.h"
using namespace std;

const char* NAME = "main.txt";
extern int sample_size;
extern double budget, delta_f;
extern char GRAPH_PATH[50];
extern char OUTPUT_PATH[30];
extern size_t period_T;

clock_t total_start, total_end;
void get_list_A(vector<struct X> & A, Graph& g);
void get_list_B(vector<struct X> & B, vector<struct X> & A, Graph& g);
int RA = 4000;


int main(int argc, char **argv){
    total_start = clock();
    Graph g;
    if(argc != 2){
        printf("Wrong argument. Execution format: ./main config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);
    algo_main(g);
    total_end = clock();

    FILE * pFile;
    pFile = fopen (OUTPUT_PATH, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_PATH);
        exit(EXIT_FAILURE);
    }
    printf("Total time : %fs", (double)((total_end - total_start) / CLOCKS_PER_SEC));
    fprintf(pFile, "\n------------------------\n");
    fprintf(pFile, "Total time : %fs\n", (double)((total_end - total_start) / CLOCKS_PER_SEC));
    fclose(pFile);
    return 0;
}

void algo_main(Graph& g){
    vector<struct X> A, B;

    struct X null_X;
    null_X.one_dim_id = -1;
    null_X.t = -1;
    null_X.cost = 0;
    null_X.lv = -1;
    null_X.id = -1;

    B.push_back(null_X);

    get_list_A(A, g);
    get_list_B(B, A, g);
}

void get_list_A(vector<struct X> & A, Graph& g){
    printf("Start get A algorithm ..\n\n");
    double prev_best_A=diffusion(one_to_two_dim(A), g);
    double cost_A = get_group_cost(A);

    bool* X_in_set_A = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_A, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    double* diff_baseline_table = (double*) malloc(g.U_LENGTH*sizeof(double));
    memset(diff_baseline_table, 0, g.U_LENGTH * sizeof(double)); // for clearing previous record
    size_t iter = 0;
    clock_t start, end;
    while((cost_A < budget) && has_candidate_A(diff_baseline_table, g.U_LENGTH)){
        start = clock();
        calc_main_A(g, A, prev_best_A, cost_A, &diff_baseline_table, X_in_set_A);
        PSPD_update_A(g, A, &diff_baseline_table, &X_in_set_A, &prev_best_A);
        cost_A = get_group_cost(A);
        end = clock();
        printf("[ Iter A: %lu ] %fs\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));
    }
    free(X_in_set_A);
    free(diff_baseline_table);

    struct X null_X;
    null_X.one_dim_id = -1;
    null_X.t = -1;
    null_X.cost = 0;
    null_X.lv = -1;
    null_X.id = -1;
    A.insert(A.begin(), null_X);
}

void get_list_B(vector<struct X> & B, vector<struct X> & A, Graph& g){
    
    int i_day=0, j_day=0;
    double cost_B = 0;

    int len_A = A.size() -1; // For ignoring the first empty element.
    bool* X_in_set_B = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_B, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    bool* picked_day = (bool*) malloc((period_T+1)*sizeof(bool));
    memset(picked_day, false, (period_T+1) * sizeof(bool)); // for clearing previous record

    vector<vector<struct X>> set_C;
    bool A_finish_with_no_better_B = false;
    clock_t start, end;
    size_t iter = 0;
    while(cost_B < budget && !A_finish_with_no_better_B){
        start = clock();

        vector<struct X> C_per_t ; // Not to insert null element
        if(i_day+1 < len_A){
            PSPD_get_C_i(C_per_t, g, B, A, &X_in_set_B, i_day);
        }else{
            cost_update_C(C_per_t, g, B, &X_in_set_B, i_day);
        }
        assert(i_day+1 > (int)set_C.size());
        if(C_per_t.size()!=0){
            // argmax F, X belongs to "C"_i+1, where "C"_i+1 = C_per_t
            int max_X_idx_in_C;

            // max_B_F = F("B"_i U {B_i+1})
            double max_B_F;
            get_max_idx_from_C(&max_B_F, &max_X_idx_in_C, B, C_per_t, g);
            // F("A"_i+1)
            vector<struct X> A_i;
            if(i_day+1 < len_A){
                A_i = get_sublist(A, i_day+1);
            }else{
                A_i = A;
            }
            double F_a = diffusion(one_to_two_dim(A_i), g);


            if(max_B_F >= (1 - delta_f)*F_a){
                migrate_strategy(B, C_per_t, max_X_idx_in_C, &X_in_set_B);
                set_C.push_back(C_per_t);// C_i -> C_i+1
                i_day++;
            }else{
                // RCR: 1. trim list C 
                //      2. C_i -> C_i+1
                cout<<"[ In RCR 1 ]\n";
                if(RCR_simple_version)
                    i_day = RCR_simple(A, B, set_C, i_day, g, &X_in_set_B, &picked_day);
                else{
                    j_day = RCR(A, B, set_C, j_day, i_day, g, &X_in_set_B);
                    i_day = j_day+1;
                }
            }
        }else{
            // F("B"_i)
            vector<struct X> B_i = get_sublist(B, i_day);
            double F_b = diffusion(one_to_two_dim(B_i), g);

            // F("A"_i+1)
            vector<struct X> A_i;

            if(i_day+1 < len_A){
                A_i = get_sublist(A, i_day+1);
            }else{
                A_i = A;
            }
            double F_a = diffusion(one_to_two_dim(A_i), g);

            bool better_F = F_b > (1 - delta_f) * F_a;
            bool X_out_of_cost = is_out_of_cost(B_i, g.U, &X_in_set_B);

            if(X_out_of_cost){
                if(i_day + 1 >= len_A){
                    A_finish_with_no_better_B = true; // it must finishs this while loop
                    continue;
                }
            }

            if(better_F){
                cost_update_C(C_per_t, g, B, &X_in_set_B, i_day);
                int max_X_idx_in_C;

                // max_B_F = F("B"_i U {B_i+1})
                double F_b_nextday;
                get_max_idx_from_C(&F_b_nextday, &max_X_idx_in_C, B_i, C_per_t, g);
                if(F_b_nextday > F_b){
                    migrate_strategy(B, C_per_t, max_X_idx_in_C, &X_in_set_B);
                    set_C.push_back(C_per_t);
                    i_day++;
                }else if(i_day + 1 >= len_A){
                    break;
                }else{
                    continue;
                }
            }else{
                cout<<"[ In RCR 2 ]\n";
                if(RCR_simple_version)
                    i_day = RCR_simple(A, B, set_C, i_day, g, &X_in_set_B, &picked_day);
                else{
                    j_day = RCR(A, B, set_C, j_day, i_day, g, &X_in_set_B);
                    i_day = j_day+1;
                }
            }
        }
        cost_B = get_group_cost(B);
        // print_list(B);
        end = clock();
        printf("[ Iter B: %lu ] %fs\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));

    }

    vector<vector<struct X>> S;
    double max_F;
    get_argmax_strategy(&max_F, S, A, B, g);
    SAS(S, RA, g, max_F);
    free(X_in_set_B);
    free(picked_day);
}
