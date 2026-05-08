#include <gtest/gtest.h>
#include "../model/OrderStatus.h"

// --- toString ---

TEST(OrderStatus_toString, RESERVED를_문자열로_변환한다) {
    EXPECT_EQ(toString(OrderStatus::RESERVED), "RESERVED");
}

TEST(OrderStatus_toString, REJECTED를_문자열로_변환한다) {
    EXPECT_EQ(toString(OrderStatus::REJECTED), "REJECTED");
}

TEST(OrderStatus_toString, PRODUCING을_문자열로_변환한다) {
    EXPECT_EQ(toString(OrderStatus::PRODUCING), "PRODUCING");
}

TEST(OrderStatus_toString, CONFIRMED를_문자열로_변환한다) {
    EXPECT_EQ(toString(OrderStatus::CONFIRMED), "CONFIRMED");
}

TEST(OrderStatus_toString, RELEASE를_문자열로_변환한다) {
    EXPECT_EQ(toString(OrderStatus::RELEASE), "RELEASE");
}

// --- fromString ---

TEST(OrderStatus_fromString, RESERVED_문자열을_열거형으로_변환한다) {
    EXPECT_EQ(fromString("RESERVED"), OrderStatus::RESERVED);
}

TEST(OrderStatus_fromString, REJECTED_문자열을_열거형으로_변환한다) {
    EXPECT_EQ(fromString("REJECTED"), OrderStatus::REJECTED);
}

TEST(OrderStatus_fromString, PRODUCING_문자열을_열거형으로_변환한다) {
    EXPECT_EQ(fromString("PRODUCING"), OrderStatus::PRODUCING);
}

TEST(OrderStatus_fromString, CONFIRMED_문자열을_열거형으로_변환한다) {
    EXPECT_EQ(fromString("CONFIRMED"), OrderStatus::CONFIRMED);
}

TEST(OrderStatus_fromString, RELEASE_문자열을_열거형으로_변환한다) {
    EXPECT_EQ(fromString("RELEASE"), OrderStatus::RELEASE);
}

TEST(OrderStatus_fromString, 알수없는_문자열은_예외를_던진다) {
    EXPECT_THROW(fromString("UNKNOWN"), std::invalid_argument);
}
