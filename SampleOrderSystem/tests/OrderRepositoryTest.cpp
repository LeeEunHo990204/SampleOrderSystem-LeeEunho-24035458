#include <gtest/gtest.h>
#include "../repository/OrderRepository.h"
#include "TestHelpers.h"
#include <cstdio>
#include <fstream>
#include <string>

namespace {
    const std::string TEST_ORDERS_FILE = "test_orders_tmp.json";
}

class OrderRepositoryTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove(TEST_ORDERS_FILE.c_str());
    }
};

TEST_F(OrderRepositoryTest, 파일이_없으면_빈_벡터를_반환한다) {
    OrderRepository repo("nonexistent_orders_xyz.json");
    auto result = repo.loadAll();
    EXPECT_TRUE(result.empty());
}

TEST_F(OrderRepositoryTest, 빈_배열_파일을_로드하면_빈_벡터를_반환한다) {
    {
        std::ofstream f(TEST_ORDERS_FILE);
        f << "[]";
    }
    OrderRepository repo(TEST_ORDERS_FILE);
    auto result = repo.loadAll();
    EXPECT_TRUE(result.empty());
}

TEST_F(OrderRepositoryTest, saveAll_후_loadAll하면_동일한_데이터를_반환한다) {
    std::vector<Order> orders = {
        makeOrder("O001", "S001", "홍길동", 10),
        makeOrder("O002", "S002", "김영희", 5)
    };
    orders[0].setStatus(OrderStatus::PRODUCING);
    orders[0].setProducedQty(3);

    OrderRepository repo(TEST_ORDERS_FILE);
    repo.saveAll(orders);

    auto loaded = repo.loadAll();
    ASSERT_EQ(loaded.size(), 2u);

    EXPECT_EQ(loaded[0].getId(),           "O001");
    EXPECT_EQ(loaded[0].getSampleId(),     "S001");
    EXPECT_EQ(loaded[0].getCustomerName(), "홍길동");
    EXPECT_EQ(loaded[0].getQuantity(),     10);
    EXPECT_EQ(loaded[0].getStatus(),       OrderStatus::PRODUCING);
    EXPECT_EQ(loaded[0].getProducedQty(),  3);

    EXPECT_EQ(loaded[1].getId(),           "O002");
    EXPECT_EQ(loaded[1].getSampleId(),     "S002");
    EXPECT_EQ(loaded[1].getCustomerName(), "김영희");
    EXPECT_EQ(loaded[1].getQuantity(),     5);
    EXPECT_EQ(loaded[1].getStatus(),       OrderStatus::RESERVED);
    EXPECT_EQ(loaded[1].getProducedQty(),  0);
}
