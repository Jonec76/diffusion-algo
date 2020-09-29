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
extern size_t period_T;
double A_END = 0.8;

int main(int argc, char** argv){
    Graph g;
    set_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);
    for(size_t j=0;j<g.V;j++){ // Init stage
        g.N[j]->stage = Stage::susceptible;
        double r = (rand() % 100)/100.0; //here: 0;
        double s_i = g.N[j]->params.relative * g.N[j]->params.contagion;
        if(r < s_i){
            g.N[j]->stage = Stage::infected;
        }
    }
    g.print_node();
    algo_mipc(g);
}

void algo_mipc(Graph& g){
    vector<vector<Path>> infection_path;
    vector<vector<Path>> tmp_path_set;
    vector<Path> single_path;
    int target_id = 0;

    bool visited [g.V]; 
    memset(visited, false, sizeof(bool) * g.V);
    visited[target_id] = true;

    
    Path p;
    // p.curr_node = target_id;
    p.neighbor = target_id;
    p.path_prob = 1;
    p.visited = visited;
    p.neighbor_stage = g.N[target_id]->stage;
    
    single_path.push_back(p);
    tmp_path_set.push_back(single_path);

    for(size_t t=1;t<=period_T;t++){
        for(size_t i=0;i<tmp_path_set.size();i++){
            if(tmp_path_set[i].size() > t)
                break;
            if(tmp_path_set[i].size() != t)
                continue;
            vector<Path> original_path = tmp_path_set[i];

            int end_id = original_path[original_path.size()-1].neighbor;
            struct node* end_node = g.N[end_id];
            Stage end_stage = original_path[original_path.size()-1].neighbor_stage;
            double end_prob = original_path[original_path.size()-1].path_prob;

            switch (end_stage){
                case Stage::infected:
                    cout<<end_id;
                    if(end_id != target_id){
                        vector<Path> extend_path = original_path;
                        Path p;
                        p.neighbor = end_id;
                        p.neighbor_stage = Stage::infected;
                        p.path_prob *= (1 - end_node->params.symptom - end_node->params.healing_fromI);
                        p.visited[end_id] = true;
                        extend_path.push_back(p);
                        tmp_path_set.push_back(extend_path);
                    }
                    for(size_t e=0;e < g.adj[end_id].size();e++){
                        // if(g.adj[end_id][e] exist in visited)
                        //     continue;
                        struct node* u = g.N[g.adj[end_id][e].neighbor];
                        double edge_prob = g.get_edge_prob(g.N[target_id], u);
                        bool* end_visited = original_path[original_path.size()-1].visited;

                        if(end_visited[u->id])
                            continue;
                        end_visited[u->id] = true;
                        for(int i=0;i<3;i++){
                            vector<Path> extend_path = original_path;
                            Path p;
                            p.neighbor = u->id;
                            p.visited[u->id] = true;
                            switch (i){
                                case 0:
                                    p.path_prob *= edge_prob * end_node->params.relative * end_node->params.contagion;
                                    p.neighbor_stage = Stage::infected;
                                    break;
                                case 1:
                                    p.path_prob *= edge_prob * end_node->params.contagion;
                                    p.neighbor_stage = Stage::ailing;
                                    break;
                                case 2:
                                    p.path_prob *= edge_prob * end_node->params.contagion;
                                    p.neighbor_stage = Stage::threatened;
                                    break;
                                default:
                                    break;
                            }
                            extend_path.push_back(p);
                            tmp_path_set.push_back(extend_path);
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
                                p.path_prob *= end_node->params.symptom;
                                p.neighbor_stage = Stage::infected;
                                break;
                            case 1:
                                p.path_prob *= (1 - end_node->params.critical - end_node->params.healing_fromA);
                                p.neighbor_stage = Stage::ailing;
                                break;
                            default:
                                break;
                        }
                        extend_path.push_back(p);
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
                                p.path_prob *= end_node->params.critical;
                                p.neighbor_stage = Stage::ailing;
                                break;
                            case 1:
                                p.path_prob *= (1 - end_node->params.death - end_node->params.healing_fromT);
                                p.neighbor_stage = Stage::threatened;
                                break;
                            default:
                                break;
                        }
                        extend_path.push_back(p);
                        tmp_path_set.push_back(extend_path);
                    }
                    break;
                default:
                    continue;
            }

            for(size_t k=0;k<tmp_path_set.size();k++){
                for(size_t j=0;j<tmp_path_set[k].size();j++){
                    cout<<tmp_path_set[k][j].neighbor<<" ";
                }
                cout<<endl;
            }

            // if(end_stage != Stage::infected || end_prob * A_END){
            //     tmp_path_set.erase(i + tmp_path_set.begin());
            //     i--;
            // }else{
            //     original_path[original_path.size()-1].path_prob *= A_END;
            // }
        }
    }
}

