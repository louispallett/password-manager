#pragma once

#include "State.h"

namespace app
{

class UnlockedState final : public State
{
    public:
        std::vector<MenuOption> menu_options() const override;

        void on_enter(Application& app) override;

        bool allows(Action action) const noexcept override;

        std::unique_ptr<State> transition(Action action) override;
};

}
