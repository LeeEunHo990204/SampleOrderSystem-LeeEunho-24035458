#include "Order.h"

Order::Order(std::string id, std::string sampleId, std::string customerName, int quantity)
    : id_(std::move(id))
    , sampleId_(std::move(sampleId))
    , customerName_(std::move(customerName))
    , quantity_(quantity)
    , status_(OrderStatus::RESERVED)
    , producedQty_(0)
    , productionStartedAt_(0)
    , releasedAt_("")
{
}

const std::string& Order::getId()                   const { return id_; }
const std::string& Order::getSampleId()             const { return sampleId_; }
const std::string& Order::getCustomerName()         const { return customerName_; }
int                Order::getQuantity()             const { return quantity_; }
OrderStatus        Order::getStatus()               const { return status_; }
int                Order::getProducedQty()          const { return producedQty_; }
long long          Order::getProductionStartedAt()  const { return productionStartedAt_; }

void Order::setStatus(OrderStatus status) {
    status_ = status;
}

void Order::setProducedQty(int qty) {
    if (qty < 0)
        throw std::invalid_argument("setProducedQty: 생산 수량은 0 이상이어야 합니다.");
    producedQty_ = qty;
}

void Order::setProductionStartedAt(long long t) {
    productionStartedAt_ = t;
}

const std::string& Order::getReleasedAt() const { return releasedAt_; }

void Order::setReleasedAt(const std::string& dateTime) {
    releasedAt_ = dateTime;
}
