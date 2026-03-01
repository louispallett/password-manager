#pragma once

#include <vector>
#include "crypto/CryptoTypes.h"
#include "util/Expected.h"

namespace vault { class Entry; }
namespace vault { enum class VaultError; }
namespace vault { enum class VaultFileError; }

namespace vault 
{

class Vault 
{
    public:
        const std::vector<Entry>& entries () const noexcept
        {
            return entries_;
        }

        util::Expected<void, VaultError> add_entry (Entry entry);

        util::Expected<void, VaultError> update_entry (
            size_t index,
            Entry updated
        );

        util::Expected<void, VaultError> remove_entry (size_t index);

        crypto::ByteBuffer serialise() const;

        static util::Expected<Vault, VaultFileError> deserialise (
            const crypto::ByteBuffer& data
        );

    private:
        std::vector<Entry> entries_;
};

} // namespace vault
