cmake_minimum_required(VERSION 3.18)

add_compile_options(-W2)
set(CMAKE_CXX_FLAGS_RELEASE "/Oi /Ot /Oy /O2 /GL /Qpar /WX")

project (QuickCast)

option(USE_LTO "Use link time optimization" TRUE)
option(WITH_TRAY "Display tray icon" TRUE)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION USE_LTO)

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src")
set(RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.rc")
add_executable (${CMAKE_PROJECT_NAME}
                WIN32
                src/main.c
                ${RESOURCE})

add_definitions(-DSOURCE_DIR=${CMAKE_SOURCE_DIR})

if (WITH_TRAY)
  execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  add_definitions(-DHASH_CMAKE=${GIT_COMMIT_HASH})
  add_definitions(-DWITH_TRAY=${WITH_TRAY})
endif (WITH_TRAY)

target_precompile_headers(${CMAKE_PROJECT_NAME}
  PRIVATE
    src/stdafx.h
)

target_link_libraries(${CMAKE_PROJECT_NAME} ntdll)

configure_file( ${CMAKE_SOURCE_DIR}/misc/conf.ini.example
                ${CMAKE_CURRENT_BINARY_DIR}/Release/conf.ini
                COPYONLY )
