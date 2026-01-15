#include "Contract.h"
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include <iostream>


namespace ContractLoader {

    using ContractTable = std::unordered_map<std::string, std::unique_ptr<Contract>>;
    ContractTable contracts_;

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

    std::unique_ptr<Contract> load_contract_from_wasm(const std::string& wasm_path) {
        //Wasm -> LLVM IR -> native
        Contract::FnPtr fn_ptr = nullptr; // заглушка
        return std::make_unique<Contract>(fn_ptr);
    }

    void load_contracts_from_folder(const std::string& folder_path) {
        namespace fs = std::filesystem;
        for (const auto& entry : fs::directory_iterator(folder_path)) {
            if (entry.path().extension() == ".wasm") {
                std::string symbol = entry.path().stem().string();
                contracts_[symbol] = load_contract_from_wasm(entry.path().string());
                std::cout << "Loaded contract for symbol: " << symbol << std::endl;
            }
        }
    }

    Contract* get_contract(const std::string& symbol) {
        auto it = contracts_.find(symbol);
        return (it != contracts_.end()) ? it->second.get() : nullptr;
    }

}
