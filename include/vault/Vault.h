#pragma once

#include <vector>
#include "vault/Entry.h"

namespace vault 
{
class Vault 
{
    public:
        Vault() = default;

        const std::vector<Entry>& entries () const noexcept
        {
            return entries_;
        }

        std::vector<Entry>& entries () noexcept
        {
            return entries_;
        }

        void add_entry (Entry entry)
        {
            entries_.push_back(std::move(entry));
        }

    private:
        std::vector<Entry> entries_;
};
}
