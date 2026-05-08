#include "ReleaseController.h"
#include <algorithm>
#include <ctime>
#include <sstream>
#include <iomanip>

static std::string currentDateTimeString() {
    std::time_t now = std::time(nullptr);
    std::tm t{};
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    std::ostringstream oss;
    oss << std::put_time(&t, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

ReleaseController::ReleaseController(OrderRepository& orderRepo,
                                     SampleRepository& sampleRepo,
                                     ReleaseView& view)
    : orderRepo_(orderRepo), sampleRepo_(sampleRepo), view_(view)
{
}

void ReleaseController::Run() {
    bool running = true;
    while (running) {
        view_.Render();
        onRelease();
        view_.showInlineMenu();
        int choice = view_.GetMenuChoice();
        if (choice == 0) running = false;
    }
}

void ReleaseController::releaseOrder() {
    onRelease();
}

void ReleaseController::HandleInput(int command) {
    switch (command) {
    case 1: onRelease(); break;
    default:
        view_.ShowMessage("잘못된 입력입니다.");
        break;
    }
}

void ReleaseController::onRelease() {
    auto orders  = orderRepo_.loadAll();
    auto samples = sampleRepo_.loadAll();

    std::vector<Order> confirmed;
    for (const auto& o : orders)
        if (o.getStatus() == OrderStatus::CONFIRMED)
            confirmed.push_back(o);

    if (confirmed.empty()) {
        view_.ShowMessage("출고 대기 중인 주문이 없습니다.");
        return;
    }

    view_.showConfirmedList(confirmed, samples);

    int listNo = view_.inputListNo();
    if (listNo == 0) return;
    if (listNo < 1 || listNo > static_cast<int>(confirmed.size())) {
        view_.ShowMessage("유효하지 않은 번호입니다.");
        return;
    }

    const Order& selected = confirmed[listNo - 1];
    std::string dateTime  = currentDateTimeString();

    auto orderIt = std::find_if(orders.begin(), orders.end(),
        [&selected](const Order& o) { return o.getId() == selected.getId(); });

    orderIt->setStatus(OrderStatus::RELEASE);
    orderIt->setReleasedAt(dateTime);

    orderRepo_.saveAll(orders);
    view_.showReleaseResult(*orderIt);
}
