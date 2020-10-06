#include <string.h>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <regex>
#include <assert.h>

#include "data.h"
#include "diff_func.h"
#include "graph.h"
#include "init.h"

using namespace std;

struct X X1, X2, X3, X4;

// el0 is not used (0 for the group who are not quantined. we handle it in get_X_cost)

struct el el0, el1 = {0.2, 0.2}, el2 = {0.8, 0.8}, el3 = {1, 1};
vector<struct el> level_table = {el0, el1, el2, el3};
const char* RESULT_DIR = "./result/";
const char* GRAPH_DIR = "./covid_data/";
char OUTPUT_FILE[50];

char GRAPH_PATH [50];
size_t sample_size = 1, period_T=10;
double budget = 10;

void init_node(Graph& g, vector<char*>& input_line){
    struct node* n = (struct node*)malloc(sizeof(struct node));
    n->id = atoi(input_line[0]);
    n->type = atoi(input_line[1]);
    n->params.relative = atof(input_line[2]);
    n->params.contagion = atof(input_line[3]); // Using get_contagion(type)? or random value?
    n->params.symptom = atof(input_line[4]);
    n->params.critical = atof(input_line[5]);
    n->params.healing_fromI = atof(input_line[6]);
    n->params.healing_fromA = atof(input_line[7]);
    n->params.healing_fromT = atof(input_line[8]);
    n->params.death = atof(input_line[9]);
    g.N.push_back(n);
}

void init_edge(Graph& g, vector<char*>& input_line){
    g.addEdge(atoi(input_line[0]), atoi(input_line[1]), atof(input_line[2]));
}

void init_strategy(Graph& g, vector<char*>& input_line){
    struct X x;
    x.t = atoi(input_line[0]);
    assert((unsigned int)x.t < period_T);
    x.cost = atoi(input_line[1]);
    x.lv = atoi(input_line[2]);
    assert(1 == x.lv || 2 == x.lv || 3 == x.lv);
    x.eta = atoi(input_line[3]);
    x.id = g.U[x.t].size();
    char delim[] = ",";
    char *token = strtok(input_line[4], delim);
    while (token) {
        x.D.push_back(atoi(token));
        token = strtok(NULL, delim);
    }
    g.U[x.t].push_back(x);
}

void get_split_data(vector<char*>& input_line, char* data, char const data_delim[]){
    char *token = strtok(data, data_delim);
    while (token) {
        input_line.push_back(token);
        token = strtok(NULL, data_delim);
    }
}


/**
 * " ": split type
 * ",": split data(exclude group data)
 * "_": split group data
 */
void create_graph(Graph &g, const char* GRAPH_FILE) {
    FILE *fp_graph = fopen(GRAPH_FILE, "r");
    if (fp_graph == NULL) {
        printf("Failed to open file %s.", GRAPH_FILE);
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;

    while ((getline(&line, &len, fp_graph)) != -1) {
        char* type = strtok(line, " ");
        char* data = strtok(NULL, " ");
        vector<char*>input_line;
        if(strcmp(type, "g") == 0){
            get_split_data(input_line, data, ",");
            int V=atoi(input_line[0]);
            int E=atoi(input_line[1]); //input_line[2]: U length
            int U_LENGTH = atoi(input_line[2]);
            g.init_graph(V, E, U_LENGTH);
        }else if(strcmp(type, "e") == 0){
            get_split_data(input_line, data, ",");
            init_edge(g, input_line);
        }else if(strcmp(type, "n") == 0){
            get_split_data(input_line, data, ",");
            init_node(g, input_line);
        }else if(strcmp(type, "X") == 0){
            get_split_data(input_line, data, "_");
            init_strategy(g, input_line); // TODO
        }else{
            cout<<"wrong input"<<endl;
        }
    }
    fclose(fp_graph);
    if (line) free(line);
}


void set_config(char* argv, const char* file_name){
    FILE *fp_config = fopen(argv, "r");
    if (fp_config == NULL) {
        printf("Failed to open file %s.", argv);
        exit(EXIT_FAILURE);
    }

    DIR* dir = opendir(RESULT_DIR);
    if (dir) {
        closedir(dir);
    } else{
        if (mkdir("result", S_IRWXU|S_IRWXG|S_IROTH))
            printf("wrong at create dir");
    }

    char *line = NULL;
    size_t len = 0;
    vector<char*>input_line;

    while ((getline(&line, &len, fp_config)) != -1) {
        char* type = strtok(line, " ");
        char* data = strtok(NULL, " ");
        if(strcmp(type, "#") == 0){
            continue;
        }else if(strcmp(type, "c") == 0){
            get_split_data(input_line, data, ",");
        }
    }
    
    strcat(GRAPH_PATH, GRAPH_DIR);
    strcat(GRAPH_PATH, input_line[0]);

    sample_size = atoi(input_line[1]);
    budget = atof(input_line[2]);
    period_T = atoi(input_line[3]);

    string tmp = RESULT_DIR;
    // For cleaning string

    tmp += file_name;
    tmp += "_";
    tmp += input_line[0]; 
    tmp += "_"; 
    tmp += input_line[1];
    tmp += "_"; 
    tmp += input_line[2];
    tmp += "_"; 
    tmp += input_line[3];

    regex reg(".txt");
    tmp = regex_replace(tmp, reg, "");

    tmp += ".txt"; 

    strcpy(OUTPUT_FILE, tmp.c_str());
    
    FILE* pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    fprintf (pFile, "=======================================================\n");
    fprintf (pFile, "%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s\n","Graph file: ", input_line[0], "Sample size ", input_line[1], "Budget ", input_line[2], "T", input_line[3]);
    fprintf (pFile, "=======================================================\n");
    fclose (pFile);
    
    printf ("%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s\n","Graph file ", input_line[0], "Sample size ", input_line[1], "Budget ", input_line[2], "T", input_line[3]);
}