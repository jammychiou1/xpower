# Building

1. Run `cmake -DOPTIMIZE=y . -B build`. The flag `-DOPTIMIZE=y` is required to
select the `-O3` version.
2. Run `cmake --build build`.

# Benchmarking

1. See `enable_ccr` to build the kernel module, which enables user access to
cycle count register.
2. Run `build/bench/rqmul_enable_ccr_bench` to see the cycle count of the whole
polynomial multiplication (polymul).
3. Run `build/bench/subs_enable_ccr_bench` to see the cycle counts of each
subroutines.

The output formats is (out of 1000 runs):
```
${MEDIAN}
${MIN} ${MAX}
${FIRST_QUARTILE} ${THIRD_QUARTILE}
```

NOTE:
Due to task migration between cores, the cycle count can (very rarely) have
extremely wild values (e.g. the maximum being orders of magnitude larger than
the median, or the minimum being less than zero).
