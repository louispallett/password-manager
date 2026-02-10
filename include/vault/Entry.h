#pragma once

#include "util/SecureString.h"

namespace vault
{
struct Entry
{
    util::SecureString name;
    util::SecureString username;
    util::SecureString secret;

    Entry(
        util::SecureString name_,
        util::SecureString username_,
        util::SecureString secret_
    )
        : name(std::move(name_))
        , username(std::move(username_))
        , secret(std::move(secret_))
    {}

    bool operator==(const Entry& other) const noexcept
    {
        return name == other.name &&
               username == other.username &&
               secret == other.secret;
    }
};
}
