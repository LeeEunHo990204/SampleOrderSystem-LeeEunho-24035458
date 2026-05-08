#pragma once
#include <cmath>
#include <algorithm>

constexpr double YIELD_SAFETY_FACTOR = 0.9;

inline int calcShortage(int quantity, int stock) {
    return std::max(0, quantity - stock);
}

inline int calcActualQty(int shortage, double yieldRate) {
    if (shortage <= 0) return 0;
    return static_cast<int>(std::ceil(
        static_cast<double>(shortage) / (yieldRate * YIELD_SAFETY_FACTOR)));
}
