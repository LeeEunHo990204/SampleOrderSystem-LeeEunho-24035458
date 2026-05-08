#pragma once
#include "ConsoleView.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include <string>

class OrderView : public ConsoleView {
public:
    // 서브메뉴 렌더링: [1] 주문 접수  [0] 뒤로
    void Render() override;

    // 주문 입력 내용 확인 화면
    virtual void showConfirm(const Sample& sample,
                             const std::string& customerName,
                             int qty);

    // 예약 접수 완료 화면
    virtual void showOrderCreated(const Order& order,
                                  const std::string& sampleName);

    // 입력 메서드 (virtual: 테스트 stub 오버라이드 가능)
    virtual std::string inputSampleId();
    virtual std::string inputCustomerName();
    virtual int         inputQuantity();
    virtual char        inputConfirm();

    // 결과 화면 하단 인라인 메뉴 (화면 클리어 없음)
    void showInlineMenu();

    // Phase 4용: 승인/거절 시 주문 ID 조회
    std::string inputOrderId();
};
