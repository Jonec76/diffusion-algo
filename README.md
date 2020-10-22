# README

## 執行步驟
1. `make`

2. 執行 greedy 演算法 => `./greedy config.txt`</br>
   執行 main 演算法 => `./main config.txt`</br>
   執行 diffusion 演算法 => `./diffusion config.txt`</br>
   執行 opt 演算法 => `./opt config.txt`
    - config 中的圖要換成 `OPTgraph_` 開頭的圖檔
    - config `c` 後面的參數要對上圖檔檔名的 T, b

## 參數

- config.txt （詳細請看 config.txt）
    - w_S
    - w_I
    - w_R
    - delta_c
    - delta_f
    - delta_i
    - theta
    - a_end
- data.h
    - simple/original RCR
- diff_func.cpp 
    - alpha_f=0.944 (使用於 Obj. function)
    - alpha_g=0.5 (使用於 Obj. function)
    - param_c=2 (使用於 Obj. function)
- init.cpp
    - level_table
- main.cpp
    - RA (使用於限制 procedure 7 的集合總上限數)
- opt.cpp
    - level_table_opt

## 備註
1. 更改 `config.txt` 內容調整參數
2. 產出結果位於 `./result` 底下
3. 圖檔的讀取都會從 `covid_data` 資料夾下讀取
4. `graph_format.md` 紀錄圖的參數設定
