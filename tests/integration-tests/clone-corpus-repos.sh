#!/usr/bin/env bash

# This script prepare's hdoc's "corpus": a set of test repositories
# that we use to ensure hdoc works on real world code.
# These repositories were chosen because they have minimal dependencies and can
# be configured easily to output a compile_commands.json.

set -eu

TEST_DIR=corpus

# Don't do any work if the testing directory already exists due to gitlab CI caching
if [ -d "$TEST_DIR" ]; then
 exit 0
fi

# Pin everything to a specific commit hash to prevent the content of test repositiories from changin
# under our feet.
URLS=(
    https://github.com/davisking/dlib/archive/02330e0a15e9b90a888f577e31cec86cbb76d496.tar.gz
    https://github.com/qedsoftware/doxygen-demo/archive/f261c082f7f42f5172ca8b1a76912a99444b42ad.tar.gz
    https://github.com/google/draco/archive/7ec8a2783f4c86d5f3c9120d1d58c7ce1b3094ef.tar.gz
    https://github.com/titapo/ezy/archive/515d1f4b9cf2f796cae730316dac4bb942142153.tar.gz
    https://github.com/SanderMertens/flecs/archive/2b62b0663c59c3ed014c5d6b0ddc86099c5b6487.tar.gz
    https://github.com/nlohmann/json/archive/a3e6e26dc83a726b292f5be0492fcc408663ce55.tar.gz
    https://github.com/google/marl/archive/a47a3a5c54435751d30e3154fdf17e3b29fc6796.tar.gz
    https://github.com/oatpp/oatpp/archive/6063b57b250bfa9bdf02c47351a4a4ce1a969079.tar.gz
    https://github.com/google/oboe/archive/3ad855135b360697e581239fcf4843db00bb13f7.tar.gz
    https://github.com/chromium/subspace/archive/9728d5948b56744a32381cc1b18fafec3d463e9a.tar.gz
    https://github.com/nemtrif/utfcpp/archive/780bd57d6341b3291a11a546bf69358277b724bc.tar.gz
)

mkdir -p "$TEST_DIR"

pushd "$TEST_DIR"
for URL in "${URLS[@]}"; do
    PROJECT_NAME=$(echo "$URL" | cut -d'/' -f5)
    FILE_NAME=$(basename "$URL")
    HASH=$(basename "$URL" .tar.gz)

    # Download the source code, which will be downloaded to a file that looks like $HASH.tar.gz
    # Example: 02330e0a15e9b90a888f577e31cec86cbb76d496.tar.gz
    wget "$URL"

    # Extract the source code, which will result in a directory that looks like PROJECT_NAME-$HASH
    # Example: dlib-02330e0a15e9b90a888f577e31cec86cbb76d496
    tar xzf "$FILE_NAME"

    # Delete the now-useless archive to save space
    rm -rf "$FILE_NAME"

    # Change the name of the directory to remove the hash.
    mv "$PROJECT_NAME"-"$HASH" "$PROJECT_NAME"

    # Enter the directory and configure it with CMake so that the compile_commands.json file is present
    pushd "$PROJECT_NAME"
    cmake -GNinja                             \
          -Bbuild                             \
          -DCMAKE_BUILD_TYPE=Release          \
          -DCMAKE_EXPORT_COMPILE_COMMANDS=1

    # Copy .hdoc.toml file for this repository.
    cp ../../corpus-tomls/"$PROJECT_NAME".hdoc.toml .hdoc.toml
    popd
done
popd
