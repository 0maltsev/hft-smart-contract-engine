#pragma once
#include <cstdint>
#include <functional>   // <- добавлено для std::function

class Contract {
public:
    using FnPtr = std::function<int(double)>;  // лямбда -> std::function

    explicit Contract(FnPtr fn) : fn_(fn) {}

    inline int32_t execute(double price) const {
        return fn_(price);  // прямой вызов
    }

private:
    FnPtr fn_;
};
