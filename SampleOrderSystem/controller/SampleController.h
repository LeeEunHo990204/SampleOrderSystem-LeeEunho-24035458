#pragma once
#include "../core/IController.h"
#include "../repository/SampleRepository.h"
#include "../view/SampleView.h"
#include "../model/Sample.h"
#include <vector>
#include <string>

class SampleController : public IController {
public:
    SampleController(SampleRepository& repo, SampleView& view);

    void Run() override;

    // 테스트에서 직접 호출 가능한 공개 메서드
    void registerSample();
    std::vector<Sample> searchByName(const std::string& keyword);

protected:
    void HandleInput(int command) override;

private:
    void onRegister();
    void onList();
    void onSearch();

    SampleRepository& repo_;
    SampleView&       view_;
    int pendingChoice_ = -1;  // 인라인 메뉴에서 받은 다음 선택값
};
