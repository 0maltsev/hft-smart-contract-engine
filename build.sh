#!/bin/bash
set -e

LLVM_PATH=$(brew --prefix llvm@18)/lib/cmake/llvm

mkdir -p build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR="$LLVM_PATH"

make -j$(sysctl -n hw.logicalcpu)