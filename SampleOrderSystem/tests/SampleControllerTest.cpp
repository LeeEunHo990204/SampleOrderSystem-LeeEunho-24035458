// SampleControllerTest.cpp — Phase 2 TDD tests for SampleController
#include <gtest/gtest.h>
#include "../controller/SampleController.h"
#include "../view/SampleView.h"
#include "../repository/SampleRepository.h"
#include "TestHelpers.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

namespace {
    const std::string TEST_FILE = "test_sample_ctrl_tmp.json";
}

// ---------------------------------------------------------------------------
// StubSampleView: SampleView의 Stub — 입력값을 미리 세팅, 출력은 무시
// ---------------------------------------------------------------------------
class StubSampleView : public SampleView {
public:
    // 등록 입력 세팅
    void setInputId(const std::string& v)         { id_ = v; }
    void setInputName(const std::string& v)       { name_ = v; }
    void setInputAvgTime(double v)                { avgTime_ = v; }
    void setInputYieldRate(double v)              { yieldRate_ = v; }
    void setInputSearchName(const std::string& v) { searchName_ = v; }

    // SampleView override — 테스트에서는 실제 출력 불필요
    void Render()                                                   override {}
    void ShowMessage(const std::string&)                            override {}
    int  GetMenuChoice()                                            override { return 0; }
    void showSampleList(const std::vector<Sample>&, int)            override {}

    std::string inputId()                                           override { return id_; }
    std::string inputName()                                         override { return name_; }
    double      inputAvgTime()                                      override { return avgTime_; }
    double      inputYieldRate()                                    override { return yieldRate_; }
    std::string inputSearchName()                                   override { return searchName_; }

private:
    std::string id_;
    std::string name_;
    double      avgTime_    = 0.0;
    double      yieldRate_  = 0.0;
    std::string searchName_;
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class SampleControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 빈 JSON 배열로 초기화
        std::ofstream f(TEST_FILE);
        f << "[]";
    }

    void TearDown() override {
        std::remove(TEST_FILE.c_str());
    }

    SampleRepository  repo_{ TEST_FILE };
    StubSampleView    view_;
};

// ---------------------------------------------------------------------------
// 테스트 1: 시료 등록 후 loadAll에 포함된다
// ---------------------------------------------------------------------------
TEST_F(SampleControllerTest, 시료_등록_후_loadAll에_포함된다) {
    view_.setInputId("S-001");
    view_.setInputName("실리콘 웨이퍼");
    view_.setInputAvgTime(2.5);
    view_.setInputYieldRate(0.9);

    SampleController ctrl(repo_, view_);
    ctrl.registerSample();

    auto samples = repo_.loadAll();
    ASSERT_EQ(samples.size(), 1u);
    EXPECT_EQ(samples[0].getId(),   "S-001");
    EXPECT_EQ(samples[0].getName(), "실리콘 웨이퍼");
    EXPECT_DOUBLE_EQ(samples[0].getAvgProductionTime(), 2.5);
    EXPECT_DOUBLE_EQ(samples[0].getYieldRate(), 0.9);
    EXPECT_EQ(samples[0].getStock(), 0);
}

// ---------------------------------------------------------------------------
// 테스트 2: 중복 ID 등록은 저장되지 않는다
// ---------------------------------------------------------------------------
TEST_F(SampleControllerTest, 중복_ID_등록은_저장되지_않는다) {
    view_.setInputId("S-001");
    view_.setInputName("첫 번째 시료");
    view_.setInputAvgTime(1.0);
    view_.setInputYieldRate(0.8);

    SampleController ctrl(repo_, view_);
    ctrl.registerSample();  // 첫 번째 등록

    view_.setInputId("S-001");  // 동일 ID
    view_.setInputName("중복 시료");
    ctrl.registerSample();  // 두 번째 등록 시도

    auto samples = repo_.loadAll();
    EXPECT_EQ(samples.size(), 1u);  // 1개만 저장되어야 함
}

// ---------------------------------------------------------------------------
// 테스트 3: 이름 검색 결과가 일치한다 (부분 일치)
// ---------------------------------------------------------------------------
TEST_F(SampleControllerTest, 이름_검색_결과가_일치한다) {
    // 데이터 사전 준비
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼", 2.5, 0.9, 10),
        makeSample("S-002", "GaN 기판",     3.0, 0.85, 5),
        makeSample("S-003", "실리콘 카바이드", 4.0, 0.8, 0)
    };
    repo_.saveAll(samples);

    view_.setInputSearchName("실리콘");

    SampleController ctrl(repo_, view_);
    auto result = ctrl.searchByName("실리콘");

    ASSERT_EQ(result.size(), 2u);
    // 두 결과 모두 "실리콘" 포함
    for (const auto& s : result) {
        EXPECT_NE(s.getName().find("실리콘"), std::string::npos);
    }
}

// ---------------------------------------------------------------------------
// 테스트 4: 이름 검색 결과 없으면 빈 벡터
// ---------------------------------------------------------------------------
TEST_F(SampleControllerTest, 이름_검색_결과_없으면_빈_벡터) {
    std::vector<Sample> samples = {
        makeSample("S-001", "실리콘 웨이퍼", 2.5, 0.9, 10)
    };
    repo_.saveAll(samples);

    SampleController ctrl(repo_, view_);
    auto result = ctrl.searchByName("존재하지않는이름");

    EXPECT_TRUE(result.empty());
}
