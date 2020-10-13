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
char MIPC_OUTPUT_FILE[50];

char MIPC_GRAPH_PATH [50];


// c
const size_t c_size = 4;

char GRAPH_PATH [50];
size_t sample_size = 0;
size_t period_T=0;
double budget = 0;


// p

const size_t p_size = 8;

double w_S = 0;
double w_I = 0;
double w_R = 0;

double delta_c = 0;
double delta_f = 0;
double delta_i = 0;

double THETA  = 0;
double A_END  = 0;

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
    n->a_v = atof(input_line[10]);
    g.N.push_back(n);
}

void init_edge(Graph& g, vector<char*>& input_line){
    g.addEdge(atoi(input_line[0]), atoi(input_line[1]), atof(input_line[2]));
}

void init_strategy_U(Graph& g, vector<char*>& input_line){
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
void init_strategy_one_dim_id(vector<vector<struct X>>& U){
    int one_dim_idx =0;
    for(size_t i=0;i<U.size();i++){
        for(size_t j=0;j<U[i].size();j++){
            U[i][j].one_dim_id = one_dim_idx;
            one_dim_idx++;
        }
    }
}; 


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
    size_t x_nums = 0;
    while ((getline(&line, &len, fp_graph)) != -1) {
        char* type = strtok(line, " ");
        char* data = strtok(NULL, " ");
        vector<char*>input_line;
        if(strcmp(type, "g") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == g_size && "Wrong numbers of g type");
            int V=atoi(input_line[0]);
            int E=atoi(input_line[1]); //input_line[2]: U length
            int U_LENGTH = atoi(input_line[2]);
            g.init_graph(V, E, U_LENGTH);
        }else if(strcmp(type, "e") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == e_size && "Wrong numbers of e type");
            init_edge(g, input_line);
        }else if(strcmp(type, "n") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == n_size && "Wrong numbers of n type");
            init_node(g, input_line);
        }else if(strcmp(type, "X") == 0){
            x_nums++;
            get_split_data(input_line, data, "_");
            assert(input_line.size() == x_size && "Wrong numbers of x type");
            init_strategy_U(g, input_line); // TODO
        }else{
            assert(false && "wrong type in graph file");
            break;
        }
    }
    assert(g.U_LENGTH == x_nums && "Wrong parameter at 'g' 3rd parameter setting");

    fclose(fp_graph);
    if (line) free(line);
    init_strategy_one_dim_id(g.U);
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
    vector<char*>input_line, params_input_line;

    while ((getline(&line, &len, fp_config)) != -1) {
        char* type = strtok(line, " ");
        char* data = strtok(NULL, " ");
        if(strcmp(type, "#") == 0){
            continue;
        }else if(strcmp(type, "c") == 0){
            get_split_data(input_line, data, ",");
            assert(input_line.size() == c_size && "Wrong c config");

            strcat(GRAPH_PATH, GRAPH_DIR);
            strcat(GRAPH_PATH, input_line[0]);
            sample_size = atoi(input_line[1]);
            budget = atof(input_line[2]);
            period_T = atoi(input_line[3]);
        }else if(strcmp(type, "p") == 0){
            get_split_data(params_input_line, data, ",");
            assert(params_input_line.size() == p_size && "Wrong p config");
            
            w_S = atof(params_input_line[0]);
            w_I = atof(params_input_line[1]);
            w_R = atof(params_input_line[2]);

            delta_c = atof(params_input_line[3]);
            delta_f = atof(params_input_line[4]);
            delta_i = atof(params_input_line[5]);

            THETA  = atof(params_input_line[6]);
            A_END  = atof(params_input_line[7]);
        }
        line = NULL;
    }
    

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
    regex nl("\n");
    tmp = regex_replace(tmp, reg, "");
    tmp = regex_replace(tmp, nl, "");
    tmp += ".txt"; 

    strcpy(OUTPUT_FILE, tmp.c_str());
    
    FILE* pFile = fopen (OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    printf ("=======================================================\n");
    printf ("%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s","Graph file: ", GRAPH_PATH, "Sample size ", input_line[1], "Budget ", input_line[2], "T", input_line[3]);
    printf ("=======================================================\n");
    
    
    fprintf (pFile, "=======================================================\n");
    fprintf (pFile, "%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s","Graph file: ", GRAPH_PATH, "Sample size ", input_line[1], "Budget ", input_line[2], "T", input_line[3]);
    fprintf (pFile, "=======================================================\n");
    fclose (pFile);
    fclose (fp_config);
}

void set_mipc_config(char* argv, const char* file_name){
    FILE *fp_config = fopen(argv, "r");
    if (fp_config == NULL) {
        printf("Failed to open file %s.", argv);
        exit(EXIT_FAILURE);
    }
    
    vector<char*>input_line;
    
    assert(true && "It needs to be rescheduled how to read the config file without seperate this function");


    strcat(MIPC_GRAPH_PATH, GRAPH_DIR);
    strcat(MIPC_GRAPH_PATH, input_line[0]);

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
    tmp += "_"; 
    tmp += input_line[4];

    regex reg(".txt");
    regex nl("\n");
    tmp = regex_replace(tmp, reg, "");
    tmp = regex_replace(tmp, nl, "");
    tmp += ".txt"; 

    strcpy(MIPC_OUTPUT_FILE, tmp.c_str());


    FILE* pFile = fopen (MIPC_OUTPUT_FILE, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", argv);
        exit(EXIT_FAILURE);
    }
    fprintf (pFile, "=======================================================\n");
    fprintf (pFile, "%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s\n%-15s :%s\n","Graph path ", input_line[0], "A end ", input_line[1], "Theta ", input_line[2], "T", input_line[3], "Start node", input_line[4]);
    fprintf (pFile, "=======================================================\n");
    fclose (pFile);
    fclose (fp_config);
}

