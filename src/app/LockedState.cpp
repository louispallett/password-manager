#include "app/LockedState.h"
#include "app/State.h"
#include "app/UnlockedState.h"
#include "app/ShutdownState.h"
#include <memory>

namespace app
{

std::vector<MenuOption> LockedState::menu_options () const
{
    return 
    {
        { Action::Unlock, "Unlock Vault" },
        { Action::Quit, "Quit" }
    };
}

std::unique_ptr<State> LockedState::on_enter (Application& app)
{
    return nullptr;
}

bool LockedState::allows (Action action) const noexcept
{
    switch (action) 
    {
        case Action::Unlock:
        case Action::Quit:
            return true;
        default:
            return false;
    }
}

std::unique_ptr<State> LockedState::transition (Action action)
{
    switch (action) 
    {
        case Action::Unlock:
            return std::make_unique<UnlockedState>();
        case Action::Quit:
            return std::make_unique<ShutdownState>();
        default:
            return nullptr;
    }
}

} // namespace app
