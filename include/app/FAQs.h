#pragma once

#include "app/FAQOption.h"
#include <string>
#include <vector>

namespace app
{

struct FAQTopic
{
    FAQMenuOption  id;
    std::string    title;
    std::string    description;
};

const std::vector<FAQTopic>& FAQ_topics();

}

