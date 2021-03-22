#!/usr/bin/env bash

set -eux

TEST_DIR=hdoc-test

# Exit if the testing directory already exists due to gitlab CI caching
if [ -d "$TEST_DIR" ]; then
 exit 0
fi

URLS=(
    https://github.com/qedsoftware/doxygen-demo.git
    https://github.com/mandreyel/mio.git
    https://github.com/nlohmann/crow.git
    https://github.com/davisking/dlib.git
)

mkdir -p "$TEST_DIR"

pushd "$TEST_DIR"
for URL in "${URLS[@]}"; do
    # Clone repo
    git clone --single-branch --depth=1 "$URL"
    DIR=$(basename "$URL" .git)

    # Get git info for given project
    pushd "$DIR"

    # Configure CMake
    cmake -GNinja                             \
          -Bbuild                             \
          -DCMAKE_BUILD_TYPE=Release          \
          -DCMAKE_EXPORT_COMPILE_COMMANDS=1

    # Copy .hdoc.toml file for this repository
    cp ../../tomls/"$DIR".toml .hdoc.toml

    # Remove git files to save some space
    rm -rf .git/
    popd
done
popd
