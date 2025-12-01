#!/bin/bash
set -e

# Initialize Wine (Headless)
echo "Initializing Wine..."
xvfb-run wineboot --init
wineserver -w

echo "Installing UPX..."
# Update and install UPX (running as root inside container)
apt-get update && apt-get -y install upx-ucl

echo "Preparing Application..."
# Cypress downloaded the file to 'cypress/downloads/installer.exe' relative to the workspace root
# We copy it to the current directory
if [ -f "cypress/downloads/installer.exe" ]; then
    cp cypress/downloads/installer.exe installer.exe
else
    echo "Error: Source file cypress/downloads/installer.exe not found!"
    ls -R cypress/downloads || true
    exit 1
fi

echo "Decompressing Executable..."
upx -d installer.exe

echo "Running Application..."
xvfb-run wine installer.exe /S /LOG install.log

if [ -f "install.log" ]; then
  echo "--- Displaying contents of install.log ---"
  cat install.log
else
  echo "Error: install.log was not created."
fi

# Verify Installation
# Note: In this Docker container running as root, HOME is /root
installPath="$HOME/.wine/drive_c/Program Files/7-Zip/7z.exe"

if [ -f "$installPath" ]; then
  echo "Installation successful!"
else
  echo "Error: Installation failed! File not found at $installPath"
  # Debugging aid: list files if missing
  ls -R "$HOME/.wine/drive_c/Program Files/" || true
  exit 1
fi