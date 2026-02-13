#include "app/LockedState.h"
#include  "app/UnlockedState.h"
#include "app/State.h"
#include <memory>

namespace app
{

void UnlockedState::on_enter (Application& app)
{

}

bool UnlockedState::allows (Action action) const noexcept
{
    switch (action)
    {
        case Action::ListEntries:
        case Action::AddEntry:
        case Action::AlterEntry:
        case Action::RemoveEntry:
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
