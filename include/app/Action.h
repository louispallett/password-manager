#pragma once

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

}
