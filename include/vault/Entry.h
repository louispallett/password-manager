#pragma once

#include <string>

namespace vault
{
struct Entry
{
    std::string name;
    std::string username;
    std::string secret;

    bool operator==(const Entry& other) const noexcept
    {
        return name == other.name &&
               username == other.username &&
               secret == other.secret;
    }
};
}
