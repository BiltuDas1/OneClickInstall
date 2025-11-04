#!/bin/bash
set -e

# Env vars GH_TOKEN and VERSION are passed by the workflow
if gh release view "$VERSION" >/dev/null 2>&1; then
  echo "Release $VERSION already exists. Skipping new release and build."
  echo "skip=true" >> $GITHUB_OUTPUT
else
  echo "Release $VERSION does not exist. Proceeding."
  echo "skip=false" >> $GITHUB_OUTPUT
fi