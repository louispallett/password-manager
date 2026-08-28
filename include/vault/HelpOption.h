#pragma once

#include "util/SecureString.h"
namespace vault 
{
struct HelpOption
{
    util::SecureString name;
    util::SecureString description;

    HelpOption(
        util::SecureString name_,
        util::SecureString description_
    )
        : name(std::move(name_))
        , description(std::move(description_))
    {}

    bool operator==(const HelpOption& other) const noexcept
    {
        return name == other.name && description == other.description;
    }
};
}
