#!/bin/bash

set -euo pipefail

mkdir -p build && cd build

cmake ..

cmake --build .

ctest -j $(nproc)

cpack -G DEB 