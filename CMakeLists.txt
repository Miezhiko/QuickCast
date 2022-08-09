cmake_minimum_required(VERSION 3.18)

add_compile_options(-W2)
set(CMAKE_CXX_FLAGS_RELEASE "/Oi /Oy /O2 /GL /Qpar /WX")

project (quickcast)

option(USE_LTO "Use link time optimization" TRUE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION USE_LTO)

add_executable (quickcast WIN32 global_hook.c)
configure_file( ${CMAKE_SOURCE_DIR}/conf.ini
                ${CMAKE_CURRENT_BINARY_DIR}/Release/conf.ini
                COPYONLY )
