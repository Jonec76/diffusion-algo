#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "greedy.h"
#include "graph.h"
using namespace std;

extern const char* BASELINE_FILE;

vector<vector<struct X> > greedy_algo(Graph g){
    printf("Start greedy algorithm ..\n\n");
    vector<vector<struct X>> A, B, S;
    init_strategy(A);
    init_strategy(B);
    int i=0, j=-1;
    double prev_best_A=0;
    double cost_A = get_group_cost(A);
    double cost_B = get_group_cost(B);
    bool* X_in_set_A = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_A, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    double* diff_baseline_table = (double*) malloc(g.U_LENGTH*sizeof(double));
    memset(diff_baseline_table, 0, g.U_LENGTH * sizeof(double)); // for clearing previous record
    size_t iter = 0;
    FILE * pFile;
    clock_t start, end;
    pFile = fopen (BASELINE_FILE, "a");
    while((cost_A < budget) && has_candidate(diff_baseline_table, g.U_LENGTH)){
        start = clock();
        fprintf (pFile, " [ Iter: %d ] %f\n", iter, (double)((end - start) / CLOCKS_PER_SEC));
        vector<struct X> C;
        calc_baseline(g, A, prev_best_A, cost_A, &diff_baseline_table, X_in_set_A, sample_size);
        PSPD(g, A, &diff_baseline_table, &X_in_set_A, &prev_best_A);
        cost_A = get_group_cost(A);
        end = clock();
        printf("[ Iter: %d ] %f\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));
    }
    free(X_in_set_A);
    free(diff_baseline_table);
    fclose(pFile);
    return S;
}