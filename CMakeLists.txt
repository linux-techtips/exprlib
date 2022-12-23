cmake_minimum_required(VERSION 3.24)

project(
    exprlib
    VERSION 0.0.1
    LANGUAGES CXX
)

if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED true)

    set(CMAKE_CXX_EXTENSIONS OFF)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing library
    include(CTest)
endif()

include(FetchContent)
FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 5.3.0
)

FetchContent_MakeAvailable(fmtlib)

add_subdirectory(src)

if ((${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME} OR EXPRLIB_CMAKE_BUILD_TESTING) AND BUILD_TESTING)
    add_subdirectory(tests)
endif()