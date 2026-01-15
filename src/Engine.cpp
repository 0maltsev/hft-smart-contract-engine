#include "Contract.h"
#include <string>
#include <iostream>
#include <filesystem>
#include "ContractLoader.cpp"

enum class Side { Buy, Sell };

class Engine {
public:
    void load_all_contracts(const std::string& folder) {
        ContractLoader::load_contracts_from_folder(folder);
    }

    void on_market_data(const std::string& symbol, double price) {
        auto c = ContractLoader::get_contract(symbol);
        if (!c) return;

        int32_t action = c->execute(price); // zero-overhead
        if (action == 1) {
            send_order(symbol, Side::Sell, price + 0.01, 100);
        }
    }

private:
    void send_order(const std::string&, Side, double, int) {
        // DMA/FIX
    }
};
