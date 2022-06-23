cmake_minimum_required(VERSION 3.18)
set (CMAKE_CXX_STANDARD 20)

# set(VERSION 1.0.0)

add_compile_options(-W2)

project (quickcast)

option(USE_LTO "Use link time optimization" TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION USE_LTO)

# add_definitions( -DVERSION_CMAKE=${VERSION} )

# dll inject method (risky and crashing wc3, maybe because of anti-anti-debug)
# add_library(mawa SHARED hotkeys.cxx)
# add_executable (quickcast dll_inject.cxx)

add_executable (quickcast WIN32 global_hook.cxx)
