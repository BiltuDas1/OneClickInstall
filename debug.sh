#!/bin/bash

# Compile only the Windows Binary when COMPILE_ONLY flag is set
if [[ -n "$COMPILE_ONLY" ]]; then
  cd /mnt

  # If RELEASE_BUILD_ONLY is set then build only Release version
  if [[ -n "$RELEASE_BUILD_ONLY" ]]; then
    source scripts/compile.sh

    clean_configuration
    configure_only
    build_only
    strip_all
    # If PIPELINE_BUILD is not set then compress the exe
    if [[ ! -n "$PIPELINE_BUILD" ]]; then
      compress
    fi
    inject_default_tokenid

    exit
  fi

  bash scripts/build.sh
  exit
fi

# Start Python (Poetry) in the background
poetry install > /dev/null
poetry run debug &
PYTHON_PID=$!

# Function to handle termination signals
function cleanup {
  echo "Sending Termination Signal to Server..."
  kill -TERM $PYTHON_PID
}

# Trap Ctrl+C (SIGINT) and Docker stop (SIGTERM)
trap cleanup INT TERM

# Wait for the Python process to finish
wait $PYTHON_PID
EXIT_CODE=$?

echo "Server exited with code $EXIT_CODE"
exit $EXIT_CODE
