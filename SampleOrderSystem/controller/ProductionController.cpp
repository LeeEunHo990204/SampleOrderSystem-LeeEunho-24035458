// ProductionController.cpp — Phase 5
#include "ProductionController.h"
#include <algorithm>
#include <cmath>
#include <ctime>

// ---------------------------------------------------------------------------
// 생성자
// ---------------------------------------------------------------------------
ProductionController::ProductionController(SampleRepository& sampleRepo,
                                           OrderRepository&  orderRepo,
                                           ProductionQueue&  queue,
                                           ProductionView&   view)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), queue_(queue), view_(view)
{
}

// ---------------------------------------------------------------------------
// Run — 조회 → 인라인 메뉴 → 입력 루프
// ---------------------------------------------------------------------------
void ProductionController::Run() {
    bool running = true;
    while (running) {
        view_.Render();
        onShowStatus();
        view_.showInlineMenu();
        int choice = view_.GetMenuChoice();
        if (choice == 0) running = false;
        else HandleInput(choice);
    }
}

// ---------------------------------------------------------------------------
// HandleInput
// ---------------------------------------------------------------------------
void ProductionController::HandleInput(int command) {
    switch (command) {
    case 1: onProcessNext(); break;
    default:
        view_.ShowMessage("잘못된 입력입니다.");
        break;
    }
}

// ---------------------------------------------------------------------------
// 공개 래퍼 — 테스트에서 직접 호출 가능
// ---------------------------------------------------------------------------
void ProductionController::processNext() {
    onProcessNext();
}

void ProductionController::showStatus() {
    onShowStatus();
}

// ---------------------------------------------------------------------------
// autoCompleteFinished — 생산 시간이 경과한 주문을 자동으로 CONFIRMED 처리
// ---------------------------------------------------------------------------
void ProductionController::autoCompleteFinished() {
    while (!queue_.empty()) {
        std::string orderId = queue_.front();
        auto orders  = orderRepo_.loadAll();
        auto samples = sampleRepo_.loadAll();

        auto orderIt = std::find_if(orders.begin(), orders.end(),
            [&orderId](const Order& o) { return o.getId() == orderId; });
        if (orderIt == orders.end()) { queue_.pop(); continue; }

        auto sampleIt = std::find_if(samples.begin(), samples.end(),
            [&orderIt](const Sample& s) { return s.getId() == orderIt->getSampleId(); });
        if (sampleIt == samples.end()) { queue_.pop(); continue; }

        long long startTime = orderIt->getProductionStartedAt();
        if (startTime == 0) break;

        int shortage  = std::max(0, orderIt->getQuantity() - sampleIt->getStock());
        int actualQty = (shortage > 0)
                        ? static_cast<int>(std::ceil(
                              static_cast<double>(shortage)
                              / (sampleIt->getYieldRate() * 0.9)))
                        : 0;
        double totalMinutes = sampleIt->getAvgProductionTime() * actualQty;

        long long now = static_cast<long long>(std::time(nullptr));
        double elapsedMinutes = (now - startTime) / 60.0;

        if (totalMinutes <= 0.0 || elapsedMinutes < totalMinutes) break;

        sampleIt->addStock(actualQty);
        orderIt->setStatus(OrderStatus::CONFIRMED);
        orderIt->setProducedQty(actualQty);

        sampleRepo_.saveAll(samples);
        orderRepo_.saveAll(orders);
        queue_.pop();
    }
}

// ---------------------------------------------------------------------------
// onShowStatus — 현재 큐 현황 표시
// ---------------------------------------------------------------------------
void ProductionController::onShowStatus() {
    autoCompleteFinished();
    if (queue_.empty()) {
        view_.showEmptyQueue();
        return;
    }

    std::string currentId = queue_.front();
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    // 현재 처리 중인 주문 찾기
    auto currentIt = std::find_if(orders.begin(), orders.end(),
        [&currentId](const Order& o) { return o.getId() == currentId; });

    if (currentIt == orders.end()) {
        view_.showEmptyQueue();
        return;
    }

    // 대기 중인 주문 수집 (front 제외, 나머지 큐 순서대로)
    std::vector<Order> waitingOrders;
    const auto& queueRef = queue_.getQueue();
    for (std::size_t i = 1; i < queueRef.size(); ++i) {
        const std::string& wId = queueRef[i];
        auto wIt = std::find_if(orders.begin(), orders.end(),
            [&wId](const Order& o) { return o.getId() == wId; });
        if (wIt != orders.end())
            waitingOrders.push_back(*wIt);
    }

    view_.showProductionStatus(*currentIt, waitingOrders, samples);
}

// ---------------------------------------------------------------------------
// onProcessNext — 다음 주문 생산 처리
// ---------------------------------------------------------------------------
void ProductionController::onProcessNext() {
    if (queue_.empty()) {
        view_.ShowMessage("대기 중인 주문이 없습니다.");
        return;
    }

    std::string orderId = queue_.front();
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    // 주문 찾기
    auto orderIt = std::find_if(orders.begin(), orders.end(),
        [&orderId](const Order& o) { return o.getId() == orderId; });
    if (orderIt == orders.end()) {
        view_.ShowMessage("주문 정보를 찾을 수 없습니다: " + orderId);
        return;
    }

    // 시료 찾기
    const std::string& sampleId = orderIt->getSampleId();
    auto sampleIt = std::find_if(samples.begin(), samples.end(),
        [&sampleId](const Sample& s) { return s.getId() == sampleId; });
    if (sampleIt == samples.end()) {
        view_.ShowMessage("시료 정보를 찾을 수 없습니다: " + sampleId);
        return;
    }

    int shortage  = std::max(0, orderIt->getQuantity() - sampleIt->getStock());
    int actualQty = (shortage > 0)
                    ? static_cast<int>(std::ceil(
                          static_cast<double>(shortage)
                          / (sampleIt->getYieldRate() * 0.9)))
                    : 0;
    double totalTime = sampleIt->getAvgProductionTime() * actualQty;

    view_.showProcessingResult(*orderIt, *sampleIt, shortage, actualQty, totalTime);

    sampleIt->addStock(actualQty);
    orderIt->setStatus(OrderStatus::CONFIRMED);
    orderIt->setProducedQty(actualQty);

    sampleRepo_.saveAll(samples);
    orderRepo_.saveAll(orders);
    queue_.pop();
}
