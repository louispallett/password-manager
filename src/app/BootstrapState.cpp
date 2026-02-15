#include "app/BootstrapState.h"
#include "app/State.h"
#include "app/LockedState.h"
#include "app/ShutdownState.h"

namespace app
{

void BootstrapState::on_enter (Application& app)
{
   // if (app.vault_exists())
   // {
   //      app.change_state(std::make_unique<LockedState>());
   // }
}

bool BootstrapState::allows (Action action) const noexcept
{
    switch (action)
    {
        case Action::CreateVault:
        case Action::Quit:
            return true;
        default:
            return false;
    }
}

std::unique_ptr<State> BootstrapState::transition (Action action)
{
    switch (action)
    {
        case Action::CreateVault:
            return std::make_unique<LockedState>();
        case Action::Quit:
            return std::make_unique<ShutdownState>();
        default:
            return nullptr;
    }
}

}
