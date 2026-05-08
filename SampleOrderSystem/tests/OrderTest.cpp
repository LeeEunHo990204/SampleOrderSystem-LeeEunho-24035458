#include <gtest/gtest.h>
#include "../model/Order.h"

TEST(Order_생성자, 전달된_값으로_초기화된다) {
    Order o("O001", "S001", "홍길동", 10);

    EXPECT_EQ(o.getId(),           "O001");
    EXPECT_EQ(o.getSampleId(),     "S001");
    EXPECT_EQ(o.getCustomerName(), "홍길동");
    EXPECT_EQ(o.getQuantity(),     10);
    EXPECT_EQ(o.getStatus(),       OrderStatus::RESERVED);
    EXPECT_EQ(o.getProducedQty(),  0);
}

TEST(Order_setStatus, 상태를_변경하면_새로운_상태가_반환된다) {
    Order o("O001", "S001", "홍길동", 10);
    o.setStatus(OrderStatus::PRODUCING);
    EXPECT_EQ(o.getStatus(), OrderStatus::PRODUCING);
}

TEST(Order_setProducedQty, 생산수량을_설정하면_해당_값이_반환된다) {
    Order o("O001", "S001", "홍길동", 10);
    o.setProducedQty(5);
    EXPECT_EQ(o.getProducedQty(), 5);
}

TEST(Order_setProducedQty, 생산수량은_음수일_수_없다) {
    Order o("O001", "S001", "홍길동", 10);
    EXPECT_THROW(o.setProducedQty(-1), std::invalid_argument);
}
