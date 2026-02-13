#pragma once

#include "app/Application.h"

#include <memory>

namespace app
{

enum class Action
{
	None,
    CreateVault,
	Unlock,
	AddEntry,
    AlterEntry,
	RemoveEntry,
	ListEntries,
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
