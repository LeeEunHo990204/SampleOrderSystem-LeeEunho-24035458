#pragma once
#include "../core/IController.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/ProductionQueue.h"
#include "../view/ProductionView.h"

class ProductionController : public IController {
public:
    ProductionController(SampleRepository& sampleRepo,
                         OrderRepository&  orderRepo,
                         ProductionQueue&  queue,
                         ProductionView&   view);

    void Run() override;

    // 테스트용 public 래퍼
    void processNext();
    void showStatus();

protected:
    void HandleInput(int command) override;

private:
    void onShowStatus();
    void onProcessNext();
    void autoCompleteFinished();

    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
    ProductionQueue&  queue_;
    ProductionView&   view_;
};
