#!/bin/bash
set -e

# Env vars GH_TOKEN, VERSION, and IS_PRERELEASE are passed by the workflow
gh release create "$VERSION" \
  --title "Release $VERSION" \
  --notes-file changelog.md \
  --prerelease=$IS_PRERELEASE