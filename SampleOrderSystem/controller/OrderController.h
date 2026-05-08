#pragma once
#include "../core/IController.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../view/OrderView.h"

class OrderController : public IController {
public:
    OrderController(SampleRepository& sampleRepo,
                    OrderRepository&  orderRepo,
                    OrderView&        view);

    void Run() override;

    // 테스트에서 직접 호출 가능한 공개 메서드
    void createOrder();

protected:
    void HandleInput(int command) override;

private:
    void onCreateOrder();

    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
    OrderView&        view_;
};
