#include "app/Help.h"
#include "app/HelpMenuOption.h"


namespace app 
{
    std::vector<HelpMenu> Help::help_menu_options() const
    {
        return  
        {
            { HelpMenuOption::HowItWorks, "How it works" },
            { HelpMenuOption::Vault, "What is a vault?" },
            { HelpMenuOption::MasterPassword, "Master Password" },
            { HelpMenuOption::AddEntry, "Adding an Entry" },
            { HelpMenuOption::EditEntry, "Editing Entries" },
            { HelpMenuOption::RemoveEntry, "Removing Entries" }
        };
    }
}
