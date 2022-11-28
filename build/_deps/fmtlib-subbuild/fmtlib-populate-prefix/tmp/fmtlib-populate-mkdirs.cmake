# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-src"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-build"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/tmp"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/src/fmtlib-populate-stamp"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/src"
  "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/src/fmtlib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/src/fmtlib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/m3rlin/Code/exprlib/build/_deps/fmtlib-subbuild/fmtlib-populate-prefix/src/fmtlib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
