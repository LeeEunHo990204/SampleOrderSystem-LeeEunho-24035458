#include "Sample.h"

Sample::Sample(std::string id, std::string name, double avgProductionTime, double yieldRate, int stock)
    : id_(std::move(id))
    , name_(std::move(name))
    , avgProductionTime_(avgProductionTime)
    , yieldRate_(yieldRate)
    , stock_(stock)
{
}

const std::string& Sample::getId()                const { return id_; }
const std::string& Sample::getName()              const { return name_; }
double             Sample::getAvgProductionTime() const { return avgProductionTime_; }
double             Sample::getYieldRate()         const { return yieldRate_; }
int                Sample::getStock()             const { return stock_; }

void Sample::addStock(int qty) {
    if (qty < 0)
        throw std::invalid_argument("addStock: 수량은 0 이상이어야 합니다.");
    stock_ += qty;
}

void Sample::reduceStock(int qty) {
    if (qty < 0)
        throw std::invalid_argument("reduceStock: 수량은 0 이상이어야 합니다.");
    if (qty > stock_)
        throw std::invalid_argument("reduceStock: 재고가 부족합니다.");
    stock_ -= qty;
}
