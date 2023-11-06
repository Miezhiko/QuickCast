cmake_minimum_required(VERSION 3.18)

add_compile_options(-W4)
set(CMAKE_C_FLAGS_RELEASE "/Oi /Ot /Oy /Ox /GL /Gz /Qpar /WX /TC /std:c17")

project (QuickCast)

option(USE_LTO "Use link time optimization" TRUE)
option(WITH_BORDERS_CHECK "Check ingame for clickable area" TRUE)
option(WITH_MEMES "Additional funny rarely useful hotkeys" FALSE)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION USE_LTO)

include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/src")
set(RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}.rc")

add_executable (${CMAKE_PROJECT_NAME}
                WIN32
                src/main.c
                ${RESOURCE})

execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE)
add_definitions(-DHASH_CMAKE=${GIT_COMMIT_HASH})

if (WITH_BORDERS_CHECK)
  add_definitions(-DWITH_BORDERS_CHECK=${WITH_BORDERS_CHECK})
endif (WITH_BORDERS_CHECK)

if (WITH_MEMES)
  add_definitions(-DWITH_MEMES=${WITH_MEMES})
endif (WITH_MEMES)

target_link_libraries(${CMAKE_PROJECT_NAME} ntdll)
target_precompile_headers(${CMAKE_PROJECT_NAME}
  PRIVATE
    src/stdafx.h
)

configure_file( ${CMAKE_SOURCE_DIR}/misc/conf.ini.example
                ${CMAKE_CURRENT_BINARY_DIR}/Release/conf.ini
                COPYONLY )
