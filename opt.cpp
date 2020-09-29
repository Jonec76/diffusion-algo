#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
using namespace std;

const char* NAME = "opt.txt";
extern int sample_size;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];
clock_t total_start, total_end;

int main(int argc, char **argv){
    Graph g;
    total_end = clock();
    if(argc != 2){
        printf("Wrong argument. Execution format: ./main config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    // opt(g, GRAPH_PATH);
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

// void opt(Graph &g, const char* GRAPH_FILE) {
//     FILE *fp_graph = fopen(GRAPH_FILE, "r");
//     if (fp_graph == NULL) {
//         printf("Failed to open file %s.", GRAPH_FILE);
//         exit(EXIT_FAILURE);
//     }
    
//     DIR* dir = opendir(RESULT_DIR);
//     if (dir) {
//         closedir(dir);
//     } else{
//         if (mkdir("result", S_IRWXU|S_IRWXG|S_IROTH))
//             printf("wrong at create dir");
//     }
    
//     char *line = NULL;
//     size_t len = 0;
    
//     while ((getline(&line, &len, fp_graph)) != -1) {
//         char* type = strtok(line, " ");
//         char* data = strtok(NULL, " ");
//         vector<char*>input_line;
//         if(strcmp(type, "g") == 0){
//             get_split_data(input_line, data, ",");
//             int V=atoi(input_line[0]);
//             int E=atoi(input_line[1]); //input_line[2]: U length 
//             int U_LENGTH = atoi(input_line[2]);
//             g.init_graph(V, E, U_LENGTH);
//         }else if(strcmp(type, "e") == 0){
//             get_split_data(input_line, data, ",");
//             init_edge(g, input_line);
//         }else if(strcmp(type, "n") == 0){
//             get_split_data(input_line, data, ",");
//             init_node(g, input_line);
//         }else if(strcmp(type, "X") == 0){
//             get_split_data(input_line, data, "_");
//             init_strategy(g, input_line); // TODO
//         }
        
//         /**
//          * Record the index of strategy
//          * 
//          * else if( new symbol for opt groups){
//          *  get_split_data(input_line, data, "_");
//          *  init_opt_strategy(g, input_line); // TODO
//          * else if( * for opt groups) 
//          *  diffusion(U, 0, g)
//         */

//         else{
//             cout<<"wrong input"<<endl;
//         }
//     }
//     fclose(fp_graph);
//     if (line) free(line);
// }