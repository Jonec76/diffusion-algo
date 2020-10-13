#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include "diff_func.h"
#include "algo.h"
#include "graph.h"
#include "init.h"
#include "mipc.h"

using namespace std;
extern size_t period_T;
extern char OUTPUT_FILE[30];

void remove_yz(vector<vector<struct X> >& s, vector<struct X> Y, vector<struct X> Z){
    for(size_t i=0;i<Y.size();i++){
        int t = Y[i].t;
        int id = Y[i].id;
        for(size_t j=0;j<s[t].size();j++){
            if(s[t][j].id == id){
                s[t].erase(s[t].begin() + j);
                break;
            }
        }
    }

    for(size_t i=0;i<Z.size();i++){
        int t = Z[i].t;
        int id = Z[i].id;
        for(size_t j=0;j<s[t].size();j++){
            if(s[t][j].id == id){
                s[t].erase(s[t].begin() + j);
                break;
            }
        }
    }
}

void migrate_x(vector<vector<struct X> >& tmp_S, int m_bit, vector<struct X> X_list, int migrate_day){
    for(size_t i=0;i<X_list.size();i++){
        struct X migrate_X = X_list[i];
        int m_t = migrate_X.t;
        bool remove = m_bit & 1;
        if(remove){
            assert(m_t + migrate_day >= 0);
            assert(m_t + migrate_day < (int)tmp_S.size());
            migrate_X.t = m_t + migrate_day;
            tmp_S[m_t + migrate_day].push_back(migrate_X);
        }else{
            tmp_S[m_t].push_back(migrate_X);
        }
        m_bit >>= 1;
    }
}


// what if Y or Z size equals to 0?
bool rescheduling(vector<struct X> Y, vector<struct X> Z, int ra, Graph& g, vector<vector<struct X> >&S, double max_F){
    bool flag = false;
    size_t y_size = Y.size();
    size_t z_size = Z.size();

    cout<<"[ rescheduling ] Y size: "<<y_size<<" Z size: "<<z_size<<endl;
    if(y_size == 0 && z_size == 0)
        return false;

    if(y_size > 15)
        y_size = 15;
    if(z_size > 15)
        z_size = 15;

    unsigned int pow_y = 1 << y_size;
    unsigned int pow_z = 1 << z_size;

    vector< pair <int,int> > sets; 
    
    int set_ctr = 0;

    // set size should smaller(equal) than ra
    // Add all combinations if it does, or break it.
    bool stop_append = false;
    for(unsigned int i=0;i<pow_y && !stop_append;i++){
        for(unsigned int j=0;j<pow_z;j++){
            if(i==0 && j == 0){
                set_ctr++;
                continue;
            }
            if(set_ctr <= ra){
                pair<unsigned int, unsigned int> p;
                p.first = i;
                p.second = j;
                sets.push_back(p);
            }else{
                stop_append = true;
                break;
            }
            set_ctr++;
        }
    }

    for(size_t i=0;i<sets.size();i++){
        vector<vector<struct X> >tmp_S = S;
        remove_yz(tmp_S, Y, Z);
        migrate_x(tmp_S, sets[i].first, Y, -1);
        migrate_x(tmp_S, sets[i].second, Z, 1);

        double migrate_F = diffusion(tmp_S, g);
        printf ("\n%-15s :%f\n%-15s :%f\n\n","Scheduled F ", migrate_F, "Original F ",  max_F);
        if(migrate_F > max_F){
            FILE * pFile;
            pFile = fopen (OUTPUT_FILE, "a");
            if (pFile == NULL) {
                printf("Failed to open file %s.", OUTPUT_FILE);
                exit(EXIT_FAILURE);
            }

            fprintf (pFile, "After rescheduling:   \n");
            for(size_t i=0;i<tmp_S.size();i++){
                for(size_t j=0;j<tmp_S[i].size();j++)
                    fprintf (pFile, "         %d_%d_%d\n",tmp_S[i][j].t, tmp_S[i][j].id, tmp_S[i][j].one_dim_id);
            }
                // print_list(tmp_S[i]);
            fprintf(pFile, "new F: %f\n", migrate_F);
            fprintf(pFile, "bitmask i: %d ,j: %d\n", sets[i].first, sets[i].second);
            S = tmp_S;
            flag = true;
            return flag;
            // //return or not?
        }
    }
    return flag;
}

bool check_exist(struct X x, vector<struct X> check_list){
    bool exist = false;
    for(size_t i=0;i<check_list.size();i++){ // All X in day t
        struct X c_x = check_list[i];
        if(x.D.size() != c_x.D.size()){
            exist |= false;
            continue;
        }else{
            bool same_node = true;
            for(size_t j=0;j < x.D.size();j++){
                if(x.D[j] == c_x.D[j]){
                    same_node &= true;
                }else{
                    same_node &= false;
                }
            }
            exist |= same_node;
            if(exist)
                return exist;
        }
    }
    return exist;
}

void forward_rescheduling(vector<struct X>& Y, vector<vector<struct X> >&S, size_t t, Graph& g){
    assert(t >= 1);
    assert(t < period_T);
    

    Y.clear();
    for(size_t y=0;y<S[t].size();y++){
        if(check_exist(S[t][y], S[t-1])){
            continue;
        }

        // check whether this group appears at prevois day
        vector<vector<struct X> >tmp_S = S;
        tmp_S[t].erase(tmp_S[t].begin() + y);

        double ss_t = SS(S[t][y], tmp_S, g, t);
        double ss_prev_t = SS(S[t][y], tmp_S, g, t-1);
        if(((ss_t - ss_prev_t)/ ss_prev_t) > 1)
            Y.push_back(S[t][y]);
    }
}

void backward_rescheduling(vector<struct X>& Z, vector<vector<struct X> >&S, size_t t, Graph& g){
    assert(t <= period_T - 2);
    assert(t >= 0);

    Z.clear();
    for(size_t z=0;z<S[t].size();z++){
        // check whether this group appears at prevois day
        if(check_exist(S[t][z], S[t+1])){
            continue;
        }
        vector<vector<struct X> >tmp_S = S;

        tmp_S[t].erase(tmp_S[t].begin() + z);

        double ss_t = SS(S[t][z], tmp_S, g, t);
        double ss_t_with_S = SS(S[t][z], S, g, t);
        double ss_next_t = SS(S[t][z], tmp_S, g, t+1);
        if(((ss_next_t - ss_t)/ ss_t) < (ss_t_with_S / S[t][z].D.size()))
            Z.push_back(S[t][z]);
    }
}

void SAS(vector<vector<struct X> >&S, int ra, Graph& g, double max_F){
    
    if(period_T == 0)
        return;
    
    assert(period_T > 0);
    int first =0;
    size_t last = period_T;
    bool flag = true;
    
    while(flag){
        flag = false;
        for(size_t t=first +1;t<period_T;t++){
            vector<struct X> Y, Z;
            forward_rescheduling(Y, S, t, g);
            backward_rescheduling(Z, S, t-1, g);
            flag |= rescheduling(Y, Z, ra, g, S, max_F);
            if(flag)
                break;
        }
        if(flag)
            break;
        last--;
        for(int t=last;t>=first +1;t--){
            vector<struct X> Y, Z;
            forward_rescheduling(Y, S, t, g);
            backward_rescheduling(Z, S, t-1, g);
            flag |= rescheduling(Y, Z, ra, g, S, max_F);
            if(flag)
                break;
        }
        first++;
    }
}