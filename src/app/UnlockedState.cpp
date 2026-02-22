#include "app/LockedState.h"
#include  "app/UnlockedState.h"
#include "app/State.h"
#include <memory>

namespace app
{

std::vector<MenuOption> UnlockedState::menu_options () const
{
    return 
    {
        { Action::AddEntry, "ADD NEW ENTRY" },
        { Action::ListEntries, "LIST ENTRIES" },
        { Action::RemoveEntry, "REMOVE ENTRY" },
        { Action::Save, "SAVE" },
        { Action::SaveAndClose, "SAVE AND CLOSE VAULT" }
    };
}

std::unique_ptr<State> UnlockedState::on_enter (Application& app)
{
    return nullptr;
}

bool UnlockedState::allows (Action action) const noexcept
{
    switch (action)
    {
        case Action::ListEntries:
        case Action::AddEntry:
        case Action::AlterEntry:
        case Action::RemoveEntry:
        case Action::Save:
        case Action::SaveAndClose:
            return true;
        default:
            return false;
    }
}

std::unique_ptr<State> UnlockedState::transition (Action action)
{
    switch (action)
    {
        case Action::SaveAndClose:
            return std::make_unique<LockedState>();
        default:
            return nullptr;
    }
}

}
