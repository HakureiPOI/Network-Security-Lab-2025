import subprocess
import re
import matplotlib.pyplot as plt
import numpy as np
from collections import defaultdict

# 运行次数
num_runs = 10

# 用于统计每个 i 的所有访问时间
access_times = defaultdict(list)

# 正则匹配每一行输出
pattern = re.compile(r"访问 array\[(\d+)\*4096\] 的时间: (\d+) 个CPU周期")

print("正在运行 CacheTime 程序 {} 次...\n".format(num_runs))

for run in range(num_runs):
    print(f"第 {run + 1} 次运行")
    result = subprocess.run(["./CacheTime"], capture_output=True, text=True)
    lines = result.stdout.strip().split('\n')

    for line in lines:
        match = pattern.search(line)
        if match:
            index = int(match.group(1))
            time = int(match.group(2))
            access_times[index].append(time)

# 汇总平均值
indexes = sorted(access_times.keys())
avg_times = [np.mean(access_times[i]) for i in indexes]

# 打印结果
print("\n平均访问时间（单位：CPU周期）")
for i, t in zip(indexes, avg_times):
    print(f"array[{i} * 4096]: {t:.2f}")

# 可视化
plt.figure(figsize=(10, 6))
plt.bar(indexes, avg_times)
plt.title("平均访问时间 vs 数组下标")
plt.xlabel("数组索引 i")
plt.ylabel("平均访问时间（CPU 周期）")
plt.xticks(indexes)
plt.grid(True)
plt.tight_layout()
plt.show()
