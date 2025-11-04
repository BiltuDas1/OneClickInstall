#!/bin/bash
set -e

# Mark the main repository and all submodules as safe
git config --global --add safe.directory $GITHUB_WORKSPACE
git submodule foreach 'git config --global --add safe.directory "$PWD"'

echo "## Changelog" > changelog.md

git fetch --tags

# This command is now safe to fail if no tags exist
LATEST_TAG=$(git describe --tags --abbrev=0 2>/dev/null || true)

if [ -n "$LATEST_TAG" ]; then
  echo "Found previous tag: $LATEST_TAG"
  echo -e "\n**Full commit log since $LATEST_TAG:**\n" >> changelog.md
  git log $LATEST_TAG..HEAD --pretty=format:"* %s (%h)" >> changelog.md
else
  echo "No previous tag found. Listing all commits."
  echo -e "\n**Full commit log:**\n" >> changelog.md
  git log --pretty=format:"* %s (%h)" >> changelog.md
fi

echo "---" >> changelog.md
echo "Changelog generated:"
cat changelog.md