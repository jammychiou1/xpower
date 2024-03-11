#include <cstdint>
#include <iostream>
#include <algorithm>

#include <linux/perf_event.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "rqmul.h"

int16_t in1_poly[768];
int16_t in2_poly[768];
int16_t out_poly[768];

uint64_t records[1000];

int cycle_fd;
perf_event_mmap_page *cycle_info;

int ctx_fd;
perf_event_mmap_page *ctx_info;

void setup_perf() {

  perf_event_attr cycle_attr = {
    .type = PERF_TYPE_HARDWARE,
    .config = PERF_COUNT_HW_CPU_CYCLES,
    .config1 = 0x3,
  };

  cycle_fd = syscall(SYS_perf_event_open, &cycle_attr, 0, -1, -1, 0);
  if (cycle_fd == -1) {
    fprintf(stderr, "perf_event_open failed\n");
    exit(1);
  }

  cycle_info = (perf_event_mmap_page*) mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_SHARED, cycle_fd, 0);

  if (cycle_info == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    exit(1);
  }

  perf_event_attr ctx_attr = {
    .type = PERF_TYPE_SOFTWARE,
    .config = PERF_COUNT_SW_CONTEXT_SWITCHES,
  };

  ctx_fd = syscall(SYS_perf_event_open, &ctx_attr, 0, -1, -1, 0);
  if (ctx_fd == -1) {
    fprintf(stderr, "perf_event_open failed\n");
    exit(1);
  }

  ctx_info = (perf_event_mmap_page*) mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_SHARED, ctx_fd, 0);

  if (ctx_info == MAP_FAILED) {
    fprintf(stderr, "mmap failed\n");
    exit(1);
  }

}

uint64_t get_time() {
  uint64_t t;
  __asm__ volatile("mrs %0, PMCCNTR_EL0" : "=r" (t));
  return t;
}

int main() {
  setup_perf();

  ioctl(cycle_fd, PERF_EVENT_IOC_ENABLE, 0);
  ioctl(ctx_fd, PERF_EVENT_IOC_ENABLE, 0);
  for (int t = 0; t < 1000; t++) {
    ioctl(ctx_fd, PERF_EVENT_IOC_RESET, 0);
    uint64_t start = get_time();

    for (int d = 0; d < 1000000; d++) {
      xpower::rqmul::rqmul(in1_poly, in2_poly, out_poly);
    }
    sleep(3);

    uint64_t end = get_time();

    records[t] = end - start;
    int64_t count;
    read(ctx_fd, &count, sizeof(count));
    printf("ctx: %lld\n", count);
    printf("cycle: %lld\n", end - start);
  }

  std::sort(records, records + 1000);
  std::cout << records[500]  << '\n';
  std::cerr << records[0] << ' ' << records[999] << '\n';
}

