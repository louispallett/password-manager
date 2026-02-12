#pragma once

#include <memory>

namespace app
{

class Application;

enum class Action
{
	None,
    CreateVault,
	Unlock,
	AddEntry,
    AlterEntry,
	RemoveEntry,
	ListEntry,
	SaveAndClose,
	Quit
};

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
