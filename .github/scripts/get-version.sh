#!/bin/bash
set -e

VERSION=$(cat version.txt)
IS_PRERELEASE="false"

if [[ "$VERSION" == *"-alpha"* || "$VERSION" == *"-beta"* ]]; then
  IS_PRERELEASE="true"
fi

echo "VERSION=$VERSION" >> $GITHUB_OUTPUT
echo "IS_PRERELEASE=$IS_PRERELEASE" >> $GITHUB_OUTPUT