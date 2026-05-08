#include <gtest/gtest.h>
#include "../repository/SampleRepository.h"
#include "TestHelpers.h"
#include <cstdio>
#include <fstream>
#include <string>

namespace {
    const std::string TEST_SAMPLES_FILE = "test_samples_tmp.json";
}

class SampleRepositoryTest : public ::testing::Test {
protected:
    void TearDown() override {
        std::remove(TEST_SAMPLES_FILE.c_str());
    }
};

TEST_F(SampleRepositoryTest, 파일이_없으면_빈_벡터를_반환한다) {
    SampleRepository repo("nonexistent_file_xyz.json");
    auto result = repo.loadAll();
    EXPECT_TRUE(result.empty());
}

TEST_F(SampleRepositoryTest, 빈_배열_파일을_로드하면_빈_벡터를_반환한다) {
    {
        std::ofstream f(TEST_SAMPLES_FILE);
        f << "[]";
    }
    SampleRepository repo(TEST_SAMPLES_FILE);
    auto result = repo.loadAll();
    EXPECT_TRUE(result.empty());
}

TEST_F(SampleRepositoryTest, saveAll_후_loadAll하면_동일한_데이터를_반환한다) {
    std::vector<Sample> samples = {
        makeSample("S001", "실리콘 웨이퍼", 2.5, 0.9, 100),
        makeSample("S002", "GaN 기판", 3.0, 0.85, 50)
    };

    SampleRepository repo(TEST_SAMPLES_FILE);
    repo.saveAll(samples);

    auto loaded = repo.loadAll();
    ASSERT_EQ(loaded.size(), 2u);

    EXPECT_EQ(loaded[0].getId(),   "S001");
    EXPECT_EQ(loaded[0].getName(), "실리콘 웨이퍼");
    EXPECT_DOUBLE_EQ(loaded[0].getAvgProductionTime(), 2.5);
    EXPECT_DOUBLE_EQ(loaded[0].getYieldRate(), 0.9);
    EXPECT_EQ(loaded[0].getStock(), 100);

    EXPECT_EQ(loaded[1].getId(),   "S002");
    EXPECT_EQ(loaded[1].getName(), "GaN 기판");
    EXPECT_DOUBLE_EQ(loaded[1].getAvgProductionTime(), 3.0);
    EXPECT_DOUBLE_EQ(loaded[1].getYieldRate(), 0.85);
    EXPECT_EQ(loaded[1].getStock(), 50);
}
