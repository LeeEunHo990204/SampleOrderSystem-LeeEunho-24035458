// ProductionControllerTest.cpp — Phase 5 TDD tests
#include <gtest/gtest.h>
#include "../model/ProductionQueue.h"
#include "../controller/ProductionController.h"
#include "../view/ProductionView.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include "../model/OrderStatus.h"
#include "TestHelpers.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

namespace {
    const std::string PROD_SAMPLE_FILE = "test_prod_ctrl_sample_tmp.json";
    const std::string PROD_ORDER_FILE  = "test_prod_ctrl_order_tmp.json";
}

// ---------------------------------------------------------------------------
// StubProductionView: ProductionView 의 Stub
// ---------------------------------------------------------------------------
class StubProductionView : public ProductionView {
public:
    void Render()                                              override {}
    void ShowMessage(const std::string&)                      override {}
    int  GetMenuChoice()                                      override { return 0; }
    void showProductionStatus(const Order&,
                               const std::vector<Order>&,
                               const std::vector<Sample>&)   override { statusShown_ = true; }
    void showEmptyQueue()                                     override { emptyShown_ = true; }
    void showProcessingResult(const Order&, const Sample&,
                               int, int, double)              override {}
    void showInlineMenu()                                     override {}

    bool statusShown_ = false;
    bool emptyShown_  = false;
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ProductionControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream sf(PROD_SAMPLE_FILE);
        sf << "[]";
        std::ofstream of(PROD_ORDER_FILE);
        of << "[]";
    }

    void TearDown() override {
        std::remove(PROD_SAMPLE_FILE.c_str());
        std::remove(PROD_ORDER_FILE.c_str());
    }

    SampleRepository    sampleRepo_{ PROD_SAMPLE_FILE };
    OrderRepository     orderRepo_{ PROD_ORDER_FILE };
    ProductionQueue     queue_;
    StubProductionView  view_;
};

// ---------------------------------------------------------------------------
// 테스트 1: ProductionQueue 기본 동작 — push/front/pop/size/empty
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, ProductionQueue_기본_동작_push_front_pop_size_empty) {
    ProductionQueue q;
    EXPECT_TRUE(q.empty());
    EXPECT_EQ(q.size(), 0);

    q.push("ORD-001");
    q.push("ORD-002");

    EXPECT_FALSE(q.empty());
    EXPECT_EQ(q.size(), 2);
    EXPECT_EQ(q.front(), "ORD-001");

    q.pop();
    EXPECT_EQ(q.size(), 1);
    EXPECT_EQ(q.front(), "ORD-002");

    q.pop();
    EXPECT_TRUE(q.empty());
}

// ---------------------------------------------------------------------------
// 테스트 2: 빈 큐에서 front 호출 시 예외
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 빈_큐에서_front_호출_시_예외) {
    ProductionQueue q;
    EXPECT_THROW(q.front(), std::runtime_error);
}

// ---------------------------------------------------------------------------
// 테스트 3: 생산 처리 — PRODUCING → CONFIRMED 전환, 재고 추가, producedQty 설정, 큐 pop
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 생산_처리_PRODUCING_CONFIRMED_전환_재고추가_큐pop) {
    // 재고 30, 주문량 200 → 부족분 170
    // actualQty = ceil(170 / (0.8 * 0.9)) = ceil(170 / 0.72) = ceil(236.11) = 237
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 30)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-003", "삼성전자 파운드리", 200)
    };
    orders[0].setStatus(OrderStatus::PRODUCING);
    orderRepo_.saveAll(orders);

    queue_.push("ORD-20260508-0001");

    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.processNext();

    // 큐에서 제거됐는지 확인
    EXPECT_TRUE(queue_.empty());

    // 주문 상태 CONFIRMED, producedQty 설정 확인
    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::CONFIRMED);
    EXPECT_EQ(savedOrders[0].getProducedQty(), 237);

    // 재고 추가 확인: 30 + 237 = 267
    auto savedSamples = sampleRepo_.loadAll();
    ASSERT_EQ(savedSamples.size(), 1u);
    EXPECT_EQ(savedSamples[0].getStock(), 267);
}

// ---------------------------------------------------------------------------
// 테스트 4: 빈 큐에서 생산 처리 시 아무 변화 없음
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 빈_큐에서_생산_처리_시_아무_변화_없음) {
    std::vector<Sample> samples = {
        makeSample("S-001", "산화막 웨이퍼-SiO2", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-001", "LG이노텍", 100)
    };
    orderRepo_.saveAll(orders);

    // 큐는 비어 있음
    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.processNext();

    // 상태 변화 없음
    auto savedOrders = orderRepo_.loadAll();
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::RESERVED);

    // 재고 변화 없음
    auto savedSamples = sampleRepo_.loadAll();
    EXPECT_EQ(savedSamples[0].getStock(), 200);
}

// ---------------------------------------------------------------------------
// 테스트 5: 생산 현황 표시 — PRODUCING 있으면 showProductionStatus 호출
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 생산_현황_표시_PRODUCING_있으면_showProductionStatus_호출) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 30)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-003", "삼성전자 파운드리", 200)
    };
    orders[0].setStatus(OrderStatus::PRODUCING);
    orderRepo_.saveAll(orders);

    queue_.push("ORD-20260508-0001");

    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.showStatus();

    EXPECT_TRUE(view_.statusShown_);
    EXPECT_FALSE(view_.emptyShown_);
}

// ---------------------------------------------------------------------------
// 테스트 6: 생산 현황 표시 — 빈 큐면 showEmptyQueue 호출
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 생산_현황_표시_빈_큐면_showEmptyQueue_호출) {
    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.showStatus();

    EXPECT_FALSE(view_.statusShown_);
    EXPECT_TRUE(view_.emptyShown_);
}

// ---------------------------------------------------------------------------
// 테스트 7: 재고 충분(shortage=0) 시 생산 처리 — actualQty=0, 재고 불변, CONFIRMED 전환
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 재고_충분_시_생산_처리_actualQty_0_재고_불변_CONFIRMED) {
    // stock(200) >= quantity(100) → shortage=0, actualQty=0
    std::vector<Sample> samples = {
        makeSample("S-001", "산화막 웨이퍼-SiO2", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-001", "LG이노텍", 100)
    };
    orders[0].setStatus(OrderStatus::PRODUCING);
    orderRepo_.saveAll(orders);

    queue_.push("ORD-20260508-0001");

    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.processNext();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(),      OrderStatus::CONFIRMED);
    EXPECT_EQ(savedOrders[0].getProducedQty(), 0);

    // 재고 불변 (shortage=0이므로 addStock(0))
    auto savedSamples = sampleRepo_.loadAll();
    ASSERT_EQ(savedSamples.size(), 1u);
    EXPECT_EQ(savedSamples[0].getStock(), 200);

    EXPECT_TRUE(queue_.empty());
}

// ---------------------------------------------------------------------------
// 테스트 8: 자동 완료 — productionStartedAt 경과 시 showStatus() 호출로 CONFIRMED 전환
// ---------------------------------------------------------------------------
TEST_F(ProductionControllerTest, 자동완료_경과시간_초과시_CONFIRMED_전환) {
    // avgProductionTime=1.0, yieldRate=0.9, stock=0, qty=1
    // shortage=1, actualQty=ceil(1/(0.9*0.9))=ceil(1.235)=2, totalMinutes=2
    // startTime = now - 3*60 → elapsed=3 >= 2 → auto-complete
    std::vector<Sample> samples = {
        Sample("S-AUTO", "자동완료 테스트", 1.0, 0.9, 0)
    };
    sampleRepo_.saveAll(samples);

    long long pastTime = static_cast<long long>(std::time(nullptr)) - 3 * 60;
    std::vector<Order> orders = {
        Order("ORD-20260508-AUTO", "S-AUTO", "테스트 고객", 1)
    };
    orders[0].setStatus(OrderStatus::PRODUCING);
    orders[0].setProductionStartedAt(pastTime);
    orderRepo_.saveAll(orders);

    queue_.push("ORD-20260508-AUTO");

    ProductionController ctrl(sampleRepo_, orderRepo_, queue_, view_);
    ctrl.showStatus();

    // 자동 완료: 큐 비어 있음
    EXPECT_TRUE(queue_.empty());

    // 주문 상태 CONFIRMED, producedQty 설정 확인
    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(),      OrderStatus::CONFIRMED);
    EXPECT_EQ(savedOrders[0].getProducedQty(), 2);

    // 재고 추가 확인: 0 + 2 = 2
    auto savedSamples = sampleRepo_.loadAll();
    ASSERT_EQ(savedSamples.size(), 1u);
    EXPECT_EQ(savedSamples[0].getStock(), 2);
}
