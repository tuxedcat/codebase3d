#!/usr/bin/env bash
set -e
mkdir -p build
src=${1:-"src/main.cpp"}
build_type=${2:-"Debug"}

############################
#   build execution file   #
############################
mkdir -p build/$src/$build_type
cmake -Bbuild/$src/$build_type -DSOURCE_PATH=${src} -DCMAKE_BUILD_TYPE=${build_type} -GNinja
cmake --build build/$src/$build_type 
