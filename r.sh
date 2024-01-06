#!/bin/bash

# 檢查是否有一個參數傳入
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <input number>"
    exit 1
fi

# 使用傳入的參數
input_number=$1

# 清理並構建項目
make clean
make

# 執行程式與解決方案檢查
./Lab4 "./input/input${input_number}.in" "./output/output${input_number}.out"
./testcase/SolutionChecker "./input/input${input_number}.in" "./output/output${input_number}.out"
