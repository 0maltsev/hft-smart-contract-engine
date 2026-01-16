#!/bin/bash
set -e

mkdir -p ../build
cd ../build

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.logicalcpu)