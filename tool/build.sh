#!/usr/bin/env bash
set -e
mkdir -p build
build_type=${1:-"Debug"}

############################
#   build execution file   #
############################
mkdir -p build/$build_type
cmake -Bbuild/$build_type -DCMAKE_BUILD_TYPE=${build_type} -GNinja
cmake --build build/$build_type 
