#include <iostream>
#include <vector>
#include <stdlib.h>
#include "data.h"
#include "func.h"
using namespace std;

int main(){
    int RUN=1, V=3, E=2, T=1; 
    vector<struct edge> adj[V]; 
    vector<struct node*> N; 
    for(int i=0;i<V;i++){ // Init each node
        struct node* tmp = (struct node*)malloc(sizeof(struct node));
        int type=0;

        tmp->id = i;
        tmp->params.contagion = get_contagion(type);

        N.push_back(tmp);
    }
    struct edge e;
    /// Add edge
    addEdge(adj, 0, 2, 0.87); 
    addEdge(adj, 1, 7, 0.88); 

    for(int i=0;i<RUN;i++){
        cout<<"RUN:"<<i<<endl;
        vector<struct node*>susceptible, infected, ailing, threatened, recovered, dead;
        vector<vector<struct node*>*> total_group{&infected, &ailing, &threatened, &dead, &recovered}; 
        susceptible = N;
        
        for(int j=0;j<V;j++){ // Init stage
            N[j]->stage = Stage::susceptible;
            double r = 0; //here: (rand() % 100)/100.0;
            double s_i = N[j]->params.relative * N[j]->params.contagion;
            if(r < s_i){
                N[j]->stage = Stage::infected;
                migrate(&susceptible, &infected, N[j]);
            }
        }
    
        for(int t=0;t<T;t++){// Quarantine
            vector<struct node*> tmp_susceptible, tmp_infected, tmp_ailing, tmp_threatened, tmp_recovered, tmp_dead;
            vector<vector<struct node*>*> tmp_group{&tmp_infected, &tmp_ailing, &tmp_threatened, &tmp_dead, &tmp_recovered};// Shall align the order of total_group 
            vector<vector<struct node*>*> positive_group{&infected, &ailing, &threatened};

            for(int i=0;i<positive_group.size();i++){ // infected, ailing, threatened
                for(int j=0;j<positive_group[i]->size();j++){ // node of each group
                    struct node* positive_v = positive_group[i]->at(j);
                    infection_process(adj, N, &susceptible, &tmp_infected, positive_v);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            print_group(total_group);
            // objective_at_t();
        }
    }
}

