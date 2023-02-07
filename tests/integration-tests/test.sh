#!/usr/bin/env bash

set -eu

pushd corpus

PROJECT_DIRS=$(ls)
for DIR in $PROJECT_DIRS; do
    pushd "$DIR"
    ../../../../build/hdoc --verbose
    popd
done
popd
