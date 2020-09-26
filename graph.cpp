#include "graph.h"

Graph::Graph(){
    
}

Graph::~Graph(){
    for(size_t i=0;i<N.size();i++){
        free(N[i]);
    }
}

void Graph::init_graph(int V_, int E_, int U_LENGTH_){
    V = V_;
    E = E_;
    U_LENGTH = U_LENGTH_;
    // Init each node
    for(size_t i=0;i<V;i++){ 
        vector<struct edge> tmp_e;
        adj.push_back(tmp_e);
    }
    // Init U
    for(size_t i=0;i<period_T;i++){
        vector<struct X> tmp;
        U.push_back(tmp);
    }
    // init type  => contagion
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

void Graph::push_U(struct X x){
    U[x.t].push_back(x);
}

void Graph::set_node_lv(vector<struct X> x_t){
    for(size_t i=0;i<V;i++){
        N[i]->q_level = q_free;
    }
    for(size_t i=0;i<x_t.size();i++){
        for(size_t n=0;n<x_t[i].D.size();n++){
            if(x_t[i].lv > N[x_t[i].D[n]]->q_level){
                N[x_t[i].D[n]]->q_level = x_t[i].lv;
            }
        }
    }
}