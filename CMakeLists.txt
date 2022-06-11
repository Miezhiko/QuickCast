cmake_minimum_required(VERSION 3.18)
set (CMAKE_CXX_STANDARD 20)

project (quickcast)

option(USE_LTO "Use link time optimization" TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION USE_LTO)

#add_library(mawa SHARED hotkeys.cxx)
#add_executable (quickcast dll_inject.cxx)
add_executable (quickcast WIN32 global_hook.cxx)
