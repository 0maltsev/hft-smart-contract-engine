#include "Contract.h"
#include "ContractLoader.h"
#include <iostream>

enum class Side { Buy, Sell };

class Engine {
public:
    void load_all_contracts(const std::string& folder) {
        ContractLoader::load_contracts_from_folder(folder);
    }

    void on_market_data(const std::string& symbol, double price) {
        Contract* c = ContractLoader::get_contract(symbol);
        if (!c) return;

        int32_t action = c->execute(price);
        if (action == 1) send_order(symbol, Side::Buy, price, 100);
        else if (action == 2) send_order(symbol, Side::Sell, price, 100);
    }

private:
    void send_order(const std::string& symbol, Side side, double price, int qty) {
        std::cout << "Send order: "
                << symbol << " "
                << (side == Side::Buy ? "BUY" : "SELL")
                << " " << qty
                << " @ " << price << std::endl;
    }
};