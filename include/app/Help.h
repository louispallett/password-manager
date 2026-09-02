#pragma once
#include "app/HelpMenuOption.h"
#include <string>
#include <vector>

namespace app
{

struct HelpTopic
{
    HelpMenuOption id;
    std::string    title;
    std::string    description;
};

const std::vector<HelpTopic>& help_topics();

}
