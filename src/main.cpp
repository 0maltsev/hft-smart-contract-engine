#include "Contract.h"
#include "ContractLoader.h"
#include <iostream>
#include <chrono>

enum class Side { Buy, Sell };


class Engine {
public:
    void load_all_contracts(const std::string& folder) {
        ContractLoader::load_contracts_from_folder(folder);
    }

    void on_market_data(const std::string& symbol, double price) {
        Contract* c = ContractLoader::get_contract(symbol);
        if (!c) return;

        auto start = std::chrono::high_resolution_clock::now();
        int32_t action = c->execute(price);  // нативный вызов
        auto end = std::chrono::high_resolution_clock::now();

        if (action == 1) send_order(symbol, Side::Buy, price, 100);
        else if (action == 2) send_order(symbol, Side::Sell, price, 100);

        std::cout << "[Latency ns] " 
                << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()
                << std::endl;
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

int main() {
    Engine engine;

    // Загружаем все LLVM IR контракты из папки
    engine.load_all_contracts("../contracts");

    // Проверяем наличие контракта
    Contract* c = ContractLoader::get_contract("strategy");
    if (!c) {
        std::cerr << "Contract 'strategy' not found!\n";
        return 1;
    }
    std::cout << "Contract 'strategy' loaded successfully!\n";

    // Эмуляция рыночных данных
    double prices[] = {99.5, 100.5, 101.0};
    for (double p : prices) {
        engine.on_market_data("strategy", p);
    }

    return 0;
}
