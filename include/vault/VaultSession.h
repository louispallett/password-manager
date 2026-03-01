#pragma once

#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include <filesystem>
#include <utility>
#include "vault/Vault.h"
#include "vault/Entry.h"

namespace vault { enum class VaultError; }
namespace vault { enum class VaultFileError; }

namespace vault
{

class VaultSession
{
    public:
        VaultSession(
            Vault vault,
            crypto::ByteBuffer key,
            std::filesystem::path path
        ) : 
        vault_(std::move(vault)),
        key_(std::move(key)),
        path_(std::move(path))
        {}

        ~VaultSession();

        VaultSession(const VaultSession&) = delete;
        VaultSession& operator=(const VaultSession&) = delete;

        VaultSession(VaultSession&&) noexcept = default;
        VaultSession& operator=(VaultSession&&) noexcept = default;

        // vault::Vault functions
        bool is_empty() const;
        const std::vector<Entry>& entries () const noexcept;
        util::Expected<void, VaultError> add_entry (Entry entry);
        util::Expected<void, VaultError> remove_entry (size_t index);

        util::Expected<void, VaultFileError> save();

    private:
        Vault vault_;
        crypto::ByteBuffer key_;
        std::filesystem::path path_;
};

}
