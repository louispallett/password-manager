#include "app/BootstrapState.h"
#include "app/State.h"
#include "app/LockedState.h"
#include "app/ShutdownState.h"
#include "app/Application.h"
#include <memory>

namespace app
{

std::vector<MenuOption> BootstrapState::menu_options () const
{
    return 
    {
        { Action::CreateVault, "CREATE VAULT" },
	      { Action::Help, "HELP" },
        { Action::FAQs, "FAQs" },
        { Action::Quit, "QUIT" }
    };
}

std::unique_ptr<State> BootstrapState::on_enter (Application& app)
{
   if (app.vault_exists())
   {
       return std::make_unique<LockedState>();
   }
   return nullptr;
}

bool BootstrapState::allows (Action action) const noexcept
{
    switch (action)
    {
        case Action::CreateVault:
	      case Action::Help:
        case Action::FAQs:
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
