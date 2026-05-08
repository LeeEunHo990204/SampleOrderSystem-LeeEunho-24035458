#pragma once
#include "../core/IController.h"
#include "../repository/OrderRepository.h"
#include "../repository/SampleRepository.h"
#include "../view/ReleaseView.h"

class ReleaseController : public IController {
public:
    ReleaseController(OrderRepository& orderRepo,
                      SampleRepository& sampleRepo,
                      ReleaseView& view);

    void Run() override;

    void releaseOrder();

protected:
    void HandleInput(int command) override;

private:
    void onRelease();

    OrderRepository&  orderRepo_;
    SampleRepository& sampleRepo_;
    ReleaseView&      view_;
};
