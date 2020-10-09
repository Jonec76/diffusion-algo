#include <iostream>
#include <vector>
#include <stdlib.h>
#include <assert.h> 
#include "graph.h"
#include "diff_func.h"
using namespace std;

extern size_t sample_size, period_T;

double diffusion_full_result(vector<vector<struct X> > Strategy, Graph& g){
    long double f = 0;
    srand(time(0));
    double R0_num[period_T] = {0}, susceptible_num[period_T] = {0},  infected_num[period_T] = {0}, ailing_num[period_T] = {0}, threatened_num[period_T] = {0}, recovered_num[period_T] = {0}, dead_num[period_T] = {0};

    for(size_t i=0;i<sample_size;i++){
        vector<struct node*>susceptible, infected, ailing, threatened, recovered, dead;
        vector<vector<struct node*>*> total_group{&infected, &ailing, &threatened, &dead, &recovered}; 
        vector<vector<struct node*>*> all_group{&susceptible, &infected, &ailing, &threatened, &dead, &recovered}; 
        susceptible = g.N;
        double prev_I_num;
        
        for(size_t j=0;j<g.V;j++){ // Init stage
            g.N[j]->stage = Stage::susceptible;
            double r = (rand() % 100)/100.0; //here: 0;
            if(r < g.N[j]->a_v){
                g.N[j]->stage = Stage::infected;
                migrate(&susceptible, &infected, g.N[j]);
            }
        }
        vector<vector<struct node*>*> positive_group{&infected, &ailing, &threatened};
        vector<vector<struct node*>*> health_group{&susceptible, &infected, &recovered};
        
        assert(Strategy.size() == period_T);
        
        vector<struct node*> tmp_susceptible, tmp_infected, tmp_ailing, tmp_threatened, tmp_recovered, tmp_dead;
        vector<vector<struct node*>*> tmp_group{&tmp_infected, &tmp_ailing, &tmp_threatened, &tmp_dead, &tmp_recovered};// Shall align the order of total_group 
        prev_I_num = infected.size();
        for(size_t t=0;t<period_T;t++){// Quarantine
            g.set_node_lv(Strategy[t]);
            // ====check lv of overlap node (g.N[i]->q_level)===";
            for(size_t i=0;i<positive_group.size();i++){ // infected, ailing, threatened
                for(size_t j=0;j<positive_group[i]->size();j++){ // node of each group
                    struct node* positive_v = positive_group[i]->at(j);
                    infection_process(g, susceptible, tmp_infected, positive_v, Strategy[t]);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            f += objective_at_t(health_group, Strategy[t], g.V, g.N);
            double IAT_num = get_positive_count(positive_group);
            double new_I_num = infected.size()-prev_I_num;
            prev_I_num = infected.size();
            R0_num[t] += (new_I_num / IAT_num);
        }
    }

    // TODO: divide by sample size
    for(size_t t=0;t<period_T;t++){
        R0_num[t] /= sample_size;
        cout<<R0_num[t]<<endl;
    }

    return f/(double)sample_size;
}

double diffusion(vector<vector<struct X> > Strategy, Graph& g){
    long double f = 0;
    srand(time(0));
    for(size_t i=0;i<sample_size;i++){
        vector<struct node*>susceptible, infected, ailing, threatened, recovered, dead;
        vector<vector<struct node*>*> total_group{&infected, &ailing, &threatened, &dead, &recovered}; 
        vector<vector<struct node*>*> all_group{&susceptible, &infected, &ailing, &threatened, &dead, &recovered}; 
        susceptible = g.N;
        
        for(size_t j=0;j<g.V;j++){ // Init stage
            g.N[j]->stage = Stage::susceptible;
            double r = (rand() % 100)/100.0; //here: 0;
            if(r < g.N[j]->a_v){
                g.N[j]->stage = Stage::infected;
                migrate(&susceptible, &infected, g.N[j]);
            }
        }
        vector<vector<struct node*>*> positive_group{&infected, &ailing, &threatened};
        vector<vector<struct node*>*> health_group{&susceptible, &infected, &recovered};
        
        assert(Strategy.size() == period_T);
        
        vector<struct node*> tmp_susceptible, tmp_infected, tmp_ailing, tmp_threatened, tmp_recovered, tmp_dead;
        vector<vector<struct node*>*> tmp_group{&tmp_infected, &tmp_ailing, &tmp_threatened, &tmp_dead, &tmp_recovered};// Shall align the order of total_group 
        for(size_t t=0;t<period_T;t++){// Quarantine
            g.set_node_lv(Strategy[t]);
            // ====check lv of overlap node (g.N[i]->q_level)===";
            for(size_t i=0;i<positive_group.size();i++){ // infected, ailing, threatened
                for(size_t j=0;j<positive_group[i]->size();j++){ // node of each group
                    struct node* positive_v = positive_group[i]->at(j);
                    infection_process(g, susceptible, tmp_infected, positive_v, Strategy[t]);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            f += objective_at_t(health_group, Strategy[t], g.V, g.N);
        }
    }
    return f/(double)sample_size;
}

double diffusion_greedy(vector<vector<struct X> > Strategy, Graph& g){
    size_t f = 0;
    srand(time(0));
    for(size_t i=0;i<sample_size;i++){
        vector<struct node*>susceptible, infected, ailing, threatened, recovered, dead;
        vector<vector<struct node*>*> total_group{&infected, &ailing, &threatened, &dead, &recovered}; 
        vector<vector<struct node*>*> all_group{&susceptible, &infected, &ailing, &threatened, &dead, &recovered}; 
        susceptible = g.N;
        
        for(size_t j=0;j<g.V;j++){ // Init stage
            g.N[j]->stage = Stage::susceptible;
            double r = (rand() % 100)/100.0; //here: 0;
            if(r < g.N[j]->a_v){
                g.N[j]->stage = Stage::infected;
                migrate(&susceptible, &infected, g.N[j]);
            }
        }
        vector<vector<struct node*>*> positive_group{&infected, &ailing, &threatened};
        
        vector<struct node*> tmp_susceptible, tmp_infected, tmp_ailing, tmp_threatened, tmp_recovered, tmp_dead;
        vector<vector<struct node*>*> tmp_group{&tmp_infected, &tmp_ailing, &tmp_threatened, &tmp_dead, &tmp_recovered};// Shall align the order of total_group 
        
        assert(Strategy.size() == period_T);

        for(size_t t=0;t<period_T;t++){// Quarantine
            g.set_node_lv(Strategy[t]);
            // ====check lv of overlap node (g.N[i]->q_level)===";
            for(size_t i=0;i<positive_group.size();i++){ // infected, ailing, threatened
                for(size_t j=0;j<positive_group[i]->size();j++){ // node of each group
                    struct node* positive_v = positive_group[i]->at(j);
                    infection_process(g, susceptible, tmp_infected, positive_v, Strategy[t]);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            f += get_positive_count(positive_group);
        }
    }
    return f/(double)sample_size;
}