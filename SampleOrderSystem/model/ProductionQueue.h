#pragma once
#include <deque>
#include <string>
#include <stdexcept>
#include "../core/IModel.h"

class ProductionQueue : public IModel {
public:
    void        push(const std::string& orderId);
    std::string front() const;   // 비어 있으면 std::runtime_error 예외
    void        pop();
    bool        empty() const;
    int         size()  const;

    const std::deque<std::string>& getQueue() const;

private:
    std::deque<std::string> queue_;
};
