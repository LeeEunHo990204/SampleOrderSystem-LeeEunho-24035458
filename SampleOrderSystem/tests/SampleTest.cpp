#include <gtest/gtest.h>
#include "../model/Sample.h"

TEST(Sample_생성자, 전달된_값으로_초기화된다) {
    Sample s("S001", "실리콘 웨이퍼", 2.5, 0.9, 100);

    EXPECT_EQ(s.getId(),               "S001");
    EXPECT_EQ(s.getName(),             "실리콘 웨이퍼");
    EXPECT_DOUBLE_EQ(s.getAvgProductionTime(), 2.5);
    EXPECT_DOUBLE_EQ(s.getYieldRate(), 0.9);
    EXPECT_EQ(s.getStock(),            100);
}

TEST(Sample_addStock, 재고를_추가하면_수량이_증가한다) {
    Sample s("S001", "웨이퍼", 1.0, 0.8, 50);
    s.addStock(20);
    EXPECT_EQ(s.getStock(), 70);
}

TEST(Sample_reduceStock, 재고를_차감하면_수량이_감소한다) {
    Sample s("S001", "웨이퍼", 1.0, 0.8, 50);
    s.reduceStock(10);
    EXPECT_EQ(s.getStock(), 40);
}

TEST(Sample_reduceStock, 재고보다_많이_차감하면_예외를_던진다) {
    Sample s("S001", "웨이퍼", 1.0, 0.8, 5);
    EXPECT_THROW(s.reduceStock(10), std::invalid_argument);
}

TEST(Sample_addStock, 음수_수량_추가는_예외를_던진다) {
    Sample s("S001", "웨이퍼", 1.0, 0.8, 50);
    EXPECT_THROW(s.addStock(-1), std::invalid_argument);
}

TEST(Sample_reduceStock, 음수_수량_차감은_예외를_던진다) {
    Sample s("S001", "웨이퍼", 1.0, 0.8, 50);
    EXPECT_THROW(s.reduceStock(-1), std::invalid_argument);
}
