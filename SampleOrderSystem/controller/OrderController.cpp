// OrderController.cpp — Phase 3 + Phase 4
#include "OrderController.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <sstream>
#include <iomanip>

// ---------------------------------------------------------------------------
// 주문 ID 생성: ORD-YYYYMMDD-NNNN
// ---------------------------------------------------------------------------
static std::string generateOrderId(int existingCount) {
    std::time_t now = std::time(nullptr);
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    std::ostringstream oss;
    oss << "ORD-"
        << std::put_time(&t, "%Y%m%d")
        << "-"
        << std::setw(4) << std::setfill('0') << (existingCount + 1);
    return oss.str();
}

// ---------------------------------------------------------------------------
// 생성자
// ---------------------------------------------------------------------------
OrderController::OrderController(SampleRepository& sampleRepo,
                                 OrderRepository&  orderRepo,
                                 OrderView&        view)
    : sampleRepo_(sampleRepo), orderRepo_(orderRepo), view_(view)
{
}

// ---------------------------------------------------------------------------
// Run — Phase 4: 목록 표시 → 번호 선택 → 승인/거절 루프
// ---------------------------------------------------------------------------
void OrderController::Run() {
    bool running = true;
    while (running) {
        view_.Render();
        onApproveOrReject();
        view_.showInlineMenu();
        int choice = view_.GetMenuChoice();
        if (choice == 0) running = false;
    }
}

// ---------------------------------------------------------------------------
// runOrderCreation — Phase 3: 시료 주문 루프
// ---------------------------------------------------------------------------
void OrderController::runOrderCreation() {
    bool running = true;
    while (running) {
        view_.renderCreateOrder();
        onCreateOrder();
        view_.showCreateOrderMenu();
        int choice = view_.GetMenuChoice();
        if (choice == 0) running = false;
    }
}

// ---------------------------------------------------------------------------
// HandleInput
// ---------------------------------------------------------------------------
void OrderController::HandleInput(int command) {
    switch (command) {
    case 1: onApproveOrReject(); break;
    default:
        view_.ShowMessage("잘못된 입력입니다.");
        break;
    }
}

// ---------------------------------------------------------------------------
// 공개 메서드 — 테스트에서 직접 호출 가능
// ---------------------------------------------------------------------------
void OrderController::createOrder() {
    onCreateOrder();
}

void OrderController::approveOrRejectOrder() {
    onApproveOrReject();
}

// ---------------------------------------------------------------------------
// Phase 3: 주문 접수 처리
// ---------------------------------------------------------------------------
void OrderController::onCreateOrder() {
    std::string sampleId = view_.inputSampleId();

    auto samples = sampleRepo_.loadAll();
    auto it = std::find_if(samples.begin(), samples.end(),
        [&sampleId](const Sample& s) { return s.getId() == sampleId; });

    if (it == samples.end()) {
        view_.ShowMessage("존재하지 않는 시료 ID입니다: " + sampleId);
        return;
    }

    const Sample& sample = *it;

    std::string customerName = view_.inputCustomerName();

    int qty = view_.inputQuantity();
    if (qty <= 0) {
        view_.ShowMessage("수량은 1 이상이어야 합니다.");
        return;
    }

    view_.showConfirm(sample, customerName, qty);

    char confirm = view_.inputConfirm();
    if (confirm != 'Y' && confirm != 'y') {
        view_.ShowMessage("주문 접수가 취소되었습니다.");
        return;
    }

    auto orders = orderRepo_.loadAll();
    std::string orderId = generateOrderId(static_cast<int>(orders.size()));

    Order newOrder(orderId, sampleId, customerName, qty);

    orders.push_back(newOrder);
    orderRepo_.saveAll(orders);

    view_.showOrderCreated(newOrder, sample.getName());
}

// ---------------------------------------------------------------------------
// Phase 4: 승인/거절 통합 처리
// ---------------------------------------------------------------------------
void OrderController::onApproveOrReject() {
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    // RESERVED 필터링
    std::vector<Order> reserved;
    for (const auto& o : orders)
        if (o.getStatus() == OrderStatus::RESERVED)
            reserved.push_back(o);

    if (reserved.empty()) {
        view_.ShowMessage("승인 대기 중인 주문이 없습니다.");
        return;
    }

    view_.showReservedList(reserved, samples);

    int listNo = view_.inputListNo();
    if (listNo == 0) return;
    if (listNo < 1 || listNo > static_cast<int>(reserved.size())) {
        view_.ShowMessage("유효하지 않은 번호입니다.");
        return;
    }

    Order selectedOrder = reserved[listNo - 1];

    view_.showStockChecking();

    auto sampleIt = std::find_if(samples.begin(), samples.end(),
        [&selectedOrder](const Sample& s) {
            return s.getId() == selectedOrder.getSampleId();
        });

    if (sampleIt == samples.end()) {
        view_.ShowMessage("시료 정보를 찾을 수 없습니다.");
        return;
    }

    Sample& sample   = *sampleIt;
    int stock        = sample.getStock();
    int quantity     = selectedOrder.getQuantity();
    int shortage     = (quantity > stock) ? (quantity - stock) : 0;
    int actualQty    = (shortage > 0)
                       ? static_cast<int>(std::ceil(
                             static_cast<double>(shortage)
                             / (sample.getYieldRate() * 0.9)))
                       : 0;
    double totalTime = sample.getAvgProductionTime() * actualQty;

    view_.showOrderInfo(sample, selectedOrder, shortage, actualQty, totalTime);

    char confirm = view_.inputConfirm();

    // orders 벡터에서 실제 수정 대상 참조
    auto orderIt = std::find_if(orders.begin(), orders.end(),
        [&selectedOrder](const Order& o) {
            return o.getId() == selectedOrder.getId();
        });

    if (confirm == 'Y' || confirm == 'y') {
        if (stock >= quantity) {
            sample.reduceStock(quantity);
            sampleRepo_.saveAll(samples);
            orderIt->setStatus(OrderStatus::CONFIRMED);
        } else {
            orderIt->setStatus(OrderStatus::PRODUCING);
        }
        orderRepo_.saveAll(orders);
        view_.showApproveResult(*orderIt, orderIt->getStatus());
    } else {
        orderIt->setStatus(OrderStatus::REJECTED);
        orderRepo_.saveAll(orders);
        view_.showRejectResult(*orderIt);
    }
}
