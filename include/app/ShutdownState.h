#pragma once

#include "State.h"

namespace app
{

class ShutdownState final : public State
{
    public:
        void on_enter(Application& app) override;

        bool allows(Action action) const noexcept override;

        std::unique_ptr<State> transition(Action action) override;
};

}

