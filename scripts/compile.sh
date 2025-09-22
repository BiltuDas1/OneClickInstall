#!/bin/bash

BUILD_TYPE=Release

function configure_only {
  if [[ -f "CMakeLists.txt" ]]; then
    if [[ -z "$1" ]]; then
      cmake -S . -B build -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF
    else
      cmake -S . -B build -DCMAKE_BUILD_TYPE=$1 -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=OFF
    fi
  fi
}

function debug_configure_only {
  configure_only Debug
}

function build_only {
  if [[ -f "CMakeLists.txt" ]]; then
    cmake --build build -j$(nproc)
  fi
}

function build_single_thread {
  if [[ -f "CMakeLists.txt" ]]; then
    cmake --build build
  fi
}

function clean_configuration {
  if [[ -f "CMakeLists.txt" ]]; then
    rm -rf build/
  fi
}

function clean_build {
  if [[ -f "CMakeLists.txt" ]]; then
    cmake --build build --target clean
  fi
}

