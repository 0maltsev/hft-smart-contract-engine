#pragma once
#include <string>
#include <cstdint>

class ContractRegistry;

class MarketDataHandler {
public:
    explicit MarketDataHandler(ContractRegistry& registry);

    void on_packet(const void* data, size_t size);

private:
    ContractRegistry& registry_;

    inline std::string_view parse_symbol(const char* packet) const;
    inline double parse_price(const char* packet) const;
};