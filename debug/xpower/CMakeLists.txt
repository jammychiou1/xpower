set(DBGS ntt10_4x_nof3456 ntt10_4x_nof34567 ntt5_4x_nof14 ntt5_4x_nof03 ntt5_4x_nof023 ntt9_2x_h12345678 main_schoolbook main_karatsuba main_radix2 mla_tmv_8_kara)

foreach(X IN LISTS DBGS)
  add_executable("${X}_dbg" "${X}.cpp")
  target_link_libraries("${X}_dbg" xpower)
  target_include_directories("${X}_dbg" PRIVATE ../../include/xpower ../../include/shared ../../src/xpower ../include)
endforeach()
