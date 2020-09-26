#include <string.h>
#include <fstream>
#include <iostream>
#include "data.h"
#include "diff_func.h"
#include "graph.h"
#include "greedy.h"

using namespace std;

void init_node(Graph& g, vector<char*>& input_line);
void init_edge(Graph& g, vector<char*>& input_line);
void init_group(Graph& g, vector<char*>& input_line);
void get_split_data(vector<char*>& input_line, char* data, char data_delim[]);
void create_graph(Graph &g, const char* GRAPH_FILE);
void create_quarantine_strategy(Graph& g, const char* GROUP_FILE);

struct X X1, X2, X3, X4;
struct el el0 = {0, 0.5}, el1 = {0.3, 0.3}, el2 = {0.7, 0.7}, el3 = {1, 1};
vector<struct el> level_table = {el0, el1, el2, el3};

int main() {
    Graph g;
    const char* GRAPH_FILE = "./covid_data/4/graph.txt";
    const char* GROUP_FILE = "./covid_data/4/group.txt";
    create_graph(g, GRAPH_FILE);
    create_quarantine_strategy(g, GROUP_FILE);
    
    greedy_algo(g);
}


void init_node(Graph& g, vector<char*>& input_line){
    struct node* n = (struct node*)malloc(sizeof(struct node));
    n->id = atoi(input_line[0]);
    n->type = atoi(input_line[1]);
    n->params.relative = atof(input_line[2]);
    n->params.contagion = atof(input_line[3]);
    n->params.symptom = atof(input_line[4]);
    n->params.critical = atof(input_line[5]);
    n->params.healing_fromI = atof(input_line[6]);
    n->params.healing_fromA = atof(input_line[7]);
    n->params.healing_fromT = atof(input_line[8]);
    n->params.death = atof(input_line[9]);
    g.N.push_back(n);
}

void init_edge(Graph& g, vector<char*>& input_line){
    g.addEdge(atoi(input_line[0]), atoi(input_line[1]), atoi(input_line[2]));
}

void init_group(Graph& g, vector<char*>& input_line){
    struct X x;
    x.t = atoi(input_line[0]);
    x.cost = atoi(input_line[1]);
    x.lv = atoi(input_line[2]);
    x.eta = atoi(input_line[3]);
    char *line = NULL;
    char delim[] = ",";
    char *token = strtok(input_line[4], delim);
    while (token) {
        x.D.push_back(atoi(token));
        token = strtok(NULL, delim);
    }
    g.U[x.t].push_back(x);
}

void get_split_data(vector<char*>& input_line, char* data, char data_delim[]){
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
    if (fp_graph == NULL) 
        exit(EXIT_FAILURE);
    
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
            g.init_graph(V, E);
        }else if(strcmp(type, "e") == 0){
            get_split_data(input_line, data, ",");
            init_edge(g, input_line);
        }else if(strcmp(type, "n") == 0){
            get_split_data(input_line, data, ",");
            init_node(g, input_line);
        }else if(strcmp(type, "X") == 0){
            get_split_data(input_line, data, "_");
            init_group(g, input_line); // TODO
        }else{
            cout<<"wrong input"<<endl;
        }
    }
    fclose(fp_graph);
    if (line) free(line);
}

void create_quarantine_strategy(Graph& g, const char* GROUP_FILE){
    FILE *fp_graph = fopen(GROUP_FILE, "r");
    char *line = NULL;
    size_t len = 0;
    char type_delim[] = " ";
    char data_delim[] = ",";
    if (fp_graph == NULL) exit(EXIT_FAILURE);
    
    while ((getline(&line, &len, fp_graph)) != -1) {
        char* type = strtok(line, type_delim);
        char* data = strtok(NULL, type_delim);
        
        vector<char*>input_line;
        char *token = strtok(data, data_delim);
        while (token) {
            input_line.push_back(token);
            token = strtok(NULL, data_delim);
        }

        if(strcmp(type, "g") == 0){
            int V=atoi(input_line[0]);
            int E=atoi(input_line[1]);
            g.init_graph(V, E);
        }else if(strcmp(type, "e") == 0){
            init_edge(g, input_line);
        }else if(strcmp(type, "n") == 0){
            init_node(g, input_line);
        }else if(strcmp(type, "X") == 0){
            init_group(g, input_line);
        }else{
            cout<<"wrong input"<<endl;
        }
    }
    fclose(fp_graph);
    if (line) free(line);
}
