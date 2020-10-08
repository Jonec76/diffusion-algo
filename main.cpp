#include <stdio.h>
#include <string.h>
#include <time.h>
#include <cstdlib>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
using namespace std;

const char* NAME = "main.txt";
extern int sample_size;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];
clock_t total_start, total_end;
void get_list_A(vector<struct X> & A, Graph& g);
void get_list_B(vector<struct X> & B, vector<struct X> & A, Graph& g);

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
    pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
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
    int i=0, j=-1;
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
        printf("[ Iter: %lu ] %fs\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));
    }
    free(X_in_set_A);
    free(diff_baseline_table);
}

void get_list_B(vector<struct X> & B, vector<struct X> & A, Graph& g){
    double cost_B = get_group_cost(B);
    int i_day=0, j_day=-1;
    size_t len_A = A.size();
    bool* X_in_set_B = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_B, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    vector<vector<struct X>> set_C;
    vector<struct X> empty;
    set_C.push_back(empty);
    while(cost_B < budget){
        vector<struct X> set_i_next_day;
        set_C.push_back(set_i_next_day);
        if(i_day < len_A){
            PSPD_update_C(set_C[i_day+1], g, B, A, &X_in_set_B, i_day);
        }else{
            cost_update_C(set_C[i_day+1], g, B, &X_in_set_B);
        }

        if(set_C.size()!=0){
            int max_X_idx_in_C;
            get_max_idx_from_C(max_X_idx_in_C, B, set_C[i_day+1], g);
            vector<struct X> tmpB = B;
            tmpB.push_back(set_C[i_day+1][max_X_idx_in_C]);
            double F_b = diffusion(one_to_two_dim(tmpB), g);
            vector<struct X> sub_A = get_sublist(A, i_day+1);
            double F_a = diffusion(one_to_two_dim(sub_A), g);

            if(F_b >= (1 - delta_f)*F_a){
                migrate_strategy(B, set_C[i_day+1], max_X_idx_in_C, &X_in_set_B);
                i_day++;
            }else{
                // RCR
            }
        }else{
            double F_b = diffusion(one_to_two_dim(B), g);
            vector<struct X> sub_A = get_sublist(A, i_day+1);
            double F_a = diffusion(one_to_two_dim(sub_A), g);
            bool better_F = F_b > (1 - delta_f) * F_a;
            bool X_out_of_cost = is_out_of_cost(B, g.U, &X_in_set_B);

            if(better_F || X_out_of_cost){
                continue;
            }else{
                // RCR
            }
        }
    }
    vector<vector<struct X>> S;
    get_argmax_strategy(S, A, B, g);
}
