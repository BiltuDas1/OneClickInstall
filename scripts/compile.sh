#!/bin/bash

BUILD_TYPE=Release

function configure_only {
  if [[ -f "CMakeLists.txt" ]]; then
    if [[ -z "$1" ]]; then
      cmake -S . -B build \
        -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
        -DCMAKE_CXX_FLAGS_RELEASE="-O2 -DNDEBUG -ffunction-sections -fdata-sections" \
        -DCMAKE_EXE_LINKER_FLAGS_RELEASE="-Wl,--gc-sections -s" \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON
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

function strip_all {
  if [[ -f "build/OneClickInstall.exe" ]]; then
    echo "Striping the executable..."
    /opt/mxe/usr/bin/x86_64-w64-mingw32.static-strip --strip-all ./build/OneClickInstall.exe
    echo "Striping Complete"
  fi
}

function compress {
  if [[ -f "build/OneClickInstall.exe" ]]; then
    echo "Compression Started"
    upx build/OneClickInstall.exe
    echo "Compression Complete"
  fi
}

function inject_default_tokenid {
  if [[ -f "build/OneClickInstall.exe" ]]; then
    printf "/*__PLACEHOLDER_TOKEN_MUST_BE_REPLACED_BY_SERVER_SCRIPT_64_B__*/" >> build/OneClickInstall.exe
    echo "Default TokenID Patched"
  fi
}