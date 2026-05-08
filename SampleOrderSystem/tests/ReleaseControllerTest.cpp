// ReleaseControllerTest.cpp — Phase 6 TDD tests for ReleaseController
#include <gtest/gtest.h>
#include "../controller/ReleaseController.h"
#include "../view/ReleaseView.h"
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
    const std::string REL_SAMPLE_FILE = "test_release_ctrl_sample_tmp.json";
    const std::string REL_ORDER_FILE  = "test_release_ctrl_order_tmp.json";
}

// ---------------------------------------------------------------------------
// StubReleaseView: ReleaseView 의 Stub
// ---------------------------------------------------------------------------
class StubReleaseView : public ReleaseView {
public:
    void Render()                                                               override {}
    void ShowMessage(const std::string& msg)                                    override { lastMessage_ = msg; }
    int  GetMenuChoice()                                                        override { return 0; }
    void showConfirmedList(const std::vector<Order>&,
                           const std::vector<Sample>&)                          override { listShown_ = true; }
    void showReleaseResult(const Order&)                                        override { resultShown_ = true; }
    void showInlineMenu()                                                       override {}
    int  inputListNo()                                                          override { return listNo_; }

    int         listNo_      = 1;
    bool        listShown_   = false;
    bool        resultShown_ = false;
    std::string lastMessage_;
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ReleaseControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ofstream sf(REL_SAMPLE_FILE);
        sf << "[]";
        std::ofstream of(REL_ORDER_FILE);
        of << "[]";
    }

    void TearDown() override {
        std::remove(REL_SAMPLE_FILE.c_str());
        std::remove(REL_ORDER_FILE.c_str());
    }

    SampleRepository  sampleRepo_{ REL_SAMPLE_FILE };
    OrderRepository   orderRepo_{ REL_ORDER_FILE };
    StubReleaseView   view_;
};

// ---------------------------------------------------------------------------
// 테스트 1: CONFIRMED 주문 출고 시 RELEASE 전환 + releasedAt 설정 + resultShown
// ---------------------------------------------------------------------------
TEST_F(ReleaseControllerTest, CONFIRMED_주문_출고_시_RELEASE_전환_및_releasedAt_설정) {
    std::vector<Sample> samples = {
        Sample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 500)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        Order("ORD-20260508-0001", "S-001", "SK하이닉스", 150)
    };
    orders[0].setStatus(OrderStatus::CONFIRMED);
    orderRepo_.saveAll(orders);

    view_.listNo_ = 1;
    ReleaseController ctrl(orderRepo_, sampleRepo_, view_);
    ctrl.releaseOrder();

    auto saved = orderRepo_.loadAll();
    ASSERT_EQ(saved.size(), 1u);
    EXPECT_EQ(saved[0].getStatus(), OrderStatus::RELEASE);
    EXPECT_FALSE(saved[0].getReleasedAt().empty());
    EXPECT_TRUE(view_.resultShown_);
}

// ---------------------------------------------------------------------------
// 테스트 2: CONFIRMED 없을 때 메시지 출력 후 상태 변화 없음
// ---------------------------------------------------------------------------
TEST_F(ReleaseControllerTest, CONFIRMED_없을_때_메시지_출력_후_상태_불변) {
    std::vector<Order> orders = {
        Order("ORD-20260508-0001", "S-001", "SK하이닉스", 150)
    };
    // RESERVED 상태 유지
    orderRepo_.saveAll(orders);

    view_.listNo_ = 1;
    ReleaseController ctrl(orderRepo_, sampleRepo_, view_);
    ctrl.releaseOrder();

    EXPECT_EQ(view_.lastMessage_, "출고 대기 중인 주문이 없습니다.");
    auto saved = orderRepo_.loadAll();
    ASSERT_EQ(saved.size(), 1u);
    EXPECT_EQ(saved[0].getStatus(), OrderStatus::RESERVED);
}

// ---------------------------------------------------------------------------
// 테스트 3: 범위 밖 번호 입력 시 상태 변화 없음 + 에러 메시지
// ---------------------------------------------------------------------------
TEST_F(ReleaseControllerTest, 범위_밖_번호_입력_시_상태_불변_에러메시지) {
    std::vector<Order> orders = {
        Order("ORD-20260508-0001", "S-001", "SK하이닉스", 150)
    };
    orders[0].setStatus(OrderStatus::CONFIRMED);
    orderRepo_.saveAll(orders);

    view_.listNo_ = 5;
    ReleaseController ctrl(orderRepo_, sampleRepo_, view_);
    ctrl.releaseOrder();

    auto saved = orderRepo_.loadAll();
    ASSERT_EQ(saved.size(), 1u);
    EXPECT_EQ(saved[0].getStatus(), OrderStatus::CONFIRMED);
    EXPECT_FALSE(view_.lastMessage_.empty());
}

// ---------------------------------------------------------------------------
// 테스트 4: listNo=0 입력 시 상태 변화 없음 (뒤로가기)
// ---------------------------------------------------------------------------
TEST_F(ReleaseControllerTest, listNo_0_입력_시_상태_불변) {
    std::vector<Order> orders = {
        Order("ORD-20260508-0001", "S-001", "SK하이닉스", 150)
    };
    orders[0].setStatus(OrderStatus::CONFIRMED);
    orderRepo_.saveAll(orders);

    view_.listNo_ = 0;
    ReleaseController ctrl(orderRepo_, sampleRepo_, view_);
    ctrl.releaseOrder();

    auto saved = orderRepo_.loadAll();
    ASSERT_EQ(saved.size(), 1u);
    EXPECT_EQ(saved[0].getStatus(), OrderStatus::CONFIRMED);
}

// ---------------------------------------------------------------------------
// 테스트 5: releasedAt 형식 확인 ("YYYY-MM-DD HH:MM:SS")
// ---------------------------------------------------------------------------
TEST_F(ReleaseControllerTest, releasedAt_형식이_YYYY_MM_DD_HH_MM_SS_이어야_한다) {
    std::vector<Sample> samples = {
        Sample("S-001", "실리콘 웨이퍼-8인치", 3.0, 0.85, 500)
    };
    sampleRepo_.saveAll(samples);

    std::vector<Order> orders = {
        Order("ORD-20260508-0001", "S-001", "SK하이닉스", 150)
    };
    orders[0].setStatus(OrderStatus::CONFIRMED);
    orderRepo_.saveAll(orders);

    view_.listNo_ = 1;
    ReleaseController ctrl(orderRepo_, sampleRepo_, view_);
    ctrl.releaseOrder();

    auto saved = orderRepo_.loadAll();
    ASSERT_EQ(saved.size(), 1u);
    const std::string& dt = saved[0].getReleasedAt();

    EXPECT_EQ(dt.size(), 19u);
    EXPECT_EQ(dt[4],  '-');
    EXPECT_EQ(dt[7],  '-');
    EXPECT_EQ(dt[10], ' ');
    EXPECT_EQ(dt[13], ':');
    EXPECT_EQ(dt[16], ':');
}
