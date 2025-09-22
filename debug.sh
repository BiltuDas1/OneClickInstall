#!/bin/bash

# Compile only the Windows Binary when COMPILE_ONLY flag is set
if [[ -n "$COMPILE_ONLY" ]]; then
  cd /mnt
  poetry run python autobuild.py
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
