// // src/MarketDataHandler.cpp
// #include "MarketDataHandler.h"
// #include "Contract.h"
// #include <cstring>

// MarketDataHandler::MarketDataHandler(ContractRegistry& registry)
//     : registry_(registry) {}

// // === HOT-PATH: эта функция должна быть максимально быстрой ===
// void MarketDataHandler::on_packet(const void* data, size_t size) {
//     if (size < MIN_PACKET_SIZE) return;

//     const char* packet = static_cast<const char*>(data);

//     // 1. Парсим symbol (предположим, он в первых 6 байтах)
//     std::string_view symbol = parse_symbol(packet); // zero-copy

//     // 2. Получаем контракт (lock-free lookup)
//     auto* contract = registry_.get_contract(symbol);
//     if (!contract) return;

//     // 3. Парсим цену
//     double price = parse_price(packet);

//     // 4. ВЫЗОВ КОНТРАКТА — главный момент!
//     int32_t action = contract->execute(price); // ← нативный вызов

//     // 5. Передаём результат дальше (например, в OrderSender)
//     if (action != 0) {
//         // Можно вызывать напрямую или ставить в ring buffer
//         handle_action(symbol, price, action);
//     }
// }

// // --- Вспомогательные inline-функции ---

// inline std::string_view MarketDataHandler::parse_symbol(const char* packet) const {
//     // Пример: символ фиксированной длины (6 байт), null-padded
//     constexpr size_t SYMBOL_LEN = 6;
//     const char* end = packet + SYMBOL_LEN;
//     while (end > packet && *(end - 1) == ' ') --end; // trim right
//     return std::string_view(packet, end - packet);
// }

// inline double MarketDataHandler::parse_price(const char* packet) const {
//     // Пример: цена как int64_t (1/1_000_000 USD)
//     // Смещение = 6 байт после символа
//     const int64_t* price_micro = reinterpret_cast<const int64_t*>(packet + 6);
//     return static_cast<double>(*price_micro) / 1'000'000.0;
// }

// void MarketDataHandler::handle_action(
//     std::string_view symbol, double price, int32_t action) {
//     // Передача в OrderSender (реализуется отдельно)
//     // Например:
//     // order_sender_->send_limit_order(symbol, Side::Sell, price + 0.01, 100);
// }