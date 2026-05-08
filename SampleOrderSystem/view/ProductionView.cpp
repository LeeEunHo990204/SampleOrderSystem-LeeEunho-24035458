// ProductionView.cpp
#include "ProductionView.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cmath>
#include <algorithm>

// ---------------------------------------------------------------------------
// Render — "[5] 생산라인 조회" 헤더
// ---------------------------------------------------------------------------
void ProductionView::Render() {
    std::cout << "\n [5] 생산라인 조회  FIFO 방식\n";
    std::cout << " ================================================\n\n";
    std::cout << " 생산라인 1개 (단일 라인)  현재 상태: \033[93m[RUNNING]\033[0m\n\n";
}

// ---------------------------------------------------------------------------
// showProductionStatus — 현재 처리 중 박스 + 대기 큐 테이블
// ---------------------------------------------------------------------------
void ProductionView::showProductionStatus(const Order& current,
                                           const std::vector<Order>& waiting,
                                           const std::vector<Sample>& samples) {
    // 현재 처리 중인 시료 찾기
    auto findSample = [&](const std::string& sampleId) -> const Sample* {
        for (const auto& s : samples)
            if (s.getId() == sampleId) return &s;
        return nullptr;
    };

    const Sample* curSample = findSample(current.getSampleId());
    if (!curSample) return;

    int    stock     = curSample->getStock();
    int    qty       = current.getQuantity();
    int    shortage  = std::max(0, qty - stock);
    int    actualQty = (shortage > 0)
                       ? static_cast<int>(std::ceil(
                             static_cast<double>(shortage)
                             / (curSample->getYieldRate() * 0.9)))
                       : 0;
    double totalTime = curSample->getAvgProductionTime() * actualQty;

    // 진행률: productionStartedAt 기준 경과 시간으로 계산
    int pct = 0;
    long long startTime = current.getProductionStartedAt();
    if (startTime > 0 && totalTime > 0.0) {
        long long now = static_cast<long long>(std::time(nullptr));
        double elapsedMinutes = (now - startTime) / 60.0;
        pct = std::min(99, static_cast<int>(elapsedMinutes / totalTime * 100));
    }
    std::string bar = makeProgressBar(pct);
    double remainingTime = (totalTime > 0.0 && startTime > 0)
        ? std::max(0.0, totalTime - (static_cast<long long>(std::time(nullptr)) - startTime) / 60.0)
        : totalTime;
    std::string eta = estimatedTime(remainingTime);

    // box drawing helper: repeat UTF-8 box char
    auto repeatStr = [](const std::string& s, int n) -> std::string {
        std::string result;
        result.reserve(s.size() * n);
        for (int i = 0; i < n; ++i) result += s;
        return result;
    };
    const std::string hLine = "\xe2\x94\x80";  // ─

    std::cout << " 현재 처리 중\n";
    std::cout << " \xe2\x94\x8c" << repeatStr(hLine, 62) << "\xe2\x94\x90\n";
    std::cout << " \xe2\x94\x82 주문번호  " << std::setw(22) << std::left << current.getId()
              << " 시료  " << std::setw(24) << std::left << curSample->getName() << "\xe2\x94\x82\n";
    std::cout << " \xe2\x94\x82 주문량  " << qty << " ea"
              << "   재고  " << stock << " ea"
              << " -> 부족  " << shortage << " ea"
              << " -> 실생산량 " << actualQty << " ea" << "    \xe2\x94\x82\n";
    std::cout << " \xe2\x94\x82          (수율 " << std::fixed << std::setprecision(2)
              << curSample->getYieldRate()
              << " / " << (int)curSample->getAvgProductionTime() << " min)"
              << "                            \xe2\x94\x82\n";
    std::cout << " \xe2\x94\x82 진행     " << bar << " " << pct << "%"
              << "   완료 예정  " << eta << "            \xe2\x94\x82\n";
    std::cout << " \xe2\x94\x94" << repeatStr(hLine, 62) << "\xe2\x94\x98\n\n";

    // 대기 큐 테이블
    std::cout << " 대기 중인 주문  (FIFO 순)\n";
    if (waiting.empty()) {
        std::cout << " (대기 중인 주문 없음)\n\n";
    } else {
        std::cout << " " << std::setw(4) << "순서"
                  << "  " << std::setw(22) << std::left << "주문번호"
                  << "  " << std::setw(22) << std::left << "시료"
                  << "  " << std::setw(8) << "주문량"
                  << "  " << std::setw(8) << "부족분"
                  << "  " << std::setw(8) << "실생산량"
                  << "  " << "완료예정\n";

        double cumulativeTime = totalTime;
        int idx = 1;
        for (const auto& wo : waiting) {
            const Sample* ws = findSample(wo.getSampleId());
            if (!ws) { ++idx; continue; }

            int    ws_shortage  = std::max(0, wo.getQuantity() - ws->getStock());
            int    ws_actual    = (ws_shortage > 0)
                                  ? static_cast<int>(std::ceil(
                                        static_cast<double>(ws_shortage)
                                        / (ws->getYieldRate() * 0.9)))
                                  : wo.getQuantity();
            double ws_time      = ws->getAvgProductionTime() * ws_actual;
            cumulativeTime     += ws_time;
            std::string ws_eta  = estimatedTime(cumulativeTime);

            std::cout << " " << std::setw(4) << std::right << idx
                      << "  " << std::setw(22) << std::left << wo.getId()
                      << "  " << std::setw(22) << std::left << ws->getName()
                      << "  " << std::setw(5) << wo.getQuantity() << " ea"
                      << "  " << std::setw(5) << ws_shortage << " ea"
                      << "  " << std::setw(5) << ws_actual << " ea"
                      << "  " << ws_eta << "\n";
            ++idx;
        }
        std::cout << "\n";
    }

    std::cout << " * 부족분 = 주문량 - 재고,  실생산량 = ceil(부족분 / (수율 × 0.9))\n";
    std::cout << " * 선입선출(FIFO) 방식으로 처리됩니다.\n";
}

// ---------------------------------------------------------------------------
// showEmptyQueue
// ---------------------------------------------------------------------------
void ProductionView::showEmptyQueue() {
    std::cout << "\n 대기 중인 생산 주문이 없습니다.\n\n";
}

// ---------------------------------------------------------------------------
// showProcessingResult
// ---------------------------------------------------------------------------
void ProductionView::showProcessingResult(const Order& order, const Sample& sample,
                                           int shortage, int actualQty, double totalTime) {
    std::cout << "\n ── 생산 처리 완료 ───────────────────────────────\n";
    std::cout << "  주문번호   " << order.getId() << "\n";
    std::cout << "  시료       " << sample.getName() << "\n";
    std::cout << "  부족분     " << shortage << " ea\n";
    std::cout << "  실 생산량  " << actualQty << " ea"
              << "  (= ceil(" << shortage
              << " / (" << std::fixed << std::setprecision(2) << sample.getYieldRate()
              << " × 0.9)))\n";
    std::cout << "  총 생산시간 " << std::fixed << std::setprecision(1) << totalTime << " min\n";
    std::cout << " ─────────────────────────────────────────────\n";
    std::cout << " [알림] 생산 완료 — 출고 대기(CONFIRMED) 전환\n\n";
}

// ---------------------------------------------------------------------------
// showInlineMenu
// ---------------------------------------------------------------------------
void ProductionView::showInlineMenu() {
    std::cout << " [1] 생산 처리  [0] 뒤로\n";
}

// ---------------------------------------------------------------------------
// makeProgressBar
// ---------------------------------------------------------------------------
std::string ProductionView::makeProgressBar(int pct, int width) {
    int filled = (pct * width) / 100;
    std::string bar = "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled)
            bar += "\xe2\x96\x88";  // █
        else
            bar += "\xe2\x96\x91";  // ░
    }
    bar += "]";
    return bar;
}

// ---------------------------------------------------------------------------
// estimatedTime — 현재 시각 + minutes → "HH:MM"
// ---------------------------------------------------------------------------
std::string ProductionView::estimatedTime(double minutes) {
    std::time_t now = std::time(nullptr);
    std::time_t future = now + static_cast<std::time_t>(minutes * 60.0);
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &future);
#else
    localtime_r(&future, &t);
#endif
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << t.tm_hour
        << ":"
        << std::setw(2) << std::setfill('0') << t.tm_min;
    return oss.str();
}
