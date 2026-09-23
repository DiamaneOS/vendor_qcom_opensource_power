#!/bin/sh
# Build and run the host hint-session lifecycle test against the real source.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
repo=$(cd "$here/../.." && pwd)
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
${CXX:-clang++} -std=c++17 -Wall -I "$here/stubs" -I "$repo" \
    "$repo/PowerHintSession.cpp" "$here/session_lifecycle_test.cpp" -o "$out/test"
"$out/test"
