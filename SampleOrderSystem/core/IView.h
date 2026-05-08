#pragma once
#include <string>

class IView
{
public:
    virtual ~IView() = default;

    virtual void Render()                             = 0;
    virtual void ShowMessage(const std::string& msg)  = 0;
    virtual int  GetMenuChoice()                      = 0;
};
