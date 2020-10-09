#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"

using namespace std;
clock_t total_start, total_end;

const char* NAME = "diffusion.txt";
extern int sample_size;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];

int main(int argc, char **argv){
    Graph g;
    total_start = clock();
    double F;
    if(argc != 2){
        printf("Wrong argument. Execution format: ./diffusion config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);

    //We use strategies to replace U set
    F = diffusion_full_result(g.U, g);
    total_end = clock();

    FILE * pFile;
    pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    printf("Total time : %fs", (double)((total_end - total_start) / CLOCKS_PER_SEC));
    fprintf(pFile, "Total time      : %fs\n", (double)((total_end - total_start) / CLOCKS_PER_SEC));
    fprintf(pFile, "Diffusion value : %f\n", F);
    fclose(pFile);
}