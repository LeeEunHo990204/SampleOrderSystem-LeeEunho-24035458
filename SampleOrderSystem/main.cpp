#include <windows.h>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "model/OrderStatus.h"
#include "model/ProductionQueue.h"
#include "view/ConsoleView.h"
#include "controller/SampleController.h"
#include "view/SampleView.h"
#include "controller/OrderController.h"
#include "view/OrderView.h"
#include "controller/ProductionController.h"
#include "view/ProductionView.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    SampleRepository sampleRepo("data/samples.json");
    OrderRepository  orderRepo("data/orders.json");
    ConsoleView      view;
    SampleView       sampleView;
    OrderView        orderView;

    // ProductionQueue 생성 및 복원 (PRODUCING 상태 주문을 FIFO 순으로 push)
    ProductionQueue prodQueue;
    for (const auto& o : orderRepo.loadAll())
        if (o.getStatus() == OrderStatus::PRODUCING)
            prodQueue.push(o.getId());

    bool running = true;
    while (running) {
        auto samples = sampleRepo.loadAll();
        auto orders  = orderRepo.loadAll();

        SystemStats stats;
        stats.sampleCount = (int)samples.size();
        for (const auto& s : samples) stats.totalStock += s.getStock();
        stats.orderCount = (int)orders.size();
        stats.productionQueueCount = prodQueue.size();

        view.Render(stats);
        int choice = view.GetMenuChoice();

        if (choice == 0) {
            view.ShowMessage("시스템을 종료합니다.");
            running = false;
        } else if (choice == 1) {
            SampleController ctrl(sampleRepo, sampleView);
            ctrl.Run();
        } else if (choice == 2) {
            OrderController ctrl(sampleRepo, orderRepo, prodQueue, orderView);
            ctrl.runOrderCreation();
        } else if (choice == 3) {
            OrderController ctrl(sampleRepo, orderRepo, prodQueue, orderView);
            ctrl.Run();
        } else if (choice == 5) {
            ProductionView prodView;
            ProductionController ctrl(sampleRepo, orderRepo, prodQueue, prodView);
            ctrl.Run();
        } else {
            // Phase 6~7: 각 Controller::Run() 으로 분기 예정
            view.ShowMessage("해당 기능은 아직 구현 중입니다.");
        }
    }

    return 0;
}
