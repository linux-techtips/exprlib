cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_FLAGS_DEBUG "-g -ldl -export-dynamic")

file(GLOB exprlib_headers
    "${PROJECT_SOURCE_DIR}/include/exprlib/*.hpp"
)

add_library(exl INTERFACE)

target_include_directories(exl INTERFACE ../include)
target_link_libraries(exl INTERFACE fmt::fmt)

string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME)
add_definitions("-DSYSTEM_${SYSTEM_NAME}")

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${exprlib_headers}
)