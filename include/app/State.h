#pragma once

#include "app/Action.h"

#include <memory>
#include <vector>
#include <string>

namespace app { class Application; }

namespace app
{

struct MenuOption
{
    Action action;
    std::string label;
};

class State
{
	public:
		virtual ~State() = default;

        virtual std::vector<MenuOption> menu_options() const = 0;
		
		// Called once when entering the state
		virtual std::unique_ptr<State> on_enter(Application& app) = 0;
		
        virtual bool allows(Action action) const noexcept = 0;

        virtual std::unique_ptr<State> transition(Action action) = 0;
};

} // namespace app
