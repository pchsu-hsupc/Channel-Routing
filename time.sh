#!/bin/bash

# 确保提供了正确数量的参数
if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <executable> <inputfile> <outputfile> <num_runs>"
    exit 1
fi

executable=$1
inputfile=$2
outputfile=$3
num_runs=$4
total_time=0

echo "Running $executable for $num_runs times..."

for (( i=0; i<num_runs; i++ ))
do
    # 记录开始时间
    start=$(date +%s.%N)

    # 运行程序
    ./"$executable" "$inputfile" "$outputfile"

    # 记录结束时间
    end=$(date +%s.%N)

    # 计算并累加这次运行的时间
    duration=$(echo "$end - $start" | bc)
    total_time=$(echo "$total_time + $duration" | bc)
done

# 计算平均时间
average_time=$(echo "$total_time / $num_runs" | bc -l)

printf "Average Execution Time: %.6f seconds\n" $average_time
