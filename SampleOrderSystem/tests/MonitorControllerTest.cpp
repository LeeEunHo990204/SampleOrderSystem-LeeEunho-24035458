// MonitorControllerTest.cpp — Phase 7 TDD tests for MonitorController
#include <gtest/gtest.h>
#include "../controller/MonitorController.h"
#include "../view/MonitorView.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include "../model/OrderStatus.h"
#include "TestHelpers.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace {
    const std::string MON_SAMPLE_FILE = "test_monitor_ctrl_sample_tmp.json";
    const std::string MON_ORDER_FILE  = "test_monitor_ctrl_order_tmp.json";
}

class StubMonitorView : public MonitorView {
public:
    void Render()                        override {}
    void ShowMessage(const std::string&) override {}
    int  GetMenuChoice()                 override { return 0; }
    void showDashboard(const OrderSummary& s,
                       const std::vector<StockInfo>& list) override {
        capturedSummary_ = s;
        capturedList_    = list;
        called_          = true;
    }
    OrderSummary           capturedSummary_;
    std::vector<StockInfo> capturedList_;
    bool                   called_ = false;
};

class MonitorControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream sf(MON_SAMPLE_FILE);
        sf << "[]";
        std::ofstream of(MON_ORDER_FILE);
        of << "[]";
    }

    void TearDown() override {
        std::remove(MON_SAMPLE_FILE.c_str());
        std::remove(MON_ORDER_FILE.c_str());
    }

    SampleRepository sampleRepo_{ MON_SAMPLE_FILE };
    OrderRepository  orderRepo_{ MON_ORDER_FILE };
    StubMonitorView  view_;
};

// ---------------------------------------------------------------------------
// 테스트 1: 상태별 주문 집계 — REJECTED 제외
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 상태별_주문_집계_REJECTED_제외) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 500)
    };
    sampleRepo_.saveAll(samples);

    auto makeWithStatus = [](const std::string& id, const std::string& sid, OrderStatus st) {
        auto o = makeOrder(id, sid, "고객A", 10);
        o.setStatus(st);
        return o;
    };
    std::vector<Order> orders;
    orders.push_back(makeWithStatus("ORD-001", "S-001", OrderStatus::RESERVED));
    orders.push_back(makeWithStatus("ORD-002", "S-001", OrderStatus::RESERVED));
    orders.push_back(makeWithStatus("ORD-003", "S-001", OrderStatus::CONFIRMED));
    orders.push_back(makeWithStatus("ORD-004", "S-001", OrderStatus::PRODUCING));
    orders.push_back(makeWithStatus("ORD-005", "S-001", OrderStatus::RELEASE));
    orders.push_back(makeWithStatus("ORD-006", "S-001", OrderStatus::REJECTED));
    orderRepo_.saveAll(orders);

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    EXPECT_TRUE(view_.called_);
    EXPECT_EQ(view_.capturedSummary_.reserved,  2);
    EXPECT_EQ(view_.capturedSummary_.confirmed, 1);
    EXPECT_EQ(view_.capturedSummary_.producing, 1);
    EXPECT_EQ(view_.capturedSummary_.release,   1);
}

// ---------------------------------------------------------------------------
// 테스트 2: 재고 상태 — SUFFICIENT (재고 >= 필요 수량)
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 재고_상태_SUFFICIENT_재고가_필요수량_이상) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders;
    auto o = makeOrder("ORD-001", "S-001", "고객A", 100);
    o.setStatus(OrderStatus::RESERVED);
    orders.push_back(o);
    orderRepo_.saveAll(orders);

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    ASSERT_EQ(view_.capturedList_.size(), 1u);
    EXPECT_EQ(view_.capturedList_[0].state, StockState::SUFFICIENT);
}

// ---------------------------------------------------------------------------
// 테스트 3: 재고 상태 — SHORTAGE (0 < stock < 필요 수량)
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 재고_상태_SHORTAGE_재고가_필요수량_미만) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 30)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders;
    auto o = makeOrder("ORD-001", "S-001", "고객A", 100);
    o.setStatus(OrderStatus::RESERVED);
    orders.push_back(o);
    orderRepo_.saveAll(orders);

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    ASSERT_EQ(view_.capturedList_.size(), 1u);
    EXPECT_EQ(view_.capturedList_[0].state, StockState::SHORTAGE);
}

// ---------------------------------------------------------------------------
// 테스트 4: 재고 상태 — DEPLETED (stock == 0)
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 재고_상태_DEPLETED_재고가_0) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 0)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders;
    auto o = makeOrder("ORD-001", "S-001", "고객A", 100);
    o.setStatus(OrderStatus::RESERVED);
    orders.push_back(o);
    orderRepo_.saveAll(orders);

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    ASSERT_EQ(view_.capturedList_.size(), 1u);
    EXPECT_EQ(view_.capturedList_[0].state, StockState::DEPLETED);
}

// ---------------------------------------------------------------------------
// 테스트 5: 잔여율 계산
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 잔여율_계산_정확성) {
    std::vector<Sample> samples = {
        makeSample("S-001", "웨이퍼A",  3.0, 0.85,  80),
        makeSample("S-002", "웨이퍼B",  3.0, 0.85,   0),
        makeSample("S-003", "웨이퍼C",  3.0, 0.85, 100)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders;
    {
        auto o = makeOrder("ORD-001", "S-001", "고객A", 20);
        o.setStatus(OrderStatus::RESERVED);
        orders.push_back(o);
    }
    {
        auto o = makeOrder("ORD-002", "S-002", "고객B", 100);
        o.setStatus(OrderStatus::RESERVED);
        orders.push_back(o);
    }
    // S-003: 주문 없음 → 필요수량=0, stock>0 → yieldPct=100
    orderRepo_.saveAll(orders);

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    ASSERT_EQ(view_.capturedList_.size(), 3u);
    // S-001: stock=80, 필요=20 → 80*100/(80+20) = 80
    EXPECT_EQ(view_.capturedList_[0].yieldPct, 80);
    // S-002: stock=0, 필요=100 → 0
    EXPECT_EQ(view_.capturedList_[1].yieldPct, 0);
    // S-003: stock=100, 필요=0 → 100
    EXPECT_EQ(view_.capturedList_[2].yieldPct, 100);
}

// ---------------------------------------------------------------------------
// 테스트 6: 주문 없을 때도 showDashboard 호출됨 (빈 stockList 허용)
// ---------------------------------------------------------------------------
TEST_F(MonitorControllerTest, 주문_없을때도_showDashboard_호출됨) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 500)
    };
    sampleRepo_.saveAll(samples);
    // 주문 없음

    MonitorController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.showDashboard();

    EXPECT_TRUE(view_.called_);
    ASSERT_EQ(view_.capturedList_.size(), 1u);
}
