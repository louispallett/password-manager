#pragma once

#include <stdexcept>
#include <string>
namespace vault
{

enum class VaultError
{
    DuplicateEntry,
    EntryNotFound
};

inline std::string to_string(VaultError error)
{
    switch (error)
    {
        case VaultError::DuplicateEntry:
            return "Duplicate Entry";
        case VaultError::EntryNotFound:
            return "Entry not found";
        default:
            throw std::invalid_argument("Unknown Vault Error vault");
    }
}

}
