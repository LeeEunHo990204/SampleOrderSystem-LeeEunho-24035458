#include "ConsoleView.h"
#include <iostream>
#include <limits>
#include <ctime>
#include <iomanip>
#include <sstream>

static const char* CYAN   = "\033[96m";
static const char* YELLOW = "\033[93m";
static const char* GRAY   = "\033[90m";
static const char* RESET  = "\033[0m";

std::string ConsoleView::formatNumber(int n) {
    std::string s = std::to_string(n);
    int pos = (int)s.length() - 3;
    while (pos > 0) { s.insert(pos, ","); pos -= 3; }
    return s;
}

std::string ConsoleView::currentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    std::ostringstream oss;
    oss << std::put_time(&t, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void ConsoleView::printAsciiArt() const {
    std::cout << GRAY;
    std::cout << " ┌────┐  ┌────────────────────────────────────────────┐  ┌────┐\n";
    std::cout << " │┌──┐│  │  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  │  │┌──┐│\n";
    std::cout << " ││  ││──┤  └─┘  └─┘  └─┘  └─┘  └─┘  └─┘  └─┘  └─┘  ├──││  ││\n";
    std::cout << " │└──┘│  │  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  ┌─┐  │  │└──┘│\n";
    std::cout << " └────┘  └────────────────────────────────────────────┘  └────┘\n";
    std::cout << RESET;
}

void ConsoleView::printStatusBar(const SystemStats& stats) const {
    std::cout << " 시스템 현황  " << currentDateTime() << "\n\n";
    std::cout << " 등록 시료  "
              << YELLOW << stats.sampleCount << "종" << RESET
              << "    집재고    "
              << YELLOW << formatNumber(stats.totalStock) << " ea" << RESET << "\n";
    std::cout << " 전체 주문  "
              << YELLOW << stats.orderCount << "건" << RESET
              << "    생산라인   "
              << YELLOW << stats.productionQueueCount << "건 대기" << RESET << "\n";
}

void ConsoleView::printMenu() const {
    std::cout << GRAY << " ------------------------------------------------\n" << RESET;
    std::cout << " " << CYAN << "[1]" << RESET << " 시료 관리          "
              << CYAN << "[2]" << RESET << " 시료 주문\n";
    std::cout << " " << CYAN << "[3]" << RESET << " 주문 승인/거절     "
              << CYAN << "[4]" << RESET << " 모니터링\n";
    std::cout << " " << CYAN << "[5]" << RESET << " 생산라인 조회      "
              << CYAN << "[6]" << RESET << " 출고 처리\n";
    std::cout << " " << CYAN << "[0]" << RESET << " 종료\n";
    std::cout << GRAY << " ------------------------------------------------\n" << RESET;
}

void ConsoleView::Render() {
    Render(SystemStats{});
}

void ConsoleView::Render(const SystemStats& stats) {
    std::cout << "\033[2J\033[H";
    printAsciiArt();
    std::cout << "\n 반도체 시료 생산주문관리 시스템\n";
    std::cout << GRAY << " ================================================\n" << RESET;
    printStatusBar(stats);
    printMenu();
}

void ConsoleView::ShowMessage(const std::string& msg) {
    std::cout << "\n " << YELLOW << "[알림]" << RESET << " " << msg << "\n";
}

int ConsoleView::GetMenuChoice() {
    int choice = 0;
    std::cout << "\n선택 > ";
    std::cin >> choice;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        choice = -1;
    }
    return choice;
}

std::string ConsoleView::inputString(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    if (std::cin.peek() == '\n') {
        std::cin.ignore();
    }
    std::getline(std::cin, value);
    return value;
}

int ConsoleView::inputInt(const std::string& prompt) {
    int value = 0;
    std::cout << prompt;
    std::cin >> value;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        value = 0;
    }
    return value;
}

double ConsoleView::inputDouble(const std::string& prompt) {
    double value = 0.0;
    std::cout << prompt;
    std::cin >> value;
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        value = 0.0;
    }
    return value;
}

void ConsoleView::printSeparator() {
    std::cout << GRAY << " ------------------------------------------------\n" << RESET;
}

void ConsoleView::pause() {
    std::cout << "\n 계속하려면 Enter 키를 누르세요...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}
