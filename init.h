#ifndef INIT_H
#define INIT_H
#include "graph.h"

const size_t n_size = 11;
const size_t g_size = 3;
const size_t e_size = 3;
const size_t x_size = 5;
const size_t c_size = 4;
const size_t p_size = 8;

void set_mipc_config(char* argv, const char* file_name);
void set_config(char* argv, const char* file_name);
void init_node(Graph& g, vector<char*>& input_line);
void init_edge(Graph& g, vector<char*>& input_line);
void init_strategy_U(Graph& g, vector<char*>& input_line);
void get_split_data(vector<char*>& input_line, char* data, const char data_delim[]);
void create_graph(Graph &g, const char* GRAPH_FILE);
void create_quarantine_strategy(Graph& g, const char* GROUP_FILE);
void init_strategy_one_dim_id(vector<vector<struct X>>& U);

#endif