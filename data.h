#ifndef DATA_H
#define DATA_H

#define w_S 0.4
#define w_I 0.5
#define w_R 0.7

#define q_free 0
#define q_low 1
#define q_mid 2
#define q_high 3

#define health_s 0
#define health_i 1
#define health_r 2

#include<iostream>
#include<vector>
using namespace std;

extern double alpha_f = 0.2;
extern double alpha_g = 0.7;
extern int V=3, E=2, T=1;
extern vector<struct node*> N;
extern vector<struct edge> adj[V]; 
extern int param_c;

enum Stage{
    susceptible, 
    infected,
    ailing,
    threatened,
    dead,
    recovered 
};

struct el{
    double remove_p;
    double phi_cost;
};


struct edge{
    int neighbor;
    double p;
};

struct node{
    int id;
    Stage stage;
    struct Params{
        double relative=1;
        double contagion=0;
        double symptom=0;
        double critical=0;
        double healing=0;
        double death=0;
    }params; 
    bool quarantine=false;
    int q_state;
};

// Group
struct X{
    vector<int>D; // Should be sorted during init level.
    int t;
    int lv;
    double eta; // -1 for not given eta value
};


extern vector<struct X> U;

#endif
