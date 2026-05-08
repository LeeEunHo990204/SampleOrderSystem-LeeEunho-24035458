#pragma once
#include "../view/ConsoleView.h"
#include "../model/Order.h"
#include "../model/Sample.h"
#include <vector>

class ReleaseView : public ConsoleView {
public:
    void Render()    override;

    virtual void showConfirmedList(const std::vector<Order>& orders,
                                   const std::vector<Sample>& samples);
    virtual void showReleaseResult(const Order& order);
    virtual void showInlineMenu();
    virtual int  inputListNo();
};
