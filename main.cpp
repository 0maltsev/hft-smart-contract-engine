#include "src/Engine.cpp"

int main() {
    Engine engine;

    // Загружаем все контракты
    engine.load_all_contracts("contracts/");

    // Эмуляция рыночных данных
    double prices[] = {99.5, 100.5, 101.0};
    std::string symbol = "buy_contract";

    for (double p : prices) {
        engine.on_market_data(symbol, p);
    }

    return 0;
}
