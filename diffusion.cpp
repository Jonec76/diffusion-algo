#include <iostream>
#include <vector>
#include <stdlib.h>
#include "data.h"
#include "diff_func.h"
using namespace std;

double diffusion(vector<vector<struct X>> Strategy, int T, int sample_size){
    long long f = 0;

    // Init each node
    for(int i=0;i<V;i++){ 
        struct node* tmp = (struct node*)malloc(sizeof(struct node));
        int type=0;
        // tmp->stage = (Stage*)malloc(T+1 * sizeof(enum Stage));
        tmp->id = i+1;
        tmp->params.contagion = get_contagion(type);
        tmp->q_state = q_free;
        N.push_back(tmp);
    }

    /// Add edge
    addEdge(adj, 0, 2, 0.87); 
    addEdge(adj, 1, 7, 0.88); 

    for(int i=0;i<sample_size;i++){
        cout<<"sample_size:"<<i<<endl;
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
    
        for(int t=1;t<=T;t++){// Quarantine
            vector<struct node*> tmp_susceptible, tmp_infected, tmp_ailing, tmp_threatened, tmp_recovered, tmp_dead;
            vector<vector<struct node*>*> tmp_group{&tmp_infected, &tmp_ailing, &tmp_threatened, &tmp_dead, &tmp_recovered};// Shall align the order of total_group 
            vector<vector<struct node*>*> positive_group{&infected, &ailing, &threatened};
            
            vector<vector<struct node*>*> health_group{&susceptible, &infected, &recovered};
            health_group[health_s] = &susceptible;
            health_group[health_i] = &infected;
            health_group[health_r] = &recovered;

            for(int i=0;i<positive_group.size();i++){ // infected, ailing, threatened
                for(int j=0;j<positive_group[i]->size();j++){ // node of each group
                    struct node* positive_v = positive_group[i]->at(j);
                    infection_process(adj, N, &susceptible, &tmp_infected, positive_v, Strategy[t]);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            // print_group(total_group);
            f += objective_at_t(health_group, Strategy[t]);
        }
    }
    return f/(double)sample_size;
}

