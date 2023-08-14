set(TESTS ntt10_4x_nof3456 ntt10_4x_nof34567 ntt9_2x_h12345678 schoolbook karatsuba radix2 karatsuba_kara radix2_kara main_lay1_fwd_extract)

foreach(X IN LISTS TESTS)
  add_executable("${X}_test" "${X}.cpp")
  target_link_libraries("${X}_test" xpower)
  target_include_directories("${X}_test" PRIVATE ../../include/xpower ../../include/shared ../../src/xpower ../include)
endforeach()
