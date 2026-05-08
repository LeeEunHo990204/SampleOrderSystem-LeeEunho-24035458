#pragma once
#include "ConsoleView.h"
#include "../model/Order.h"
#include "../model/Sample.h"
#include <vector>
#include <string>

class ProductionView : public ConsoleView {
public:
    void Render() override;

    virtual void showProductionStatus(const Order& current,
                                      const std::vector<Order>& waiting,
                                      const std::vector<Sample>& samples);
    virtual void showEmptyQueue();
    virtual void showProcessingResult(const Order& order, const Sample& sample,
                                      int shortage, int actualQty, double totalTime);
    virtual void showInlineMenu();

private:
    static std::string makeProgressBar(int pct, int width = 16);
    static std::string estimatedTime(double minutes);
};
