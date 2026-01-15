#pragma once
#include <cstdint>

class Contract {
public:
    using FnPtr = int32_t(*)(double);

    explicit Contract(FnPtr fn) : fn_(fn) {}

    inline int32_t execute(double price) const {
        return fn_(price);  // прямой вызов, zero-overhead
    }

private:
    FnPtr fn_;
};