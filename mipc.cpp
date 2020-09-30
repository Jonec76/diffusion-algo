#include <stdio.h>
#include <string.h>
#include <time.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
#include "mipc.h"

const char* NAME = "mipc.txt";
extern char GRAPH_PATH[50];
extern char OUTPUT_FILE[30];

extern size_t period_T;
extern double A_END ;
extern double THETA ;
extern int TARGET_V ;

char get_s(Stage s);
void output_path(vector<vector<Path>>&  tmp_path_set);
void do_extend(vector<Path>& extend_path, vector<Path>& original_path, Path p);

int main(int argc, char** argv){
    Graph g;
    set_mipc_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);
    for(size_t j=0;j<g.V;j++){ // Init stage
        g.N[j]->stage = Stage::susceptible;
        double r = (rand() % 100)/100.0; //here: 0;
        double s_i = g.N[j]->params.relative * g.N[j]->params.contagion;
        if(r < s_i){
            g.N[j]->stage = Stage::infected;
        }
    }
    g.N[TARGET_V]->stage = Stage::infected; // To be removed;
    algo_mipc(g, TARGET_V, THETA);
}

void algo_mipc(Graph& g, int target_v, double theta){

    printf("Start Multi-hop infection Path Construction of node [%d].. \n\n", target_v);

    vector<vector<Path>> infection_path;
    vector<vector<Path>> tmp_path_set;
    vector<Path> single_path;
    vector<bool> visited(g.V, false);

    Path p;
    p.neighbor = target_v;
    p.path_prob = 1;
    visited[target_v] = true;
    p.visited = visited;
    p.neighbor_stage = g.N[target_v]->stage;

    single_path.push_back(p);
    tmp_path_set.push_back(single_path);

    for(size_t i=0;i<tmp_path_set.size();i++){
        if(tmp_path_set[i].size() == period_T)
            break;
        
        vector<Path> original_path = tmp_path_set[i];
        int end_id = original_path[original_path.size()-1].neighbor;
        struct node* end_node = g.N[end_id];
        Stage end_stage = original_path[original_path.size()-1].neighbor_stage;
        double end_prob = original_path[original_path.size()-1].path_prob;
        Path p;
        
        if(end_prob * A_END < theta)
            continue;
        switch (end_stage){
            case Stage::infected:
                if(end_id != target_v){
                    vector<Path> extend_path = original_path;
                    p.neighbor = end_id;
                    p.neighbor_stage = Stage::infected;
                    p.path_prob = end_prob * (1 - end_node->params.symptom - end_node->params.healing_fromI);
                    do_extend(extend_path, original_path, p);
                    tmp_path_set.push_back(extend_path);
                }

                for(size_t e=0;e < g.adj[end_id].size();e++){
                    struct node* u = g.N[g.adj[end_id][e].neighbor];
                    double edge_prob = g.get_edge_prob(g.N[target_v], u);

                    if(original_path[original_path.size()-1].visited[u->id])
                        continue;
                    for(int i=0;i<3;i++){
                        vector<Path> extend_path = original_path;
                        Path p ;
                        p.neighbor = u->id;
                        switch (i){
                            case 0:
                                p.path_prob = end_prob * edge_prob * end_node->params.relative * end_node->params.contagion;
                                p.neighbor_stage = Stage::infected;
                                break;
                            case 1:
                                p.path_prob = end_prob * edge_prob * end_node->params.contagion;
                                p.neighbor_stage = Stage::ailing;
                                break;
                            case 2:
                                p.path_prob = end_prob * edge_prob * end_node->params.contagion;
                                p.neighbor_stage = Stage::threatened;
                                break;
                            default:
                                break;
                        }
                        do_extend(extend_path, original_path, p);
                        extend_path[extend_path.size()-1].visited[u->id] = true;
                        tmp_path_set.push_back(extend_path);

                        // For debuggin the visited boolean table.
                        // cout<<"end: "<<end_id<<": ";
                        // for(int i=0;i < original_path[original_path.size()-1].visited.size();i++)
                        //     cout<<original_path[original_path.size()-1].visited[i]<<" ";
                        // cout<<endl; 
                    }
                }
                break;
            case Stage::ailing:
                for(int i=0;i<2;i++){
                    vector<Path> extend_path = original_path;
                    Path p;
                    p.neighbor = end_id;
                    switch (i){
                        case 0:
                            p.path_prob = end_prob * end_node->params.symptom;
                            p.neighbor_stage = Stage::infected;
                            break;
                        case 1:
                            p.path_prob = end_prob * (1 - end_node->params.critical - end_node->params.healing_fromA);
                            p.neighbor_stage = Stage::ailing;
                            break;
                        default:
                            break;
                    }
                    do_extend(extend_path, original_path, p);
                    tmp_path_set.push_back(extend_path);
                }
                break;
            case Stage::threatened:
                for(int i=0;i<2;i++){
                    vector<Path> extend_path = original_path;
                    Path p;
                    p.neighbor = end_id;
                    switch (i){
                        case 0:
                            p.path_prob = end_prob * end_node->params.critical;
                            p.neighbor_stage = Stage::ailing;
                            break;
                        case 1:
                            p.path_prob = end_prob * (1 - end_node->params.death - end_node->params.healing_fromT);
                            p.neighbor_stage = Stage::threatened;
                            break;
                        default:
                            break;
                    }
                    do_extend(extend_path, original_path, p);
                    tmp_path_set.push_back(extend_path);
                }
                break;
            default:
                continue;
        }
        if(end_stage != Stage::infected){
            tmp_path_set.erase(i + tmp_path_set.begin());
            i--;
        }else{
            original_path[original_path.size()-1].path_prob *= A_END;
        }
    }

    // check the len=T-1 path and remove it if the end.Stage is Infected;
    for(int k=tmp_path_set.size()-1;k>=0;k--){ 
        vector<Path> check_path = tmp_path_set[k];
        size_t len = check_path.size()-1;
        if(len != period_T-1)
            break;
        if(check_path[len].neighbor_stage != Stage::infected){
            tmp_path_set.erase(k + tmp_path_set.begin());
        }  
    }
    output_path(tmp_path_set);
}

void do_extend(vector<Path>& extend_path, vector<Path>& original_path, Path p){
    extend_path.push_back(p);
    extend_path[extend_path.size()-1].visited = original_path[original_path.size()-1].visited;
}

void output_path(vector<vector<Path>>&  tmp_path_set){
    size_t total_path_num = tmp_path_set.size();
    string line = "";
    FILE *fp = fopen(OUTPUT_FILE, "a");
    if (fp == NULL) {
        printf("Failed to open file %s.", OUTPUT_FILE);
        exit(EXIT_FAILURE);
    }
    for(size_t i=0;i<total_path_num;i++){
        fprintf(fp, "%-5ld :", i);
        if(tmp_path_set[i].size() == 0)
            continue;
        fprintf(fp, " %d(%c)", tmp_path_set[i][0].neighbor, get_s(tmp_path_set[i][0].neighbor_stage));
        for(size_t j=1;j<tmp_path_set[i].size();j++){
            fprintf(fp, " <-- %d(%c)", tmp_path_set[i][j].neighbor, get_s(tmp_path_set[i][j].neighbor_stage));
        }
        fprintf(fp, "\n\n");
    }
    fclose(fp);
}

char get_s(Stage s){
    char res;
    switch (s){
        case Stage::ailing:
            res = 'a';
            return res;
        case Stage::susceptible:
            res = 's';
            return res;
        case Stage::infected :
            res = 'i';
            return res;
        case Stage::recovered :
            res = 'r';
            return res;
        case Stage::dead :
            res = 'd';
            return res;
        case Stage::threatened :
            res = 't';
            return res;
        default:
            return '?';
            break;
    }
}
