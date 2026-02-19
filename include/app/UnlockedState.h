#pragma once

#include "State.h"
#include <memory>

namespace app
{

class UnlockedState final : public State
{
    public:
        std::vector<MenuOption> menu_options() const override;

        std::unique_ptr<State> on_enter(Application& app) override;

        bool allows(Action action) const noexcept override;

        std::unique_ptr<State> transition(Action action) override;
};

}
