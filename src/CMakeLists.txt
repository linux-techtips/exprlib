cmake_minimum_required(VERSION 3.24)

file(GLOB exprlib_headers
    "${PROJECT_SOURCE_DIR}/include/exprlib/*.hpp"
)

file(GLOB exprlib_src
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_library(exl
    ${exprlib_src}
    ${exprlib_headers}
)

target_include_directories(exl PUBLIC ../include)
target_link_libraries(exl PRIVATE fmt::fmt)
target_compile_features(exl PUBLIC cxx_std_20)

source_group(
    TREE "${PROJECT_SOURCE_DIR}/include"
    PREFIX "Header Files"
    FILES ${exprlib_headers}
)