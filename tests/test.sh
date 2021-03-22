#!/usr/bin/env bash

set -e

# Colors for bash output
NOCOLOR='\033[0m'
GREEN='\033[1;32m'
BLUE='\033[1;34m'

TEST_DIR=hdoc-test
pushd "$TEST_DIR" > /dev/null
PROJECT_DIRS=$(ls)

for DIR in $PROJECT_DIRS; do
    pushd "$DIR" > /dev/null

    # Run hdoc and capture elapsed time
    echo -e "${BLUE}""Starting""${NOCOLOR} \t""$DIR"
    SECONDS=0
    ../../../build/hdoc --verbose

    # Pretty print results
    echo -e "${GREEN}""Completed""${NOCOLOR} (${SECONDS}s)\t""$DIR"

    # Minimal test suite for quick checks
    if [ "$1" = "quick" ]; then
       exit 0
    fi

    popd > /dev/null
done
popd > /dev/null
