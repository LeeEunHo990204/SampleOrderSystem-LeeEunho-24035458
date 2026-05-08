#pragma once
#include "../core/IController.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../view/MonitorView.h"

class MonitorController : public IController {
public:
    MonitorController(SampleRepository& sampleRepo,
                      OrderRepository&  orderRepo,
                      MonitorView&      view);
    void Run() override;

    void showDashboard();

protected:
    void HandleInput(int command) override;

private:
    void onDashboard();
    void onStockOnly();

    SampleRepository& sampleRepo_;
    OrderRepository&  orderRepo_;
    MonitorView&      view_;
};
