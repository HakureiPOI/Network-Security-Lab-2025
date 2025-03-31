#!/bin/bash

# 重复运行次数
RUNS=10

# 输出文件
OUTPUT="results.txt"

echo "运行 CacheTime $RUNS 次，结果保存到 $OUTPUT"
echo "" > $OUTPUT

for i in $(seq 1 $RUNS); do
    echo "第 $i 次运行：" >> $OUTPUT
    ./CacheTime >> $OUTPUT
    echo "" >> $OUTPUT
done

echo "完成！请查看 $OUTPUT 文件以查看所有结果。"
