#pragma once

#include "State.h"

namespace app
{

class ShutdownState final : public State
{
    public:
        void on_enter(Application& app) override;

        std::unique_ptr<State> handle(
            Application& app,
            Action action
        ) override;
};

}

