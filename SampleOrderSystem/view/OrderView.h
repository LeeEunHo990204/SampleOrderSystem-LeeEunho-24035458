#pragma once
#include "ConsoleView.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include "../model/OrderStatus.h"
#include <string>
#include <vector>

class OrderView : public ConsoleView {
public:
    // Phase 4: [3] 주문 승인/거절 헤더
    void Render() override;

    // Phase 3: [2] 시료 주문 헤더
    virtual void renderCreateOrder();

    // Phase 3: 주문 입력 내용 확인 화면
    virtual void showConfirm(const Sample& sample,
                             const std::string& customerName,
                             int qty);

    // Phase 3: 예약 접수 완료 화면
    virtual void showOrderCreated(const Order& order,
                                  const std::string& sampleName);

    // Phase 4: 처리 후 인라인 메뉴 ([1] 계속  [0] 뒤로)
    virtual void showInlineMenu();

    // Phase 3: 주문 접수 후 인라인 메뉴 ([1] 주문 접수  [0] 뒤로)
    virtual void showCreateOrderMenu();

    // Phase 4: 승인 대기 목록 출력
    virtual void showReservedList(const std::vector<Order>& orders,
                                  const std::vector<Sample>& samples);

    // Phase 4: 재고 확인 중 메시지
    virtual void showStockChecking();

    // Phase 4: 시료·재고·부족분 정보 표시 및 [V]/[N] 선택
    virtual void showOrderInfo(const Sample& sample,
                               const Order& order,
                               int shortage,
                               int actualQty,
                               double totalTime);

    // Phase 4: 승인 결과 화면
    virtual void showApproveResult(const Order& order, OrderStatus newStatus);

    // Phase 4: 거절 결과 화면
    virtual void showRejectResult(const Order& order);

    // 입력 메서드 (virtual: 테스트 stub 오버라이드 가능)
    virtual std::string inputSampleId();
    virtual std::string inputCustomerName();
    virtual int         inputQuantity();
    virtual char        inputConfirm();

    // Phase 4: 목록 번호 입력
    virtual int inputListNo();

    // 주문 ID 입력
    std::string inputOrderId();
};
