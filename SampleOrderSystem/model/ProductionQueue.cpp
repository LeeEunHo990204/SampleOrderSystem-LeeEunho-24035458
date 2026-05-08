// ProductionQueue.cpp
#include "ProductionQueue.h"

void ProductionQueue::push(const std::string& orderId) {
    queue_.push_back(orderId);
}

std::string ProductionQueue::front() const {
    if (queue_.empty())
        throw std::runtime_error("ProductionQueue is empty");
    return queue_.front();
}

void ProductionQueue::pop() {
    if (!queue_.empty())
        queue_.pop_front();
}

bool ProductionQueue::empty() const {
    return queue_.empty();
}

int ProductionQueue::size() const {
    return static_cast<int>(queue_.size());
}

const std::deque<std::string>& ProductionQueue::getQueue() const {
    return queue_;
}
