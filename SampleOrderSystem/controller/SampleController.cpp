// SampleController.cpp — Phase 2: 시료 관리 컨트롤러
#include "SampleController.h"
#include <algorithm>
#include <cctype>
#include <iostream>

SampleController::SampleController(SampleRepository& repo, SampleView& view)
    : repo_(repo), view_(view)
{
}

void SampleController::Run() {
    bool running = true;
    while (running) {
        int choice;
        if (pendingChoice_ >= 0) {
            choice = pendingChoice_;
            pendingChoice_ = -1;
        } else {
            view_.Render();
            choice = view_.GetMenuChoice();
        }
        if (choice == 0) {
            running = false;
        } else {
            HandleInput(choice);
        }
    }
}

void SampleController::HandleInput(int command) {
    switch (command) {
    case 1: onRegister(); break;
    case 2: onList();     break;
    case 3: onSearch();   break;
    default:
        view_.ShowMessage("잘못된 입력입니다.");
        break;
    }
}

// 공개 메서드 — 테스트에서 직접 호출 가능
void SampleController::registerSample() {
    onRegister();
}

std::vector<Sample> SampleController::searchByName(const std::string& keyword) {
    auto all = repo_.loadAll();
    std::vector<Sample> result;
    for (const auto& s : all) {
        if (s.getId().find(keyword) != std::string::npos ||
            s.getName().find(keyword) != std::string::npos) {
            result.push_back(s);
        }
    }
    return result;
}

// -------------------------------------------------------------------------
// Private helpers
// -------------------------------------------------------------------------

void SampleController::onRegister() {
    std::string id = view_.inputId();

    auto samples = repo_.loadAll();
    bool duplicate = std::any_of(samples.begin(), samples.end(),
        [&id](const Sample& s) { return s.getId() == id; });
    if (duplicate) {
        view_.ShowMessage("이미 등록된 ID입니다: " + id);
        return;
    }

    std::string name = view_.inputName();
    double avgTime   = view_.inputAvgTime();
    double yieldRate = view_.inputYieldRate();

    samples.push_back(Sample(id, name, avgTime, yieldRate, 0));
    repo_.saveAll(samples);
    view_.ShowMessage("시료 등록 완료: " + name);
}

void SampleController::onList() {
    auto samples = repo_.loadAll();
    int  page    = 0;
    bool paging  = true;

    while (paging) {
        view_.showSampleList(samples, page);

        int total    = static_cast<int>(samples.size());
        int lastPage = (total == 0) ? 0 : (total - 1) / 5;

        if (page >= lastPage) {
            view_.showInlineMenu();
            pendingChoice_ = view_.GetMenuChoice();
            paging = false;
        } else {
            std::string line;
            std::cout << "\n선택 > ";
            std::getline(std::cin, line);
            if (!line.empty() && (line[0] == 'N' || line[0] == 'n')) {
                page++;
            } else {
                paging = false;
            }
        }
    }
}

void SampleController::onSearch() {
    std::string keyword = view_.inputSearchName();
    auto result = searchByName(keyword);

    if (result.empty()) {
        view_.ShowMessage("검색 결과가 없습니다.");
        view_.showInlineMenu();
        pendingChoice_ = view_.GetMenuChoice();
    } else {
        view_.showSampleList(result, 0);
        view_.showInlineMenu();
        pendingChoice_ = view_.GetMenuChoice();
    }
}
