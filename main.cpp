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
}

vector<vector<struct X> > algo_main(Graph& g){
    printf("Start main algorithm ..\n\n");
    vector<vector<struct X> > A, B, S;
    init_strategy(A);
    init_strategy(B);
    double prev_best_A=diffusion(A, g);
    double cost_A = get_group_cost(A);
    bool* X_in_set_A = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_A, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    double* diff_baseline_table = (double*) malloc(g.U_LENGTH*sizeof(double));
    memset(diff_baseline_table, 0, g.U_LENGTH * sizeof(double)); // for clearing previous record
    size_t iter = 0;
    clock_t start, end;
    while((cost_A < budget) && has_candidate(diff_baseline_table, g.U_LENGTH)){
        start = clock();
        vector<struct X> C;
        calc_main(g, A, prev_best_A, cost_A, &diff_baseline_table, X_in_set_A);
        PSPD_main(g, A, &diff_baseline_table, &X_in_set_A, &prev_best_A);
        cost_A = get_group_cost(A);
        end = clock();
        printf("[ Iter: %lu ] %fs\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));
    }
    free(X_in_set_A);
    free(diff_baseline_table);
    return S;
}