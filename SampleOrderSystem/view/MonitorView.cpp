#include "MonitorView.h"
#include "ConsoleColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

void MonitorView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [4] 모니터링  " << currentDateTime() << "\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

void MonitorView::showInlineMenu() {
    std::cout << " " << CYAN << "[1]" << RESET << " 주문량 확인    "
              << CYAN << "[2]" << RESET << " 재고량 확인    "
              << CYAN << "[0]" << RESET << " 뒤로";
}

std::string MonitorView::stateLabel(StockState s) {
    switch (s) {
    case StockState::SUFFICIENT: return "여유";
    case StockState::SHORTAGE:   return "부족";
    case StockState::DEPLETED:   return "고갈";
    }
    return "";
}

std::string MonitorView::stateColor(StockState s) {
    switch (s) {
    case StockState::SUFFICIENT: return GREEN;
    case StockState::SHORTAGE:   return YELLOW;
    case StockState::DEPLETED:   return RED;
    }
    return RESET;
}

std::string MonitorView::makeBar(int pct, int width) {
    int filled = pct * width / 100;
    std::string bar = "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled)
            bar += "\xe2\x96\x88";
        else
            bar += "\xe2\x96\x91";
    }
    bar += "]";
    std::ostringstream pctStr;
    pctStr << std::setw(3) << pct << "%";
    bar += pctStr.str();
    return bar;
}

void MonitorView::renderStockList(const std::vector<StockInfo>& stockList) {
    std::cout << "\n 재고 현황\n";
    std::cout << GRAY;
    padTo(" 시료명", 26);
    padTo("재고", 12);
    padTo("상태", 8);
    std::cout << "잔여율\n" << RESET;

    for (const auto& info : stockList) {
        padTo(" " + info.sampleName, 26);

        std::ostringstream stockStr;
        stockStr << info.stock << " ea";
        padTo(stockStr.str(), 12);

        std::string color = stateColor(info.state);
        std::string label = stateLabel(info.state);
        std::cout << color;
        padTo(label, 8);
        std::cout << RESET;

        std::cout << makeBar(info.yieldPct) << "\n";
    }
    std::cout << "\n";
}

void MonitorView::showDashboard(const OrderSummary& summary,
                                const std::vector<StockInfo>& stockList)
{
    std::cout << "\n 상태별 주문 현황\n";
    std::cout << " " << CYAN    << "[RESERVED]" << RESET
              << "  " << summary.reserved  << "건\n";
    std::cout << " " << GREEN   << "[CONFIRMED]" << RESET
              << "  " << summary.confirmed << "건\n";
    std::cout << " " << YELLOW  << "[PRODUCING]" << RESET
              << "  " << summary.producing << "건  " << GRAY << "\xe2\x86\x90 생산라인 대기" << RESET << "\n";
    std::cout << " " << MAGENTA << "[RELEASE]" << RESET
              << "   " << summary.release << "건\n";

    renderStockList(stockList);
}

void MonitorView::showStockOnly(const std::vector<StockInfo>& stockList) {
    renderStockList(stockList);
}
