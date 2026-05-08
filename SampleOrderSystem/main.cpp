#include <windows.h>
#include "repository/SampleRepository.h"
#include "repository/OrderRepository.h"
#include "model/OrderStatus.h"
#include "view/ConsoleView.h"
#include "controller/SampleController.h"
#include "view/SampleView.h"

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

    bool running = true;
    while (running) {
        auto samples = sampleRepo.loadAll();
        auto orders  = orderRepo.loadAll();

        SystemStats stats;
        stats.sampleCount = (int)samples.size();
        for (const auto& s : samples) stats.totalStock += s.getStock();
        stats.orderCount = (int)orders.size();
        for (const auto& o : orders)
            if (o.getStatus() == OrderStatus::PRODUCING)
                stats.productionQueueCount++;

        view.Render(stats);
        int choice = view.GetMenuChoice();

        if (choice == 0) {
            view.ShowMessage("시스템을 종료합니다.");
            running = false;
        } else if (choice == 1) {
            SampleController ctrl(sampleRepo, sampleView);
            ctrl.Run();
        } else {
            // Phase 3~7: 각 Controller::Run() 으로 분기 예정
            view.ShowMessage("해당 기능은 아직 구현 중입니다.");
        }
    }

    return 0;
}
