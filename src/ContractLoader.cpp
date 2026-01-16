#include "Contract.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string>

#include "m3_api_wasi.h"
#include "m3_env.h"

namespace ContractLoader {

using ContractTable = std::unordered_map<std::string, std::unique_ptr<Contract>>;
ContractTable contracts_;

struct WasmContract {
    IM3Environment env;
    IM3Runtime runtime;
    IM3Module module;
    IM3Function fn;
};

std::vector<uint8_t> read_file(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Failed to open " + path);

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

Contract::FnPtr load_wasm_as_fnptr(const std::string& wasm_path) {
    auto wasm_bytes = ContractLoader::read_file(wasm_path);

    IM3Environment env = m3_NewEnvironment();
    if (!env) throw std::runtime_error("Failed to create Wasm environment");

    IM3Runtime runtime = m3_NewRuntime(env, 64*1024, nullptr);
    if (!runtime) throw std::runtime_error("Failed to create Wasm runtime");

    IM3Module module;
    M3Result result = m3_ParseModule(env, &module, wasm_bytes.data(), wasm_bytes.size());
    if (result) throw std::runtime_error("Failed to parse WASM: " + std::string(result));

    result = m3_LoadModule(runtime, module);
    if (result) throw std::runtime_error("Failed to load WASM module: " + std::string(result));

    IM3Function fn;
    result = m3_FindFunction(&fn, runtime, "on_market_update");
    if (result) throw std::runtime_error("Failed to find function on_market_update: " + std::string(result));

    // создаём адаптер для Contract::FnPtr
    return [runtime, fn](double price) -> int32_t {
        // подготовка аргументов
        uint32_t arg = static_cast<uint32_t>(price);  // если функция принимает uint32_t
        const void* args[1] = { &arg };

        // вызов WASM-функции
        M3Result res = m3_Call(fn, 1, args);
        if (res) {
            std::cerr << "WASM call failed: " << res << std::endl;
            return 0;
        }

        // получение результата
        uint32_t ret = 0;
        const void* retPtrs[1] = { &ret };  // массив указателей на возвращаемые значения
        res = m3_GetResults(fn, 1, retPtrs);  // 1 = количество возвращаемых значений
        if (res) {
            std::cerr << "WASM get result failed: " << res << std::endl;
            return 0;
        }

        return static_cast<int32_t>(ret);
    };
}



void load_contracts_from_folder(const std::string& folder_path) {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(folder_path)) {
        if (entry.path().extension() == ".wasm") {
            std::string symbol = entry.path().stem().string();
            contracts_[symbol] = std::make_unique<Contract>(load_wasm_as_fnptr(entry.path().string()));
            std::cout << "Loaded WASM contract: " << symbol << std::endl;
        }
    }
}

Contract* get_contract(const std::string& symbol) {
    auto it = contracts_.find(symbol);
    return (it != contracts_.end()) ? it->second.get() : nullptr;
}

}
