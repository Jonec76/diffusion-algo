#ifndef SAS_H
#define SAS_H

#include<vector>
using namespace std;
bool check_exist(struct X x, vector<struct X> check_list);
void SAS(vector<vector<struct X> >&S, int ra, Graph& g, double max_F);
void backward_rescheduling(vector<struct X>& Z, vector<vector<struct X> >&S, size_t t, Graph& g);
void forward_rescheduling(vector<struct X>& Y, vector<vector<struct X> >&S, size_t t, Graph& g);
bool rescheduling(vector<struct X> Y, vector<struct X> Z, int ra, Graph& g, vector<vector<struct X> >&S);

#endif