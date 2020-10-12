#include <string.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <errno.h>
#include "init.h"
#include "graph.h"
#include "diff_func.h"
#include "algo.h"
using namespace std;

const char* NAME = "opt.txt";
extern char* RESULT_DIR;
extern int sample_size;
extern size_t period_T;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];
extern const size_t n_size;
extern const size_t g_size;
extern const size_t e_size;
extern const size_t x_size;

clock_t total_start, total_end;
void opt(Graph &g, const char* GRAPH_FILE);
struct el opt_el0, opt_el1 = {0.5, 0.5}, opt_el2 = {1, 1};
vector<struct el> level_table_opt = {opt_el0, opt_el1, opt_el2};

int main(int argc, char **argv){
    Graph g;
    total_start = clock();
    if(argc != 2){
        printf("Wrong argument. Execution format: ./main config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    opt(g, GRAPH_PATH);
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



void opt(Graph &g, const char* GRAPH_FILE) {
    FILE *fp_graph = fopen(GRAPH_FILE, "r");
    if (fp_graph == NULL) {
        printf("Failed to open file %s.", GRAPH_FILE);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    double max = INT_MIN;
    vector<vector<struct X> > max_content;
    int index = 0;
    int max_index = 0;

    while ((getline(&line, &len, fp_graph)) != -1) {
        char* type = strtok(line, " ");
        char* data = strtok(NULL, " ");
        vector<char*>input_line;
        if(strcmp(type, "g") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == g_size && "Wrong numbers of g type");
            int V=atoi(input_line[0]);
            int E=atoi(input_line[1]); //input_line[2]: U length

            // U_LENGTH is not used in opt.
            int U_LENGTH = 0;
            g.init_graph(V, E, U_LENGTH);
        }
        else if(strcmp(type, "e") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == e_size && "Wrong numbers of e type");
            init_edge(g, input_line);
        }else if(strcmp(type, "n") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == n_size && "Wrong numbers of n type");
            init_node(g, input_line);
        }else if(strcmp(type, "S") == 0){
            get_split_data(input_line, data, "_");
            assert(input_line.size() == x_size && "Wrong numbers of s type");
            init_strategy_U(g, input_line);
        }else if(strcmp(type, "*") == 0){
            double compare = 0;
            compare = diffusion(g.U, g, level_table_opt);
            if (compare > max){
                max = compare;
                max_index = index;
                max_content = g.U;
            }
            g.U.clear();
            // Init U
            for(size_t i=0;i<period_T;i++){
                vector<struct X> tmp;
                g.U.push_back(tmp);
            }
            index += 1;
            cout << "Strategy index :" <<index <<endl;
        }
        else{
            assert(false && "wrong input");
            cout<<"wrong input"<<endl;
        }
    }
    double empty_F = diffusion(g.U, g);
    if(max < empty_F){
        max = empty_F;
        max_index = -1;
        max_content = g.U;
    }

    FILE * pFile;
    pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf(pFile,"Optimal Value : %3f\nOptimal Strategy Index : %d\n\nStrategy :\n",max,max_index);
    for(size_t i=0;i<max_content.size();i++){
        for(size_t j=0;j<max_content[i].size();j++){
            fprintf (pFile, "%d_%d ", max_content[i][j].t, max_content[i][j].id);
        }
    }
    fprintf(pFile, "\n");
    fclose (pFile);

    fclose(fp_graph);
    if (line) free(line);
}