#pragma once

#include <vector>
#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include "vault/Entry.h"
#include "vault/VaultError.h"
#include "vault/VaultFileError.h"

namespace vault 
{

class Vault 
{
    public:
        const std::vector<Entry>& entries () const noexcept
        {
            return entries_;
        }

        void add_entry (Entry entry);

        util::Expected<void, VaultError> update_entry (
            size_t index,
            Entry updated
        );

        util::Expected<void, VaultError> remove_entry (
            size_t index
        );

        crypto::ByteBuffer serialise() const;

        static util::Expected<Vault, VaultFileError> deserialise (
            const crypto::ByteBuffer& data
        );

    private:
        std::vector<Entry> entries_;
};

} // namespace vault
