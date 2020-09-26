
g V,E,U_LENGTH
n id,type,R,Ct,S,Cr,HI,HA,HT,D
e n1,n2,p
X id_t_cost_lv_eta_n1,n2,…,nz

- U_LENGTH = the number of groups
- X = groups(strategy) type
- value range
    - type: 0~3
    - R,Ct,S,Cr,HI,HA,HT,D: 0~1
    - V: 從 1 開始算
    - id: count from 0
    - n1, n2: 0~V-1
    - p: 0~1
    - E: 從 1 開始算
    - t: 天數 0~T-1
    - GN: 1 開始
    - n1~nz: 0~V-1
    - cost: ?? (先 1~10 random)
    - lv: 0~3 整數
    - eta: ?? (先 (1~10) random, -1 for default)
