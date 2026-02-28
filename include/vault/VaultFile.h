#pragma once

#include <filesystem>
#include <sodium/crypto_aead_xchacha20poly1305.h>
#include <sodium/crypto_pwhash.h>

#include "crypto/CryptoTypes.h"
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

constexpr std::size_t VAULT_HEADER_SIZE =
      sizeof(uint32_t) // magic
    + sizeof(uint8_t)  // version
    + sizeof(uint8_t)  // kdf_type
    + sizeof(uint16_t) // reserved
    + sizeof(uint32_t) // argon_mem_kib
    + sizeof(uint32_t) // argon_iters
    + sizeof(uint32_t) // argon_parallelism
    + crypto_pwhash_SALTBYTES
    + crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;

class VaultFile
{
    public:
        // --- Create New ---
        static util::Expected<void, VaultFileError> create_new (
            const std::filesystem::path& path,
            const util::SecureString& password
        );

        // --- Load Vault ---
        static util::Expected<VaultSession, VaultFileError> load (
            const std::filesystem::path& path,
            const util::SecureString& password
        );
        
        // --- Save Vault ---
        static util::Expected<void, VaultFileError> save (
            const std::filesystem::path& path,
            const Vault& vault,
            const crypto::ByteBuffer key
       );
};
}
