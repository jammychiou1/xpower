set(TESTS ntt10_4x ntt10_4x_nof3456 ntt10_4x_nof34567 ntt10_4x_noh3456 ntt9_2x karatsuba radix2 main_lay1_fwd_extract main_lay2_fwd_inplace mainmul lowmul)

foreach(X IN LISTS TESTS)
  add_executable("${X}_test" "${X}.cpp")
  target_link_libraries("${X}_test" xpower)
  target_include_directories("${X}_test" PRIVATE ../../include/xpower ../../include/shared ../../src/xpower ../include)
endforeach()
