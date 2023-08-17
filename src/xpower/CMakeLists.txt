add_library(xpower OBJECT main_lay1.cpp main_lay2.cpp basemul.cpp mainmul.cpp lowmul.cpp)
target_include_directories(xpower PRIVATE ../../include/xpower ../../include/shared)
