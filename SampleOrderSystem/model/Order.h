#pragma once
#include <string>
#include <stdexcept>
#include "OrderStatus.h"

class Order {
public:
    Order(std::string id, std::string sampleId, std::string customerName, int quantity);

    const std::string& getId()           const;
    const std::string& getSampleId()     const;
    const std::string& getCustomerName() const;
    int                getQuantity()     const;
    OrderStatus        getStatus()       const;
    int                getProducedQty()  const;

    void setStatus(OrderStatus status);
    void setProducedQty(int qty);

private:
    std::string id_;
    std::string sampleId_;
    std::string customerName_;
    int         quantity_;
    OrderStatus status_;
    int         producedQty_;
};
