#include <cstdint>
#include <cstring>
#include <fstream>
#include <sodium.h>
#include <sodium/crypto_aead_xchacha20poly1305.h>
#include <vector>
#include "crypto/CryptoContext.h"
#include "crypto/CryptoTypes.h"
#include "vault/VaultFile.h"
#include "crypto/VaultCrypto.h"
#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/VaultFileError.h"


// Define header constants
#define VAULT_MAGIC 0x5641554C
#define VAULT_VERSION 1
#define KDF_TYPE_ARGON2ID 1

#pragma pack(push, 1)
struct VaultHeader
{
    uint32_t magic;          
    uint8_t  version;        
    uint8_t  kdf_type;       
    uint16_t reserved;       

    uint32_t argon_mem_kib;
    uint32_t argon_iters;
    uint32_t argon_parallelism;

    uint8_t  salt[crypto_pwhash_SALTBYTES];
    uint8_t  nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];
};
#pragma pack(pop)

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

static_assert(sizeof(VaultHeader) == VAULT_HEADER_SIZE, "VaultHeader size mismatch");

namespace 
{
crypto::ByteBuffer serialise_entries(const std::vector<vault::Entry>& entries)
{
    crypto::ByteBuffer out;
    out.reserve(sizeof(uint32_t));

    uint32_t count = static_cast<uint32_t>(entries.size());
    out.insert(
        out.end(),
        reinterpret_cast<const uint8_t*>(&count),
        reinterpret_cast<const uint8_t*>(&count) + sizeof(count)
    );

    return out;
}
} // unnamed namespace

namespace vault 
{
// Note that CryptoContext::init() must be called by app before this runs
util::Expected<void, VaultFileError> vault::VaultFile::create_new (
    const std::filesystem::path& path,
    const util::SecureString& password
)
{
    if (std::filesystem::exists(path))
    {
    	return VaultFileError::FileAlreadyExists;
    }

    // Generate Salt
    constexpr std::size_t SALT_SIZE = crypto_pwhash_SALTBYTES;
    crypto::ByteBuffer salt(SALT_SIZE);
    crypto::CryptoContext::random_bytes(salt);

    // Generate key
    auto key = crypto::VaultCrypto::derive_key(password, salt);
    if (!key)
    {
        return VaultFileError::CryptoError;
    }

    // Create file
    std::ofstream file(path, std::ios::binary);
    if (!file)
    {
	    return VaultFileError::IOError;
    }

    // Serialise empty entries
    std::vector<Entry> entries;
    crypto::ByteBuffer plain_text = serialise_entries(entries);

    // Encrypt blob
    auto encrypted_blob = crypto::VaultCrypto::encrypt(key.value(), plain_text);
    if (!encrypted_blob)
    {
        return VaultFileError::CryptoError;
    }

    // Extract nonce
    const auto& encrypted_vec = encrypted_blob.value();

    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    crypto::ByteBuffer nonce(encrypted_vec.begin(), encrypted_vec.begin() + NONCE_SIZE);
    crypto::ByteBuffer cipher(encrypted_vec.begin() + NONCE_SIZE, encrypted_vec.end());

    VaultHeader header{};
    header.magic = VAULT_MAGIC;
    header.version = VAULT_VERSION;
    header.kdf_type = KDF_TYPE_ARGON2ID;
    header.reserved = 0;

    header.argon_mem_kib = crypto::ARGON_MEM_KIB;
    header.argon_iters = crypto::ARGON_ITERS;
    header.argon_parallelism = crypto::ARGON_PARALLELISM;

    std::memcpy(header.salt, salt.data(), salt.size());
    std::memcpy(header.nonce, nonce.data(), nonce.size());

    file.write(
        reinterpret_cast<const char*>(&header),
        sizeof(header)
    );
    
    file.write(
        reinterpret_cast<const char*>(cipher.data()), 
        cipher.size()
    );

    file.close();

    return {};
}

util::Expected<Vault, VaultFileError> vault::VaultFile::load (
    const std::filesystem::path& path,
    const util::SecureString& password
)
{

}

util::Expected<void, VaultFileError> vault::VaultFile::save (
    const std::filesystem::path& path,
    const Vault& vault,
    const util::SecureString& password
)
{

}
} // namespace vault
