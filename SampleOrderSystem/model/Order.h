#pragma once
#include <string>
#include <stdexcept>
#include "OrderStatus.h"

class Order {
public:
    Order(std::string id, std::string sampleId, std::string customerName, int quantity);

    const std::string& getId()                   const;
    const std::string& getSampleId()             const;
    const std::string& getCustomerName()         const;
    int                getQuantity()             const;
    OrderStatus        getStatus()               const;
    int                getProducedQty()          const;
    long long          getProductionStartedAt()  const;
    const std::string& getReleasedAt()           const;

    void setStatus(OrderStatus status);
    void setProducedQty(int qty);
    void setProductionStartedAt(long long t);
    void setReleasedAt(const std::string& dateTime);

private:
    std::string id_;
    std::string sampleId_;
    std::string customerName_;
    int         quantity_;
    OrderStatus status_;
    int         producedQty_;
    long long   productionStartedAt_;
    std::string releasedAt_;
};
