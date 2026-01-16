cmake_minimum_required(VERSION 3.20)
project(hft-engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Исходники проекта
# -------------------------------
file(GLOB_RECURSE HFT_SRC
    src/*.cpp
)

add_executable(hft-engine ${HFT_SRC})
target_include_directories(hft-engine PRIVATE include)

# -------------------------------
# WASM3
# -------------------------------
file(GLOB WASM3_SRC
    third_party/wasm3/source/*.c
)

# Явно указываем, что это C-библиотека
add_library(m3 STATIC ${WASM3_SRC})
set_target_properties(m3 PROPERTIES LINKER_LANGUAGE C)
target_include_directories(m3 PUBLIC third_party/wasm3/source)

# hft-engine зависит от m3
add_dependencies(hft-engine m3)
target_link_libraries(hft-engine PRIVATE m3)

# -------------------------------
# Компиляционные флаги
# -------------------------------
target_compile_options(hft-engine PRIVATE
    -O3
    -Wall
    -Wextra
    -Wpedantic
)

# macOS: чтобы не ругался на nproc
if(APPLE)
    set(ENV{MAKEFLAGS} "-j$(sysctl -n hw.ncpu)")
endif()
