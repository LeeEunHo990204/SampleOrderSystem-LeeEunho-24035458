// SampleView.cpp — Phase 2: 시료 관리 서브메뉴 뷰
#include "SampleView.h"
#include <iostream>
#include <iomanip>

static const char* CYAN      = "\033[96m";
static const char* YELLOW    = "\033[93m";
static const char* GRAY      = "\033[90m";
static const char* BRIGHT_RED = "\033[91m";
static const char* RESET     = "\033[0m";

static const int PAGE_SIZE = 5;

void SampleView::Render() {
    std::cout << "\033[2J\033[H";
    std::cout << " [1] 시료 관리\n";
    std::cout << GRAY << " ================================================\n" << RESET;
    std::cout << " " << CYAN << "[1]" << RESET << " 시료 등록    "
              << CYAN << "[2]" << RESET << " 시료 목록    "
              << CYAN << "[3]" << RESET << " 시료 검색    "
              << CYAN << "[0]" << RESET << " 뒤로\n";
}

void SampleView::showSampleList(const std::vector<Sample>& samples, int page) {
    if (samples.empty()) {
        ShowMessage("등록된 시료가 없습니다.");
        return;
    }

    int total    = static_cast<int>(samples.size());
    int start    = page * PAGE_SIZE;
    int end      = std::min(start + PAGE_SIZE, total);
    int lastPage = (total - 1) / PAGE_SIZE;

    std::cout << GRAY << " ------------------------------------------------\n" << RESET;
    std::cout << " 등록 시료 목록 (총 " << total << "종)  ("
              << (page + 1) << "/" << (lastPage + 1) << " 페이지)\n";
    std::cout << GRAY << " ------------------------------------------------\n" << RESET;
    std::cout << std::left
              << " " << std::setw(10) << "ID"
              << std::setw(24) << "시료명"
              << std::setw(10) << "생산시간"
              << std::setw(8)  << "수율"
              << "재고\n";
    std::cout << GRAY << " ------------------------------------------------\n" << RESET;

    for (int i = start; i < end; ++i) {
        const Sample& s = samples[i];
        bool zeroStock  = (s.getStock() == 0);

        if (zeroStock) std::cout << BRIGHT_RED;

        std::cout << " " << std::left  << std::setw(10) << s.getId()
                  << std::setw(24) << s.getName()
                  << std::fixed << std::setprecision(1)
                  << std::right << std::setw(6) << s.getAvgProductionTime() << "h    "
                  << std::setw(5) << (s.getYieldRate() * 100.0) << "%  "
                  << std::setw(6) << s.getStock() << " ea\n";

        if (zeroStock) std::cout << RESET;
    }

    std::cout << GRAY << " ------------------------------------------------\n" << RESET;

    if (page < lastPage) {
        std::cout << " " << CYAN << "[N]" << RESET << " 다음페이지  "
                  << CYAN << "[0]" << RESET << " 뒤로\n";
    }
}

std::string SampleView::inputId() {
    return inputString(" 시료 ID (예: S-001): ");
}

std::string SampleView::inputName() {
    return inputString(" 시료 이름: ");
}

double SampleView::inputAvgTime() {
    return inputDouble(" 평균 생산시간 (시간): ");
}

double SampleView::inputYieldRate() {
    return inputDouble(" 수율 (0.0 ~ 1.0): ");
}

std::string SampleView::inputSearchName() {
    return inputString(" 검색할 이름: ");
}
