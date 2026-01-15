#pragma once

#include "Contract.h"
#include <string>

namespace ContractLoader {

void initLLVM();

void load_contracts_from_folder(const std::string& folder);

Contract* get_contract(const std::string& symbol);

}
