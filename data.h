#ifndef DATA_H
#define DATA_H

#define w_S 1
#define w_I 1
#define w_R 1

#define q_free 0
#define q_low 1
#define q_mid 2
#define q_high 3

#define health_s 0
#define health_i 1
#define health_r 2

#define eta_default -1

#define out_of_cost -2
#define out_of_budget true
#define has_in_set -1

#include<iostream>
#include<vector>
using namespace std;


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
        double relative;
        double contagion;
        double symptom;
        double critical;
        double healing_fromI;
        double healing_fromA;
        double healing_fromT;
        double death;
    }params;
    int q_level; // TODO
    double a_v;
    int type;
};

// Group
struct X{
    int id;
    double cost;
    vector<int>D; // Should be sorted during init level.
    int t;
    int lv;
    double eta; // -1 for not given eta value
};
#endif
