#include "MonitorController.h"

MonitorController::MonitorController(SampleRepository& sampleRepo,
                                     OrderRepository&  orderRepo,
                                     MonitorView&      view)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), view_(view)
{
}

void MonitorController::Run() {
    view_.Render();
    view_.showInlineMenu();
    int choice = view_.GetMenuChoice();
    while (choice != 0) {
        view_.Render();
        HandleInput(choice);
        view_.showInlineMenu();
        choice = view_.GetMenuChoice();
    }
}

void MonitorController::showDashboard() {
    onDashboard();
}

void MonitorController::HandleInput(int command) {
    switch (command) {
    case 1: onDashboard(); break;
    case 2: onStockOnly(); break;
    default:
        view_.ShowMessage("잘못된 입력입니다.");
        break;
    }
}

void MonitorController::onDashboard() {
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    OrderSummary summary;
    for (const auto& o : orders) {
        switch (o.getStatus()) {
        case OrderStatus::RESERVED:  summary.reserved++;  break;
        case OrderStatus::PRODUCING: summary.producing++; break;
        case OrderStatus::CONFIRMED: summary.confirmed++; break;
        case OrderStatus::RELEASE:   summary.release++;   break;
        case OrderStatus::REJECTED:
        default: break;
        }
    }

    std::vector<StockInfo> stockList;
    for (const auto& s : samples) {
        int needed = 0;
        for (const auto& o : orders) {
            if (o.getSampleId() == s.getId()) {
                OrderStatus st = o.getStatus();
                if (st == OrderStatus::RESERVED || st == OrderStatus::PRODUCING)
                    needed += o.getQuantity();
            }
        }

        StockState state;
        if (s.getStock() == 0)
            state = StockState::DEPLETED;
        else if (s.getStock() < needed)
            state = StockState::SHORTAGE;
        else
            state = StockState::SUFFICIENT;

        int yieldPct;
        if (needed == 0 && s.getStock() > 0)
            yieldPct = 100;
        else if (needed == 0 && s.getStock() == 0)
            yieldPct = 0;
        else
            yieldPct = s.getStock() * 100 / (s.getStock() + needed);

        StockInfo info;
        info.sampleName = s.getName();
        info.stock      = s.getStock();
        info.state      = state;
        info.yieldPct   = yieldPct;
        stockList.push_back(info);
    }

    view_.showDashboard(summary, stockList);
}

void MonitorController::onStockOnly() {
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    std::vector<StockInfo> stockList;
    for (const auto& s : samples) {
        int needed = 0;
        for (const auto& o : orders) {
            if (o.getSampleId() == s.getId()) {
                OrderStatus st = o.getStatus();
                if (st == OrderStatus::RESERVED || st == OrderStatus::PRODUCING)
                    needed += o.getQuantity();
            }
        }

        StockState state;
        if (s.getStock() == 0)
            state = StockState::DEPLETED;
        else if (s.getStock() < needed)
            state = StockState::SHORTAGE;
        else
            state = StockState::SUFFICIENT;

        int yieldPct;
        if (needed == 0 && s.getStock() > 0)
            yieldPct = 100;
        else if (needed == 0)
            yieldPct = 0;
        else
            yieldPct = s.getStock() * 100 / (s.getStock() + needed);

        StockInfo info;
        info.sampleName = s.getName();
        info.stock      = s.getStock();
        info.state      = state;
        info.yieldPct   = yieldPct;
        stockList.push_back(info);
    }

    view_.showStockOnly(stockList);
}
