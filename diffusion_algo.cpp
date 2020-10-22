#include <iostream>
#include <vector>
#include <stdlib.h>
#include <assert.h> 
#include "graph.h"
#include "diff_func.h"
using namespace std;

extern size_t sample_size, period_T;
extern char OUTPUT_PATH[30];

double diffusion_full_result(vector<vector<struct X> > Strategy, Graph& g){
    long double f = 0;
    srand(time(0));
    double R0_num[period_T] = {0}, quan_infect_rate[period_T] = {0}, susceptible_num[period_T] = {0},  infected_num[period_T] = {0}, ailing_num[period_T] = {0}, threatened_num[period_T] = {0}, recovered_num[period_T] = {0}, dead_num[period_T] = {0};

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
            // 2)
            double IAT_num = get_positive_count(positive_group);
            double new_I_num = infected.size()-prev_I_num;
            prev_I_num = infected.size();
            R0_num[t] += (new_I_num / IAT_num);
            
            // 3)
            susceptible_num[t] += susceptible.size();
            infected_num[t] += infected.size();
            ailing_num[t] += ailing.size();
            threatened_num[t] += threatened.size();
            dead_num[t] += dead.size();
            recovered_num[t] += recovered.size();

            // 4)
            get_quarantine_infect_rate(&quan_infect_rate[t], g, t);
        }
    }

    FILE * pFile;
    pFile = fopen (OUTPUT_PATH, "a");
    if (pFile == NULL) {
        printf("Failed to open file %s.", OUTPUT_PATH);
        exit(EXIT_FAILURE);
    }

    // TODO: divide by sample size
    for(size_t t=0;t<period_T;t++){
        R0_num[t] /= sample_size;
        susceptible_num[t] /= sample_size;
        infected_num[t] /= sample_size;
        ailing_num[t] /= sample_size;
        threatened_num[t] /= sample_size;
        recovered_num[t] /= sample_size;
        dead_num[t] /= sample_size;
        quan_infect_rate[t] /= sample_size;
        fprintf(pFile, "\n[t: %ld] \n", t);
        fprintf (pFile, "%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n%-15s :%f\n","R0 ", R0_num[t], "susceptible ",  susceptible_num[t],"infected ",  infected_num[t],"ailing ",  ailing_num[t],"threatened ",  threatened_num[t],"recovered ",  recovered_num[t],"dead ",  dead_num[t], "隔離區的平均感染率 ", quan_infect_rate[t]);
    }
    fclose(pFile);

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

// Do diffusion with the specific level table (Opt uses only 2 levels table)
double diffusion(vector<vector<struct X> > Strategy, Graph& g, vector<struct el> level_table_opt){
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
                    infection_process(g, susceptible, tmp_infected, positive_v, Strategy[t], level_table_opt);
                    self_transmission_process(positive_group, tmp_group, positive_v);
                }
            }
            tmp_push_back(tmp_group, total_group);
            f += objective_at_t(health_group, Strategy[t], g.V, g.N);
        }
    }
    return f/(double)sample_size;
}

// Do diffusion, calculate f with `get_positive_count` instead of `objective_at_t`
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