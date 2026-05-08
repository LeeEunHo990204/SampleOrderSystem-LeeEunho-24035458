// OrderControllerTest.cpp — Phase 3 TDD tests for OrderController
#include <gtest/gtest.h>
#include "../controller/OrderController.h"
#include "../view/OrderView.h"
#include "../repository/SampleRepository.h"
#include "../repository/OrderRepository.h"
#include "../model/Sample.h"
#include "../model/Order.h"
#include "../model/OrderStatus.h"
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

    // OrderView virtual override — 테스트에서는 실제 출력 불필요
    void Render()                                           override {}
    void ShowMessage(const std::string&)                    override {}
    int  GetMenuChoice()                                    override { return 0; }
    void showConfirm(const Sample&, const std::string&, int) override {}
    void showOrderCreated(const Order&, const std::string&)  override {}

    std::string inputSampleId()    override { return sampleId_; }
    std::string inputCustomerName() override { return customerName_; }
    int         inputQuantity()    override { return quantity_; }
    char        inputConfirm()     override { return confirm_; }

private:
    std::string sampleId_;
    std::string customerName_;
    int         quantity_ = 0;
    char        confirm_  = 'Y';
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class OrderControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 시료 데이터 초기화
        std::ofstream sf(SAMPLE_TEST_FILE);
        sf << "[]";
        // 주문 데이터 초기화
        std::ofstream of(ORDER_TEST_FILE);
        of << "[]";
    }

    void TearDown() override {
        std::remove(SAMPLE_TEST_FILE.c_str());
        std::remove(ORDER_TEST_FILE.c_str());
    }

    SampleRepository sampleRepo_{ SAMPLE_TEST_FILE };
    OrderRepository  orderRepo_{ ORDER_TEST_FILE };
    StubOrderView    view_;
};

// ---------------------------------------------------------------------------
// 테스트 1: 주문 생성 시 RESERVED 상태로 저장된다
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 주문_생성_시_RESERVED_상태로_저장된다) {
    // 유효한 시료 사전 등록
    std::vector<Sample> samples = {
        Sample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(200);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, view_);
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
// 테스트 2: 존재하지 않는 시료 ID로 주문 실패
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 존재하지_않는_시료_ID로_주문_실패) {
    // 시료 데이터 없음 (SetUp에서 빈 배열로 초기화됨)
    view_.setInputSampleId("S-999");
    view_.setInputCustomerName("테스트 고객");
    view_.setInputQuantity(10);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// 테스트 3: 수량 0 이하 입력 시 주문 미저장
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 수량_0_이하_입력_시_주문_미저장) {
    std::vector<Sample> samples = {
        Sample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(0);
    view_.setInputConfirm('Y');

    OrderController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// 테스트 4: [N] 취소 입력 시 주문 미저장
// ---------------------------------------------------------------------------
TEST_F(OrderControllerTest, 취소_입력_시_주문_미저장) {
    std::vector<Sample> samples = {
        Sample("S-003", "SiC 파워기판-6인치", 4.0, 0.8, 100)
    };
    sampleRepo_.saveAll(samples);

    view_.setInputSampleId("S-003");
    view_.setInputCustomerName("삼성전자 파운드리");
    view_.setInputQuantity(200);
    view_.setInputConfirm('N');

    OrderController ctrl(sampleRepo_, orderRepo_, view_);
    ctrl.createOrder();

    auto orders = orderRepo_.loadAll();
    EXPECT_TRUE(orders.empty());
}

// ---------------------------------------------------------------------------
// TODO (Phase 4): 주문 승인 — 재고 충분 시 PRODUCING 으로 전환
// TODO (Phase 4): 주문 승인 — 재고 부족 시 거절 처리
// TODO (Phase 4): 주문 거절 — REJECTED 상태로 변경
// ---------------------------------------------------------------------------
