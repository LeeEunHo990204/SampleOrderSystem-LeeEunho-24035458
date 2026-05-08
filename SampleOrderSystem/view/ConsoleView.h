#pragma once
#include "../core/IView.h"
#include <string>

struct SystemStats {
    int sampleCount          = 0;
    int totalStock           = 0;
    int orderCount           = 0;
    int productionQueueCount = 0;
};

class ConsoleView : public IView {
public:
    void Render()                             override;
    void Render(const SystemStats& stats);
    void ShowMessage(const std::string& msg)  override;
    int  GetMenuChoice()                      override;

    std::string inputString(const std::string& prompt);
    int         inputInt(const std::string& prompt);
    double      inputDouble(const std::string& prompt);

    void printSeparator();
    void pause();

protected:
    static std::string currentDateTime();

private:
    void printAsciiArt() const;
    void printStatusBar(const SystemStats& stats) const;
    void printMenu() const;

    static std::string formatNumber(int n);
};
