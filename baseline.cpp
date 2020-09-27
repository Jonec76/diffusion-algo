#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
using namespace std;

extern const char* BASELINE_FILE;

int main(){
    Graph g;
    const char* GRAPH_FILE = "./covid_data/new_data/graph_4.txt";
    create_graph(g, GRAPH_FILE);
    algo_baseline(g);
}

vector<vector<struct X> > algo_baseline(Graph& g){
    printf("Start baseline algorithm ..\n\n");
    vector<vector<struct X> > A, B, S;
    init_strategy(A);
    init_strategy(B);
    double prev_best_A=0;
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
        calc_baseline(g, A, prev_best_A, cost_A, &diff_baseline_table, X_in_set_A, sample_size);
        PSPD_baseline(g, A, &diff_baseline_table, &X_in_set_A, &prev_best_A);
        cost_A = get_group_cost(A);
        end = clock();
        printf("[ Iter: %lu ] %f\n", iter++, (double)((end - start) / CLOCKS_PER_SEC));
    }
    free(X_in_set_A);
    free(diff_baseline_table);
    return S;
}

