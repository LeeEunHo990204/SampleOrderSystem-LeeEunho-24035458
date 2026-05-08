#pragma once
#include "ConsoleView.h"
#include "../model/Sample.h"
#include <vector>
#include <string>

class SampleView : public ConsoleView {
public:
    // 서브메뉴 렌더링: [1] 시료 등록  [2] 시료 목록  [3] 시료 검색  [0] 뒤로
    void Render() override;

    // 시료 목록 출력 (5개/페이지, [N] 다음 페이지)
    virtual void showSampleList(const std::vector<Sample>& samples, int page);

    // 목록 하단에 서브메뉴를 인라인으로 출력 (화면 클리어 없음)
    void showInlineMenu();

    // 입력 메서드
    virtual std::string inputId();
    virtual std::string inputName();
    virtual double      inputAvgTime();
    virtual double      inputYieldRate();
    virtual std::string inputSearchName();
};
