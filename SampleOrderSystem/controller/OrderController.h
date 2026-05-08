#pragma once
#include "../core/IController.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/ProductionQueue.h"
#include "../view/OrderView.h"

class OrderController : public IController {
public:
    OrderController(SampleRepository& sampleRepo,
                    OrderRepository&  orderRepo,
                    ProductionQueue&  prodQueue,
                    OrderView&        view);

    // Phase 4: [3] 주문 승인/거절 루프
    void Run() override;

    // Phase 3: [2] 시료 주문 루프
    void runOrderCreation();

    // 테스트에서 직접 호출 가능한 공개 메서드
    void createOrder();           // Phase 3: onCreateOrder() 래퍼
    void approveOrRejectOrder();  // Phase 4: onApproveOrReject() 래퍼

protected:
    void HandleInput(int command) override;

private:
    void onCreateOrder();        // Phase 3: 주문 접수 처리
    void onApproveOrReject();    // Phase 4: 승인/거절 통합 처리

    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
    ProductionQueue&  prodQueue_;
    OrderView&        view_;
};
