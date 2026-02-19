#include "app/ShutdownState.h"
#include "app/State.h"
#include <memory>

namespace app
{

std::vector<MenuOption> ShutdownState::menu_options () const
{
    return 
    {
        { Action::None, "" },
    };
}

std::unique_ptr<State> ShutdownState::on_enter(Application& app)
{
    return nullptr;
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
