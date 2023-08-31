set(DBGS ntt10_4x_nof3456 ntt10_4x_nof34567 ntt5_4x_nof14 ntt5_4x_nof03 ntt5_4x_nof023 ntt9_2x mainmul lowmul rqmul rqmul_full main_lay1_fwd_extract_range main_lay1_bwd_insert_range main_lay2_fwd_inplace_range main_lay2_bwd_inplace_range basemul_main_range low_lay1_fwd_extract_range low_lay1_bwd_insert_range basemul_low_range ntt9_2x_range)

foreach(X IN LISTS DBGS)
  add_executable("${X}_dbg" "${X}.cpp")
  target_link_libraries("${X}_dbg" xpower)
  target_include_directories("${X}_dbg" PRIVATE ../../include/xpower ../../include/shared ../../src/xpower ../include)
endforeach()
