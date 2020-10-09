#ifndef INIT_H
#define INIT_H
#include "graph.h"

void set_mipc_config(char* argv, const char* file_name);
void set_config(char* argv, const char* file_name);
void init_node(Graph& g, vector<char*>& input_line);
void init_edge(Graph& g, vector<char*>& input_line);
void init_strategy_U(Graph& g, vector<char*>& input_line);
void get_split_data(vector<char*>& input_line, char* data, char data_delim[]);
void create_graph(Graph &g, const char* GRAPH_FILE);
void create_quarantine_strategy(Graph& g, const char* GROUP_FILE);
void init_strategy_one_dim_id(vector<vector<struct X>>& U);

#endif