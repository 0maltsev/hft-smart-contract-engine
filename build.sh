#!/bin/bash
set -e

LLVM_PATH=$(brew --prefix llvm@18)/lib/cmake/llvm

# Собираем WAVM
cd third_party/wavm
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DLLVM_DIR="$LLVM_PATH"
cmake --build build --target WAVM

cd ../..
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.logicalcpu)