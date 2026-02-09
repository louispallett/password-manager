#pragma once

#include <filesystem>

#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/Vault.h"
#include "vault/VaultFileError.h"

namespace vault
{

// Define header constants
constexpr uint32_t VAULT_MAGIC = 0x5641554C;
constexpr uint8_t VAULT_VERSION = 1;
constexpr uint8_t KDF_TYPE_ARGON2ID = 1;

class VaultFile
{
    public:
        // --- Create New ---
        static util::Expected<void, VaultFileError> create_new (
            const std::filesystem::path& path,
            const util::SecureString& password
        );

        // --- Load Vault ---
        static util::Expected<Vault, VaultFileError> load (
            const std::filesystem::path& path,
            const util::SecureString& password
        );
        
        // --- Save Vault ---
        static util::Expected<void, VaultFileError> save (
            const std::filesystem::path& path,
            const Vault& vault,
            const util::SecureString& password
        );
};
}
