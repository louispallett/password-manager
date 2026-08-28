#include "app/HelpMenuOption.h"
#include <string>
#include <vector>

namespace app 
{

struct HelpMenu
{
    HelpMenuOption option;
    std::string label;
};

class Help final 
{
    public:
        std::vector<HelpMenu> help_menu_options() const;
};

}
