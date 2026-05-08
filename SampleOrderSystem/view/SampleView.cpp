// SampleView.cpp — Phase 2: 시료 관리 서브메뉴 뷰
#include "SampleView.h"
#include "ConsoleColors.h"
#include <iostream>
#include <iomanip>
#include <sstream>

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

    static const int COL_ID    = 10;
    static const int COL_NAME  = 24;
    static const int COL_TIME  = 20;
    static const int COL_YIELD = 10;
    static const char* SEP     = " ------------------------------------------------------------------\n";

    std::cout << GRAY << SEP << RESET;
    std::cout << " 등록 시료 목록 (총 " << total << "종)  ("
              << (page + 1) << "/" << (lastPage + 1) << " 페이지)\n";
    std::cout << GRAY << SEP << RESET;
    std::cout << " ";
    padTo("ID",               COL_ID);
    padTo("시료명",           COL_NAME);
    padTo("생산시간(min/ea)", COL_TIME);
    padTo("수율",             COL_YIELD);
    std::cout << "재고\n";
    std::cout << GRAY << SEP << RESET;

    std::ostringstream oss;
    for (int i = start; i < end; ++i) {
        const Sample& s = samples[i];
        bool zeroStock  = (s.getStock() == 0);

        if (zeroStock) std::cout << RED;

        std::cout << " ";
        padTo(s.getId(), COL_ID);
        padTo(s.getName(), COL_NAME);

        oss.str(""); oss.clear();
        oss << std::fixed << std::setprecision(1) << s.getAvgProductionTime() << " min/ea";
        padTo(oss.str(), COL_TIME);

        oss.str(""); oss.clear();
        oss << std::fixed << std::setprecision(1) << (s.getYieldRate() * 100.0) << "%";
        padTo(oss.str(), COL_YIELD);

        std::cout << std::setw(6) << s.getStock() << " ea\n";

        if (zeroStock) std::cout << RESET;
    }

    std::cout << GRAY << SEP << RESET;

    if (page < lastPage) {
        std::cout << " " << CYAN << "[N]" << RESET << " 다음페이지  "
                  << CYAN << "[0]" << RESET << " 뒤로\n";
    }
}

void SampleView::showInlineMenu() {
    std::cout << "\n " << CYAN << "[1]" << RESET << " 시료 등록    "
              << CYAN << "[2]" << RESET << " 시료 목록    "
              << CYAN << "[3]" << RESET << " 시료 검색    "
              << CYAN << "[0]" << RESET << " 뒤로\n";
}

std::string SampleView::inputId() {
    return inputString(" 시료 ID: ");
}

std::string SampleView::inputName() {
    return inputString(" 시료 이름: ");
}

double SampleView::inputAvgTime() {
    return inputDouble(" 평균 생산시간 (min/ea): ");
}

double SampleView::inputYieldRate() {
    return inputDouble(" 수율 (0.0 ~ 1.0): ");
}

std::string SampleView::inputSearchName() {
    return inputString(" 검색어 (ID 또는 이름): ");
}
