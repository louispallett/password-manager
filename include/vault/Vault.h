#pragma once

#include <vector>
#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include "vault/Entry.h"
#include "vault/VaultFileError.h"

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

        crypto::ByteBuffer serialise() const;
        static util::Expected<Vault, VaultFileError> deserialise (const crypto::ByteBuffer& data);
    private:
        std::vector<Entry> entries_;
};
} // namespace vault
