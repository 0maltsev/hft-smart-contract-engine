cmake_minimum_required(VERSION 3.20)
project(hft-engine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------------
# LLVM (Homebrew)
# -------------------------------
execute_process(
    COMMAND brew --prefix llvm@18
    OUTPUT_VARIABLE LLVM_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(LLVM_CONFIG "${LLVM_PREFIX}/bin/llvm-config")

execute_process(
    COMMAND ${LLVM_CONFIG} --cxxflags
    OUTPUT_VARIABLE LLVM_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG} --libs
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND ${LLVM_CONFIG} --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

separate_arguments(LLVM_CXXFLAGS)
separate_arguments(LLVM_LDFLAGS)
separate_arguments(LLVM_LIBS)
separate_arguments(LLVM_SYSTEM_LIBS)

# -------------------------------
# Исходники
# -------------------------------
set(HFT_SRC
    src/main.cpp
    src/Engine.cpp
    src/ContractLoader.cpp
    src/MarketDataHandler.cpp
)

add_executable(hft-engine ${HFT_SRC})

target_include_directories(hft-engine PRIVATE
    include
    ${LLVM_PREFIX}/include
)

target_compile_options(hft-engine PRIVATE
    ${LLVM_CXXFLAGS}
    -O3
    -march=native
    -Wall -Wextra -Wpedantic
)

target_link_options(hft-engine PRIVATE
    ${LLVM_LDFLAGS}
)

target_link_libraries(hft-engine PRIVATE
    ${LLVM_LIBS}
    ${LLVM_SYSTEM_LIBS}
)
