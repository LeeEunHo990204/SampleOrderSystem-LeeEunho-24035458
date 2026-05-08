#pragma once

class IController
{
public:
    virtual ~IController() = default;

    virtual void Run() = 0;

protected:
    virtual void HandleInput(int command) = 0;
};
