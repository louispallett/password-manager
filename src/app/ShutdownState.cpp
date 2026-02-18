#include "app/ShutdownState.h"
#include "app/State.h"

namespace app
{

std::vector<MenuOption> ShutdownState::menu_options () const
{
    return 
    {
        { Action::None, "" },
    };
}

void ShutdownState::on_enter(Application& app)
{

}

bool ShutdownState::allows (Action action) const noexcept
{
    return false;
}

std::unique_ptr<State> ShutdownState::transition (Action action)
{
    return nullptr;
}

}
