set(BENCH rqmul rqmul_pmu)

foreach(X IN LISTS BENCH)
  add_executable("${X}_bench" "${X}.cpp")
  target_link_libraries("${X}_bench" xpower)
  target_include_directories("${X}_bench" PRIVATE ../include/xpower ../include/shared ../src)
endforeach()
