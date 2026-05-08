#pragma once
#include "../model/Sample.h"
#include "../model/Order.h"
#include <string>

inline Sample makeSample(const std::string& id   = "S-001",
                         const std::string& name = "테스트 시료",
                         double avgTime           = 1.0,
                         double yield             = 0.9,
                         int    stock             = 100) {
    return Sample(id, name, avgTime, yield, stock);
}

inline Order makeOrder(const std::string& id       = "ORD-20260101-0001",
                       const std::string& sampleId = "S-001",
                       const std::string& customer = "테스트 고객",
                       int qty                      = 50) {
    return Order(id, sampleId, customer, qty);
}
