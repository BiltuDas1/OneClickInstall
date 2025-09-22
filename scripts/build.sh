#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/compile.sh"

function build {
  BUILD_DIR="$SCRIPT_DIR/../build"

  # If the Build ran first time, then delete everything
  if [[ ! -f "$BUILD_DIR/configuration.lock" ]]; then
    clean_configuration
  fi

  # If Build Directory doesn't exist then build one
  if [[ ! -d "$BUILD_DIR" ]]; then
    debug_configure_only
    touch "$BUILD_DIR/configuration.lock" # Create flag which tells that the build is not happening first time
  fi

  build_only
}

build