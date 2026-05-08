#include "MonitorView.h"
#include <iostream>
#include <iomanip>
#include <sstream>

static const char* B_CYAN    = "\033[96m";
static const char* B_GREEN   = "\033[92m";
static const char* B_YELLOW  = "\033[93m";
static const char* B_MAGENTA = "\033[95m";
static const char* B_RED     = "\033[91m";
static const char* GRAY      = "\033[90m";
static const char* RESET     = "\033[0m";

static int dispWidth(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); ) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if      (c < 0x80) { w += 1; i += 1; }
        else if (c < 0xE0) { w += 1; i += 2; }
        else if (c < 0xF0) { w += 2; i += 3; }
        else               { w += 2; i += 4; }
    }
    return w;
}

static void padTo(const std::string& text, int colWidth) {
    std::cout << text;
    int pad = colWidth - dispWidth(text);
    if (pad > 0) std::cout << std::string(pad, ' ');
}

void MonitorView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [4] 모니터링  " << currentDateTime() << "\n";
    std::cout << GRAY << " ================================================\n" << RESET;
}

void MonitorView::showInlineMenu() {
    std::cout << " " << B_CYAN << "[1]" << RESET << " 주문량 확인    "
              << B_CYAN << "[2]" << RESET << " 재고량 확인    "
              << B_CYAN << "[0]" << RESET << " 뒤로";
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
    case StockState::SUFFICIENT: return B_GREEN;
    case StockState::SHORTAGE:   return B_YELLOW;
    case StockState::DEPLETED:   return B_RED;
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

void MonitorView::showDashboard(const OrderSummary& summary,
                                const std::vector<StockInfo>& stockList)
{
    std::cout << "\n 상태별 주문 현황\n";
    std::cout << " " << B_CYAN    << "[RESERVED]" << RESET
              << "  " << summary.reserved  << "건\n";
    std::cout << " " << B_GREEN   << "[CONFIRMED]" << RESET
              << "  " << summary.confirmed << "건\n";
    std::cout << " " << B_YELLOW  << "[PRODUCING]" << RESET
              << "  " << summary.producing << "건  " << GRAY << "\xe2\x86\x90 생산라인 대기" << RESET << "\n";
    std::cout << " " << B_MAGENTA << "[RELEASE]" << RESET
              << "   " << summary.release << "건\n";

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

void MonitorView::showStockOnly(const std::vector<StockInfo>& stockList) {
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
