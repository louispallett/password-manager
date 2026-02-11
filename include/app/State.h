#pragma once

#include <memory>

namespace app
{

class Application;

enum class Action
{
	None,
	OpenVault,
	Unlock,
	Lock,
	AddEntry,
	RemoveEntry,
	ListEntries,
	Save,
	Quit
};

class State
{
	public:
		virtual ~State() = default;
		
		// Called once when entering the state
		virtual void on_enter(Application& app) = 0;
		
		// Handle a user action and decide what happens next
		virtual std::unique_ptr<State> handle(
			Application& app,
			Action action
		) = 0;
};

} // namespace app
