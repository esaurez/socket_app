#pragma once

#include <time.h>

#include <cassert>
#include <cstdint>

static inline uint64_t rdtscp(uint32_t* auxp) {
  uint64_t ret;
  uint32_t c;

#if __GNUC_PREREQ(10, 0)
#if __has_builtin(__builtin_ia32_rdtscp)
  ret = __builtin_ia32_rdtscp(&c);
#endif
#else
  uint64_t a, d;
  asm volatile("rdtscp" : "=a"(a), "=d"(d), "=c"(c));
  ret = a | (d << 32);
#endif

  if (auxp) *auxp = c;
  return ret;
}

static inline uint64_t rdtsc(void) {
#if __GNUC_PREREQ(10, 0)
#if __has_builtin(__builtin_ia32_rdtsc)
  return __builtin_ia32_rdtsc();
#endif
#else
  uint64_t a, d;
  asm volatile("rdtsc" : "=a"(a), "=d"(d));
  return a | (d << 32);
#endif
}

static inline void cpu_serialize(void) {
  asm volatile(
      "xorl %%eax, %%eax\n\t"
      "cpuid"
      :
      :
      : "%rax", "%rbx", "%rcx", "%rdx");
}

static int time_calibrate_tsc(void) {
  /* TODO: New Intel CPUs report this value in CPUID */
	struct timespec sleeptime = {
			.tv_sec = 0,
			.tv_nsec =
					static_cast<long>(5E8)}; /* 1/2 second */
	struct timespec t_start, t_end;

  cpu_serialize();
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &t_start) == 0) {
    uint64_t ns, end, start;
    double secs;

    start = rdtsc();
    nanosleep(&sleeptime, NULL);
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
    end = rdtscp(NULL);
    ns = ((t_end.tv_sec - t_start.tv_sec) * static_cast<uint64_t>(1E9));
    ns += (t_end.tv_nsec - t_start.tv_nsec);

    secs = (double)ns / 1000;
    int cycles_per_us = static_cast<int>(static_cast<double>(end - start) / secs);
    return cycles_per_us;
  }

  return -1;
}