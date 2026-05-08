#pragma once
#include <string>
#include <stdexcept>

enum class OrderStatus {
    RESERVED,   // 주문 접수
    REJECTED,   // 주문 거절
    PRODUCING,  // 생산 중
    CONFIRMED,  // 출고 대기
    RELEASE     // 출고 완료
};

inline std::string toString(OrderStatus status) {
    switch (status) {
    case OrderStatus::RESERVED:  return "RESERVED";
    case OrderStatus::REJECTED:  return "REJECTED";
    case OrderStatus::PRODUCING: return "PRODUCING";
    case OrderStatus::CONFIRMED: return "CONFIRMED";
    case OrderStatus::RELEASE:   return "RELEASE";
    }
    return "UNKNOWN";
}

inline OrderStatus fromString(const std::string& str) {
    if (str == "RESERVED")  return OrderStatus::RESERVED;
    if (str == "REJECTED")  return OrderStatus::REJECTED;
    if (str == "PRODUCING") return OrderStatus::PRODUCING;
    if (str == "CONFIRMED") return OrderStatus::CONFIRMED;
    if (str == "RELEASE")   return OrderStatus::RELEASE;
    throw std::invalid_argument("Unknown OrderStatus: " + str);
}
