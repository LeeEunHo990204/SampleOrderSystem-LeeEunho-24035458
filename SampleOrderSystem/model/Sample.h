#pragma once
#include <string>
#include <stdexcept>

class Sample {
public:
    Sample(std::string id, std::string name, double avgProductionTime, double yieldRate, int stock);

    const std::string& getId()                const;
    const std::string& getName()              const;
    double             getAvgProductionTime() const;
    double             getYieldRate()         const;
    int                getStock()             const;

    void addStock(int qty);
    void reduceStock(int qty);

private:
    std::string id_;
    std::string name_;
    double      avgProductionTime_;
    double      yieldRate_;
    int         stock_;
};
