
## 圖的參數介紹

- Name
    - (graph_name)_(時間 T).txt
- parameters type
    - g
        1. V (節點數)
        2. E (邊數)
        3. U_LENGTH (總共的 U 數量) 
    - n
        1. id
        2. type 點的類型（目前沒用到） 
        3. Relative 
        4. Contagion
        5. Symptom
        6. Critical
        7. Healing(from Infected)
        8. Healing(from Ailing)
        9. Healing(from Threatened)
        10. Death
    - e
        1. n1 (邊的左右其中一個點)
        2. n2 (邊的扣除 n1 的另一點)
        3. p (邊的機率)
    - X
        1. id
        2. t (該 group 屬於第 t 天)
        3. cost (隔離該 group 的成本)
        4. lv (該 group 的 level)
        5. eta （初始值是 `-1`）
        6. n1,n2,…,nz （該 group 的所有的點）
- value range
    - g
        - V: 從 1 開始算
        - E: 從 1 開始算
        - U_LENGTH: 從 1 開始算
    - n
        - id: 從 0 開始算
        - type: 0~3 (沒用到)
        - R,Ct,S,Cr,HI,HA,HT,D: 0~1
    - e
        - n1, n2: 0~V-1
        - p: 0~1
    - X
        - t: 天數 0~T-1
        - lv: 1~3 或者 1~2(for opt)
        - eta: -1 for default
        - n1,n2,…,nz: 0~V-1

## 備註

1. 需要按照上述 parameters 中細項的順序給值
2. 所有的 type 都用空白分隔設定值，除了 X 之外的使用 `,` 分隔不同的設定值。X 使用 `_` 分隔該群的基本設定，再用 `,` 分隔群裡的每個點
3. opt 的圖跟一般的圖差別在於，opt 會找出所有符合 budget 的所有 X，這些的 X 使用 `* (index)` 上下分隔