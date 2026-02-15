#pragma once

#include "app/Action.h"
#include "app/Application.h"

#include <memory>

namespace app { class Application; }

namespace app
{

class State
{
	public:
		virtual ~State() = default;
		
		// Called once when entering the state
		virtual void on_enter(Application& app) = 0;
		
        virtual bool allows(Action action) const noexcept = 0;

        virtual std::unique_ptr<State> transition(Action action) = 0;
};

} // namespace app
