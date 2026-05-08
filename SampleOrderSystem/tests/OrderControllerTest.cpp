// OrderControllerTest.cpp — Phase 3 + Phase 4 TDD tests for OrderController
#include <gtest/gtest.h>
#include "../controller/OrderController.h"
#include "../view/OrderView.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include "../model/OrderStatus.h"
#include "../model/ProductionQueue.h"
#include "TestHelpers.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

namespace {
    const std::string SAMPLE_TEST_FILE = "test_order_ctrl_sample_tmp.json";
    const std::string ORDER_TEST_FILE  = "test_order_ctrl_order_tmp.json";
}

// ---------------------------------------------------------------------------
// StubOrderView: OrderView의 Stub — 입력값을 미리 세팅, 출력은 무시
// ---------------------------------------------------------------------------
class StubOrderView : public OrderView {
public:
    void setInputSampleId(const std::string& v)     { sampleId_ = v; }
    void setInputCustomerName(const std::string& v) { customerName_ = v; }
    void setInputQuantity(int v)                    { quantity_ = v; }
    void setInputConfirm(char v)                    { confirm_ = v; }
    void setInputListNo(int v)                      { listNo_ = v; }

    // OrderView virtual override — 테스트에서는 실제 출력 불필요
    void Render()                                                                  override {}
    void renderCreateOrder()                                                       override {}
    void ShowMessage(const std::string&)                                           override {}
    int  GetMenuChoice()                                                           override { return 0; }
    void showConfirm(const Sample&, const std::string&, int)                       override {}
    void showOrderCreated(const Order&, const std::string&)                        override {}
    void showInlineMenu()                                                          override {}
    void showCreateOrderMenu()                                                     override {}
    void showReservedList(const std::vector<Order>&, const std::vector<Sample>&)   override {}
    void showStockChecking()                                                       override {}
    void showOrderInfo(const Sample&, const Order&, int, int, double)              override {}
    void showApproveResult(const Order&, OrderStatus)                              override {}
    void showRejectResult(const Order&)                                            override {}

    std::string inputSampleId()     override { return sampleId_; }
    std::string inputCustomerName() override { return customerName_; }
    int         inputQuantity()     override { return quantity_; }
    char        inputConfirm()      override { return confirm_; }
    int         inputListNo()       override { return listNo_; }

private:
    std::string sampleId_;
    std::string customerName_;
    int         quantity_ = 0;
    char        confirm_  = 'Y';
    int         listNo_   = 1;
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class OrderControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream sf(SAMPLE_TEST_FILE);
        sf << "[]";
        std::ofstream of(ORDER_TEST_FILE);
        of << "[]";
    }

    void TearDown() override {
        std::remove(SAMPLE_TEST_FILE.c_str());
        std::remove(ORDER_TEST_FILE.c_str());
    }

    SampleRepository sampleRepo_{ SAMPLE_TEST_FILE };
    OrderRepository  orderRepo_{ ORDER_TEST_FILE };
    ProductionQueue  prodQueue_;
    StubOrderView    view_;
};

// ---------------------------------------------------------------------------
// Phase 3 테스트 1: 주문 생성 시 RESERVED 상태로 저장된다
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 주문_생성_시_RESERVED_상태로_저장된다) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(200);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    ASSERT_EQ(orders.size(), 1u);
    EXPECT_EQ(orders[0].getSampleId(),     "S-003");
    EXPECT_EQ(orders[0].getCustomerName(), "삼성전자 파운드리");
    EXPECT_EQ(orders[0].getQuantity(),     200);
    EXPECT_EQ(orders[0].getStatus(),       OrderStatus::RESERVED);
    EXPECT_EQ(orders[0].getProducedQty(),  0);
}

// ---------------------------------------------------------------------------
// Phase 3 테스트 2: 존재하지 않는 시료 ID로 주문 실패
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 존재하지_않는_시료_ID로_주문_실패) {
    view_.setInputSampleId("S-999");
    view_.setInputCustomerName("테스트 고객");
    view_.setInputQuantity(10);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// Phase 3 테스트 3: 수량 0 이하 입력 시 주문 미저장
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 수량_0_이하_입력_시_주문_미저장) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(0);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// Phase 3 테스트 4: [N] 취소 입력 시 주문 미저장
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 취소_입력_시_주문_미저장) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(200);
    view_.setInputConfirm('N');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// Phase 4 테스트 1: 재고 충분 시 V 승인 → CONFIRMED 전환 및 재고 차감
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 재고_충분_시_V_승인_CONFIRMED_전환_및_재고_차감) {
    std::vector<Sample> samples = {
        makeSample("S-001", "산화막 웨이퍼-SiO2", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-001", "LG이노텍", 100)
    };
    orderRepo_.saveAll(orders);

    view_.setInputListNo(1);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.approveOrRejectOrder();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::CONFIRMED);

    auto savedSamples = sampleRepo_.loadAll();
    ASSERT_EQ(savedSamples.size(), 1u);
    EXPECT_EQ(savedSamples[0].getStock(), 100);  // 200 - 100
}

// ---------------------------------------------------------------------------
// Phase 4 테스트 2: 재고 부족 + V 승인 시 PRODUCING 전환
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 재고_부족_V_입력_시_PRODUCING_전환) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 30)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-003", "삼성전자 파운드리", 200)
    };
    orderRepo_.saveAll(orders);

    view_.setInputListNo(1);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.approveOrRejectOrder();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::PRODUCING);

    // ProductionQueue에 주문 ID가 push됐는지 확인
    EXPECT_EQ(prodQueue_.size(), 1);
    EXPECT_EQ(prodQueue_.front(), "ORD-20260508-0001");
}

// ---------------------------------------------------------------------------
// Phase 4 테스트 3: 재고 부족 + N 입력 시 REJECTED 전환
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 재고_부족_N_입력_시_REJECTED_전환) {
    std::vector<Sample> samples = {
        makeSample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 30)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-003", "삼성전자 파운드리", 200)
    };
    orderRepo_.saveAll(orders);

    view_.setInputListNo(1);
    view_.setInputConfirm('N');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.approveOrRejectOrder();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::REJECTED);
}

// ---------------------------------------------------------------------------
// Phase 4 테스트 4: N 입력으로 재고 충분한 주문도 거절 가능
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, N_입력_시_재고_충분해도_REJECTED_전환) {
    std::vector<Sample> samples = {
        makeSample("S-001", "산화막 웨이퍼-SiO2", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-001", "LG이노텍", 100)
    };
    orderRepo_.saveAll(orders);

    view_.setInputListNo(1);
    view_.setInputConfirm('N');

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.approveOrRejectOrder();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::REJECTED);
}

// ---------------------------------------------------------------------------
// Phase 4 테스트 5: RESERVED 없을 때 승인 시도 → 상태 변화 없음
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, RESERVED_없을때_승인_시도_주문_저장_안됨) {
    std::vector<Sample> samples = {
        makeSample("S-001", "산화막 웨이퍼-SiO2", 3.0, 0.85, 200)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        makeOrder("ORD-20260508-0001", "S-001", "LG이노텍", 100)
    };
    orders[0].setStatus(OrderStatus::CONFIRMED);
    orderRepo_.saveAll(orders);

    view_.setInputListNo(1);

    OrderController ctrl(sampleRepo_, orderRepo_, prodQueue_, view_);
    ctrl.approveOrRejectOrder();

    auto savedOrders = orderRepo_.loadAll();
    ASSERT_EQ(savedOrders.size(), 1u);
    EXPECT_EQ(savedOrders[0].getStatus(), OrderStatus::CONFIRMED);
}
