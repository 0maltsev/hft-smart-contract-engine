cmake_minimum_required(VERSION 3.15)
project(hft-engine LANGUAGES CXX)

# -------------------------------
# Настройки компилятора
# -------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)  # для LLVM/JIT

# -------------------------------
# Внешние зависимости
# -------------------------------
add_subdirectory(third_party/wavm)

# LLVM (для Wasm -> LLVM IR -> native)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)
include(HandleLLVMOptions)
include(LLVMConfig)


include_directories(
    include
    ${CMAKE_SOURCE_DIR}/third_party/wavm/Include
    ${CMAKE_BINARY_DIR}/third_party/wavm/Include
    ${LLVM_INCLUDE_DIRS}
)

add_definitions(${LLVM_DEFINITIONS})


set(HFT_SRC
    src/MarketDataHandler.cpp
    src/ContractLoader.cpp
    src/Engine.cpp
)


add_executable(hft-engine ${HFT_SRC})


target_link_libraries(hft-engine PRIVATE
    WAVM
    WAVMIR
    WAVMRuntime
    ${LLVM_LIBS}
)

target_compile_options(hft-engine PRIVATE
    -Wall -Wextra -Wpedantic -O3 -march=native
)
