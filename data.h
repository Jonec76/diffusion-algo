#ifndef DATA_H
#define DATA_H

#include<iostream>
using namespace std;

enum Stage{
    susceptible, 
    infected,
    ailing,
    threatened,
    dead,
    recovered 
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
};
#endif
