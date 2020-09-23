#include "graph.h"

Graph::Graph(int V_, int E_){
    V = V_;
    E = E_;

    // Init each node
    for(int i=0;i<V;i++){ 
        struct node* tmp = (struct node*)malloc(sizeof(struct node));
        // int type=0;
        tmp->id = i;
        // tmp->params.contagion = get_contagion(type);
        tmp->params.relative = 1;
        tmp->params.contagion = 0.5;
        tmp->q_level = q_free;
        vector<struct edge> tmp_e;
        adj.push_back(tmp_e);
        N.push_back(tmp);
    }
    // init type  => contagion
}

void Graph::free_N(){
    for(int i=0;i<N.size();i++){
        free(N[i]);
    }
}

void Graph::addEdge(int u, int v, double p) {
    struct edge e1, e2;
    e1.neighbor = v;
    e1.p = p;
    e2.neighbor = u;
    e2.p = p;
    adj[u].push_back(e1);
    adj[v].push_back(e2);
}

double Graph::get_contagion(int type){
    return 0;
}

void Graph::print_node(){
    for(size_t i=0;i<N.size();i++){
        switch (N[i]->stage){
            case Stage::ailing:
                cout<<"ailing\n";
                break;
            case Stage::susceptible:
                cout<<"susceptible\n";
                break;
            case Stage::infected :
                cout<<"infected\n";
                break;
            case Stage::recovered :
                cout<<"recovered\n";
                break;
            case Stage::dead :
                cout<<"dead\n";
                break;
            case Stage::threatened :
                cout<<"threatened\n";
                break;
        default:
            break;
        }
    }
}

double Graph::get_edge_prob(struct node* u, struct node* v){
    for(size_t i=0;i< adj[u->id].size();i++){
        if(adj[u->id][i].neighbor == v->id){
            return adj[u->id][i].p;
        }
    }
    return 0;
}