#pragma once
#include "../view/ConsoleView.h"
#include <vector>
#include <string>

struct OrderSummary {
    int reserved  = 0;
    int producing = 0;
    int confirmed = 0;
    int release   = 0;
};

enum class StockState { SUFFICIENT, SHORTAGE, DEPLETED };

struct StockInfo {
    std::string sampleName;
    int         stock;
    StockState  state;
    int         yieldPct;
};

class MonitorView : public ConsoleView {
public:
    void Render() override;
    virtual void showDashboard(const OrderSummary& summary,
                               const std::vector<StockInfo>& stockList);
    virtual void showStockOnly(const std::vector<StockInfo>& stockList);
    void showInlineMenu();

private:
    static std::string stateLabel(StockState s);
    static std::string stateColor(StockState s);
    static std::string makeBar(int pct, int width = 14);
};
