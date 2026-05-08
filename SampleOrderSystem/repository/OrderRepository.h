#pragma once
#include <string>
#include <vector>
#include "../model/Order.h"
#include "../util/JsonValue.h"

class OrderRepository {
public:
    explicit OrderRepository(const std::string& filePath);

    std::vector<Order> loadAll();
    void saveAll(const std::vector<Order>& orders);

private:
    std::string filePath_;

    Order     fromJson(const JsonValue& obj) const;
    JsonValue toJson(const Order& o)         const;
};
