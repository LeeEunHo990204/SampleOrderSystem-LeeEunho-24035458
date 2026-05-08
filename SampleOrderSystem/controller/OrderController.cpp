// OrderController.cpp — Phase 3: 시료 주문 컨트롤러
#include "OrderController.h"
#include <algorithm>
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
// Run — 서브메뉴 루프
// ---------------------------------------------------------------------------
void OrderController::Run() {
    bool running = true;
    while (running) {
        view_.Render();
        onCreateOrder();
        view_.showInlineMenu();
        int choice = view_.GetMenuChoice();
        if (choice == 0) running = false;
    }
}

// ---------------------------------------------------------------------------
// HandleInput
// ---------------------------------------------------------------------------
void OrderController::HandleInput(int command) {
    switch (command) {
    case 1: onCreateOrder(); break;
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

// ---------------------------------------------------------------------------
// 주문 접수 처리
// ---------------------------------------------------------------------------
void OrderController::onCreateOrder() {
    // 1. 시료 ID 입력
    std::string sampleId = view_.inputSampleId();

    // 2. 시료 존재 여부 확인
    auto samples = sampleRepo_.loadAll();
    auto it = std::find_if(samples.begin(), samples.end(),
        [&sampleId](const Sample& s) { return s.getId() == sampleId; });

    if (it == samples.end()) {
        view_.ShowMessage("존재하지 않는 시료 ID입니다: " + sampleId);
        return;
    }

    const Sample& sample = *it;

    // 3. 고객명 입력
    std::string customerName = view_.inputCustomerName();

    // 4. 수량 입력 및 검증 (1 이상)
    int qty = view_.inputQuantity();
    if (qty <= 0) {
        view_.ShowMessage("수량은 1 이상이어야 합니다.");
        return;
    }

    // 5. 입력 내용 확인 화면
    view_.showConfirm(sample, customerName, qty);

    // 6. 확인/취소
    char confirm = view_.inputConfirm();
    if (confirm != 'Y' && confirm != 'y') {
        view_.ShowMessage("주문 접수가 취소되었습니다.");
        return;
    }

    // 7. 주문 ID 생성
    auto orders  = orderRepo_.loadAll();
    std::string orderId = generateOrderId(static_cast<int>(orders.size()));

    // 8. Order 객체 생성 (status=RESERVED, producedQty=0)
    Order newOrder(orderId, sampleId, customerName, qty);

    // 9. 저장
    orders.push_back(newOrder);
    orderRepo_.saveAll(orders);

    // 10. 결과 화면 출력
    view_.showOrderCreated(newOrder, sample.getName());
}
