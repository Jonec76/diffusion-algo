#include <math.h>
#include <algorithm>
#include <bits/stdc++.h> 
#include <iostream>
#include <vector>
#include <time.h>
#include "data.h"
#include "graph.h"

using namespace std;

string title[6] = {"susceptible", "infected", "ailing", "threatened", "dead", "recovered"};
double alpha_f=0.944, alpha_g=0.5, param_c=2; 
extern vector<struct el> level_table;

void print_group(vector<vector<struct node*>*> v) {
    cout<<"\n";
    for (size_t i = 0; i < v.size(); i++) {
        cout << title[i] <<":";
        for (size_t j = 0; j < v[i]->size(); j++) {
            cout << v[i]->at(j)->id <<" ";
        }
        cout<<"\n";
    }
}

void printVec(vector<struct node> v) {
    for (size_t i = 0; i < v.size(); i++) {
        cout << v[i].id << endl;
    }
}

double get_contagion(int type) {
    switch (type) {
        case 1:
            return 0.5;
            break;

        default:
            return (rand() % 100) / 100.0;
            break;
    }
}

void migrate(vector<struct node*>* from, vector<struct node*>* to, struct node* v) {
    for (size_t i = 0; i < from->size(); i++) {
        if (from->at(i)->id == v->id) {
            from->erase(from->begin() + i);
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
void self_transmission_process(vector<vector<struct node*>*> from, vector<vector<struct node*>*> to, struct node* v) {
    double r = (rand() % 100) / 100.0;
    switch (v->stage) { // TODO: init it
        case Stage::infected:
            if (r < v->params.symptom) {
                v->stage = Stage::ailing;
                migrate(from[0], to[1], v);
            } else if (r < v->params.symptom + v->params.healing_fromI) {
                v->stage = Stage::recovered;
                migrate(from[0], to[4], v);
            }
            break;
        case Stage::ailing:
            if (r < v->params.critical) {
                v->stage = Stage::threatened;
                migrate(from[1], to[2], v);
            } else if (r < v->params.critical + v->params.healing_fromA) {
                v->stage = Stage::recovered;
                migrate(from[1], to[4], v);
            }
            break;
        case Stage::threatened:
            if (r < v->params.death) {
                v->stage = Stage::dead;
                migrate(from[2], to[3], v);
            } else if (r < v->params.death + v->params.healing_fromT) {
                v->stage = Stage::recovered;
                migrate(from[2], to[4], v);
            }
            break;
        default:
            break;
    }
}

void tmp_push_back(vector<vector<struct node*>*> from, vector<vector<struct node*>*> to) {
    for (size_t i = 0; i < from.size(); i++) {
        for (size_t j = 0; j < from[i]->size(); j++) {
            to[i]->push_back(from[i]->at(j));
        }
        from[i]->clear();
    }
}

bool remove_edge(struct node* v, struct node* u){
    // printf("v: %d, neighbor_u: %d", v->id, u->id);
    int q_max_level = max(v->q_level, u->q_level);
    double remove_p = level_table[q_max_level].remove_p;
    double r = (rand() % 100)/100.0;
    // cout<<"remove_edge_p: "<<remove_p<<" < "<<r<<endl;
    if(remove_p < r)
        return false;
    else
        return true;
}

// Only v.stage==I, A, T will enter into this function.
void infection_process(Graph& g, vector<struct node*>& from, vector<struct node*>& to, struct node* v, vector<struct X> X_t) {
    // set quarantine state
    for(size_t i=0;i<g.adj[v->id].size();i++){
        struct node* u = g.N[g.adj[v->id][i].neighbor]; // the neighbor of V
        if(u->stage == Stage::susceptible){
            if(!remove_edge(v, u)){
                double p = g.get_edge_prob(u, v) * u->params.contagion;
                double r = (rand() % 100) / 100.0;
                if (v->stage== Stage::infected)
                    p *= u->params.relative;
                if (r < p) {
                    u->stage = Stage::infected;
                    migrate(&from, &to, u);
                }
            }
        }
    }
    // modify stage
    // undo quarantine state
}

double q_t(vector<struct X> X_t, int V, vector<struct node*>& N) {
    int q_node;

    int s_ctr_xt=0, i_ctr_xt=0, r_ctr_xt=0;
    bool choose_table[V];
    memset(choose_table, false, V * sizeof(bool)); // for clearing previous record
    for(size_t i=0;i<X_t.size();i++){ // each X
        for(size_t j=0;j<X_t[i].D.size();j++){ // each node of X.D
            q_node = X_t[i].D[j];
            // printf(choose_table[q_node] ? "true\n" : "false\n");
            // printf("q: %d\n", q_node);
            if(!choose_table[q_node]){
                choose_table[q_node] = true;
                switch (N[q_node]->stage){
                    case Stage::susceptible:
                        choose_table[q_node] = true;
                        s_ctr_xt++;
                        break;
                    case Stage::infected:
                        choose_table[q_node] = true;
                        i_ctr_xt++;
                        break;
                    case Stage::recovered:
                        choose_table[q_node] = true;
                        r_ctr_xt++;
                        break;
                    default:
                        break;
                } 
            }
        }
    }
    // printf("s:%d, i:%d, r:%d\n", s_ctr_xt, i_ctr_xt, r_ctr_xt);
    return (w_S * s_ctr_xt) + (w_I * i_ctr_xt) + (w_R * r_ctr_xt);
}

double M_t(vector<struct X> X_t) {
    double sum = 0;
    for (size_t i = 0; i < X_t.size(); i++) {
        sum += (X_t[i].eta == eta_default? 2.0 / (double)X_t[i].D.size():X_t[i].eta);  // Should set eta value as 2/|D| when there is no default value.
    }
    return sum - sqrt(sum);
}

double f_t(vector<vector<struct node*>*> health_group) {
    int s_ctr = health_group[health_s]->size(), i_ctr = health_group[health_i]->size(), r_ctr = health_group[health_r]->size();
    return (w_S * s_ctr) + (w_I * i_ctr) + (w_R * r_ctr);
}

double objective_at_t(vector<vector<struct node*>*> health_group, vector<struct X> X_t, int v, vector<struct node*>& N) {
    double q = 1 + q_t(X_t, v, N);
    double t = 1/param_c;
    return alpha_f * f_t(health_group) - alpha_g * M_t(X_t) * pow(q, t);
}

