#pragma once

namespace app
{

enum class Action
{
	None,
    CreateVault,
	Unlock,
	AddEntry,
	RemoveEntry,
	ListEntries,
    Save,
	SaveAndClose,
	Quit
};

}
