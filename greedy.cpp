#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
using namespace std;

const char* NAME = "greedy.txt";
extern int sample_size;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];
clock_t total_start, total_end;

int main(int argc, char **argv){
    Graph g;
    total_start = clock();
    if(argc != 2){
        printf("Wrong argument. Execution format: ./main config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);
    algo_greedy(g);
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

vector<vector<struct X> > algo_greedy(Graph& g){
    printf("Start greedy algorithm ..\n\n");
    vector<vector<struct X> > S;
    vector<vector<struct node>> min_positive_group;
    bool* X_in_set_S = (bool*) malloc(g.U_LENGTH*sizeof(bool));
    memset(X_in_set_S, false, g.U_LENGTH * sizeof(bool)); // for clearing previous record
    init_strategy(S);
    init_positive_group(min_positive_group);
    double prev_greedy_S = diffusion_greedy(S, g);
    bool has_better_group = true;
    while(get_group_cost(S) < budget && has_better_group){
        calc_greedy(S, g, &X_in_set_S, &prev_greedy_S, &has_better_group);
    }
    free(X_in_set_S);
    return S;
}

