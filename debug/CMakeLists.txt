set(DBGS rqmul)

foreach(X IN LISTS DBGS)
  add_executable("${X}_dbg" "${X}.cpp")
  target_link_libraries("${X}_dbg" xpower)
  target_include_directories("${X}_dbg" PRIVATE ../include/xpower ../include/shared ../src)
endforeach()
