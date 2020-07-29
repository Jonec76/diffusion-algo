#include <vector>
#include <algorithm>
#include <iostream>
#include "data.h"

using namespace std;

string title[5] = {"infected", "ailing", "threatened", "dead", "recovered"} ;

void print_group(vector<vector<struct node*>*> v){
    for(int i=0;i<v.size();i++){
        cout<<title[i]<<endl;
        for(int j=0;j<v[i]->size();j++){
            cout<<v[i]->at(j)->id<<endl;
        }
    }
}

void printVec(vector<struct node>v){
    for(int i=0;i<v.size();i++){
        cout<<v[i].id<<endl;
    }
}

void addEdge(vector<struct edge> adj[], int u, int v, double p) {
    struct edge e1, e2;
    e1.neighbor = v;
    e1.p = p;
    e2.neighbor = u;
    e2.p = p;
    adj[u].push_back(e1); 
    adj[v].push_back(e2); 
}

double get_contagion(int type){
    switch (type){
        case 1:
            return 0.5;
            break;
        
        default:
            return (rand()%100)/100.0;
            break;
    }
}

void migrate(vector<struct node*>* from, vector<struct node*>* to, struct node* v){
    for(int i=0;i<from->size();i++){
        if(from->at(i)->id == v->id){
            from->erase(from->begin()+i);
            break;
        }
    }
    to->push_back(v);
}

// Index
//  0:infected
//  1:ailing
//  2:threatened
//  3:dead
//  4:recovered
void self_transmission_process(vector<vector<struct node*>*> from, vector<vector<struct node*>*> to, struct node* v){
    double r = (rand()%100)/100.0;
    switch (v->stage){
        case Stage::infected:
            if(r<v->params.symptom){
                v->stage = Stage::ailing;
                migrate(from[0], to[1], v);
            }else if(r < v->params.symptom + v->params.healing){
                v->stage = Stage::recovered;
                migrate(from[0], to[4], v);
            }
            break;
        case Stage::ailing:
            if(r<v->params.critical){
                v->stage = Stage::threatened;
                migrate(from[1], to[2], v);
            }else if(r < v->params.critical + v->params.healing){
                v->stage = Stage::recovered;
                migrate(from[1], to[4], v);
            }
            break;
        case Stage::threatened:
            if(r<v->params.death){
                v->stage = Stage::dead;
                migrate(from[2], to[3], v);
            }else if(r < v->params.death + v->params.healing){
                v->stage = Stage::recovered;
                migrate(from[2], to[4], v);
            }
            break;
        default:
            break;
    }
}

void tmp_push_back(vector<vector<struct node*>*>from , vector<vector<struct node*>*> to){
    for(int i=0;i<from.size();i++){
        for(int j=0;j<from[i]->size();j++){
            to[i]->push_back(from[i]->at(j));
        }
    }
}

// Only v.stage==I, A, T will enter into this function.
void infection_process(vector<struct edge>* e, vector<node*> N, vector<struct node*>* from, vector<struct node*>* to, struct node* v){
    
    // set quarantine state
    if(v->quarantine)return ;

    for(int i=0;i<e[v->id].size();i++){
        int neighbor_idx = e[v->id][i].neighbor;
        struct node* u = N[neighbor_idx];
        if(!u->quarantine && u->stage == Stage::susceptible){
            double p = e[neighbor_idx][v->id].p * u->params.contagion;
            double r = (rand()%100)/100.0;
            if(v->stage == Stage::infected)
                p *= u->params.relative;
            if(r < p){
            // if(true){
                u->stage = Stage::infected;
                migrate(from, to, u);
            }
        }
    }
    // modify stage

    // undo quarantine state
}





