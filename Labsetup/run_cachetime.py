import subprocess
import re
import pandas as pd
from collections import defaultdict

# 运行次数
RUNS = 10

# 用于存储每个 i 的访问时间
results = defaultdict(list)

# 正则匹配输出
pattern = re.compile(r"Access time for array\[(\d+)\*4096\]: (\d+) CPU cycles")

print(f"开始执行 CacheTime 程序 {RUNS} 次...\n")

for run in range(RUNS):
    print(f"第 {run + 1} 次运行...")
    proc = subprocess.run(["./CacheTime"], capture_output=True, text=True)
    output = proc.stdout.strip().splitlines()
    
    for line in output:
        match = pattern.search(line)
        if match:
            index = int(match.group(1))
            time = int(match.group(2))
            results[index].append(time)

print("\n所有执行完成，正在统计结果...\n")

# 构造表格
data = []
for index in sorted(results.keys()):
    times = results[index]
    avg_time = sum(times) / len(times)
    data.append({"数组下标 i": index, "平均访问时间": round(avg_time, 2), "样本数": len(times)})

df = pd.DataFrame(data)
print(df)

# 打印结果表格
print(df.to_csv('cachetime.csv', index = False))
