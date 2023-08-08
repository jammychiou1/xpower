set(TESTS ntt10_4x_nof3456 ntt10_4x_nof34567 ntt5_4x_nof14 ntt5_4x_nof03 ntt5_4x_nof023)

foreach(X IN LISTS TESTS)
  add_executable("${X}" "${X}.cpp")
  target_link_libraries("${X}" xpower)
  target_include_directories("${X}" PRIVATE ../../include/xpower ../../src/xpower)
endforeach()
