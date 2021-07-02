#!/usr/bin/env bash

set -eu

TEST_DIR=hdoc-test
pushd "$TEST_DIR"

PROJECT_DIRS=$(ls)
for DIR in $PROJECT_DIRS; do
    pushd "$DIR"
    /builds/e/hdoc/build/hdoc --verbose
    popd
done
popd
