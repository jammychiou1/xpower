set(BENCH karatsuba_radix2 main_lay1_fwd_extract main_lay1_bwd_insert main_lay2_fwd_inplace main_lay2_bwd_inplace main_forward main_backward mainmul lowmul rqmul)

foreach(X IN LISTS BENCH)
  add_executable("${X}_bench" "${X}.cpp")
  target_link_libraries("${X}_bench" xpower)
  target_include_directories("${X}_bench" PRIVATE ../../include/xpower ../../include/shared ../../src/xpower)
endforeach()
