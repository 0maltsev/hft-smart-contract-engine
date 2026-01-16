#pragma once

#include "Contract.h"
#include <string>

namespace ContractLoader {
    Contract* get_contract(const std::string& symbol);
    void load_contracts_from_folder(const std::string& folder_path);
    std::unique_ptr<Contract> load_contract_from_wasm(const std::string& wasm_path);
}