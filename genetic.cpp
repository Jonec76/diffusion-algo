#include <stdio.h>
#include <string.h>
#include <time.h>
#include <vector>
#include <assert.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"

using namespace std;

const char* NAME = "genetic.txt";
extern int sample_size, period_T;
extern double budget;
extern char GRAPH_PATH[50];
extern char OUTPUT_PATH[30];
extern vector<struct el> level_table;
clock_t total_start, total_end;
vector<vector<struct X> > algo_genetic(Graph& g);
const int cost_table[4] = {0, 1, 2, 3};


int main(int argc, char **argv){
    Graph g;
    total_start = clock();
    if(argc != 2){
        printf("Wrong argument. Execution format: ./genetic config.txt\n");
        return 0;
    }
    set_config(argv[1], NAME);
    create_graph(g, GRAPH_PATH);
    algo_genetic(g);
    total_end = clock();
}

void print_vector(vector<vector<int>> population){
    for(size_t i=0;i<population.size();i++){
        for(size_t j=0;j<population[i].size();j++){
            printf("%d", population[i][j]);
        }
        puts("\n");
    }
}

void get_gene(vector<int>& gene, int g_range, int g_length, double budget){
    double total_cost = 0;
    for (int j = 0; j < g_length; j++){
        int lv = (rand() % g_range);
        double new_cost= level_table[lv].phi_cost * cost_table[lv];
        if(total_cost+new_cost > budget)
            lv = 0;
        else
            total_cost += new_cost;
        gene.push_back(lv);
    } 
}

vector<vector<int>> init_population(int budget, int g_range, int g_length, int population_num){
    srand((unsigned) time(0));
    vector<vector<int>> population;
    for (int i = 0; i < population_num; i++) {
        vector<int> gene;
        get_gene(gene, g_range, g_length, budget);
        population.push_back(gene);
    }
    return population;
}

void compute_gene_score(double **gene_score, vector<vector<int>> population, Graph& g){
    int eta = -1;

    for(size_t i=0;i<population.size();i++){
        vector<int> gene = population[i];
        double gene_cost = 0.0;
        for(size_t t=0;t<population[i].size();t++){
            int lv = population[i][t];
            g.U[t][0].t = t;
            g.U[t][0].cost = cost_table[lv];
            g.U[t][0].lv = lv;
            g.U[t][0].eta = eta;
            g.U[t][0].id = t;
        }
        (*gene_score)[i] = diffusion(g.U, g);
    }
    return;
}   

void selection(int** candidates, double** gene_score, int population_num){
    double largest = (*gene_score)[0];
    double largest1 = (*gene_score)[1];
    double temp;

    (*candidates)[0] = 0;
    (*candidates)[1] = 1;

    if (largest < largest1){
        temp = largest;
        largest = largest1;
        largest1 = temp;
        (*candidates)[0] = 1;
        (*candidates)[1] = 0;
    }
 
    for (int i = 2; i < population_num; i++){
        if ((*gene_score)[i] > largest){
            largest1 = largest;
            largest = (*gene_score)[i];
            (*candidates)[1] = (*candidates)[0];
            (*candidates)[0] = i;
        }
        else if ((*gene_score)[i] > largest1 && (*gene_score)[i] != largest){
            largest1 = (*gene_score)[i];
            (*candidates)[1] = i;
        }
    }
    printf("largest: %f, second largest: %f\n", largest, largest1);
}

void crossover(vector<vector<int>>& population, int* candidates){
    int largest = candidates[0];
    int largest1 = candidates[1];
    int cross_point = 1 + (rand() % period_T);

    for(int i=0;i<cross_point;i++){
        int tmp = population[largest][i];
        population[largest][i] = population[largest1][i];
        population[largest1][i] = tmp;
    }
}

void mutation(vector<vector<int>>& population, double m_probability, double budget, int g_range){
    srand(time(0));
    for(size_t i=0;i<population.size();i++){
        double total_cost = 0;
        for(size_t j=0;j<population[i].size();j++){
            double r = (rand() % 100)/100.0; 
            if(r < m_probability){
                int lv = (rand() % g_range);
                double new_cost = level_table[lv].phi_cost * cost_table[lv];

                if(total_cost+new_cost > budget){
                    lv = 0;
                }
                else
                    total_cost += new_cost;

                population[i][j] = lv;
            }else{
                int lv = population[i][j];
                double cost = level_table[lv].phi_cost * cost_table[lv];
                
                if(total_cost+cost > budget){
                    population[i][j] = 0;
                }
                else
                    total_cost += cost;
            }
        }
    }

    // for double check the cost of gene is less than budget.
    for(size_t i=0;i<population.size();i++){
        double total_cost = 0;
        for(size_t j=0;j<population[i].size();j++){
            int lv = population[i][j];
            double cost = level_table[lv].phi_cost * cost_table[lv];
            if(total_cost + cost > budget){
                population[i][j] = 0;
                puts("wrong mutation algo\n");
            }
            else
                total_cost += cost;
        }
    }
}

vector<vector<struct X> > algo_genetic(Graph& g){
    int budget = 10;
    int g_range = 4;
    int population_num = 5;
    int eta = -1;
    int epoch = 50;
    double m_probability = 0.3;
    /////////////////

    assert(population_num >= 2);
    init_strategy_U_gene(g);

    printf("Start genetic algorithm ..\n\n");
    vector<vector<struct X> > S;
    init_strategy(S);

    // init population
    vector<vector<int>> population;

    population = init_population(budget, g_range, period_T, population_num);

    // compute fitness score
    double* gene_score = (double*)malloc(population_num *sizeof(double));
    if(gene_score == NULL){
        printf("error malloc\n");
        return S;
    }
    for(int i=0;i<population_num;i++)
        gene_score[i] = 0.0;
    

    compute_gene_score(&gene_score, population, g);
    // evolve loop
    int* candidates = (int*)malloc(2*sizeof(int)); // For pick largest two numbers index.

    for(int i=0;i<epoch;i++){
        selection(&candidates, &gene_score, population_num);
        crossover(population, candidates);
        mutation(population, m_probability, budget, g_range);
        compute_gene_score(&gene_score, population, g);
    }
    return S;
}

