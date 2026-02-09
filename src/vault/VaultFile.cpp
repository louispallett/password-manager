#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <sodium.h>
#include <sodium/crypto_aead_xchacha20poly1305.h>
#include <span>
#include <vector>

#include "crypto/CryptoContext.h"
#include "crypto/CryptoTypes.h"
#include "vault/VaultFile.h"
#include "crypto/VaultCrypto.h"
#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/VaultFileError.h"

#pragma pack(push, 1)
struct VaultHeader
{
    VaultHeader() {};
    // TODO: (NOTE) Header fields are stored in host endianness (v1 format) - this is something
    // we will have to address ultimately.
    uint32_t magic;
    uint8_t  version;
    uint8_t  kdf_type;
    uint16_t reserved;

    uint32_t argon_mem_kib;
    uint32_t argon_iters;
    uint32_t argon_parallelism;

    uint8_t  salt[crypto_pwhash_SALTBYTES];
    uint8_t  nonce[crypto_aead_xchacha20poly1305_ietf_NPUBBYTES];

    std::span<const uint8_t> salt_view() const noexcept
    {
        return { salt, crypto_pwhash_SALTBYTES };
    }

    std::span<const uint8_t> nonce_view() const noexcept
    {
        return { nonce, crypto_aead_xchacha20poly1305_ietf_NPUBBYTES };
    }
};
#pragma pack(pop)


static_assert(sizeof(VaultHeader) == vault::VAULT_HEADER_SIZE, "VaultHeader size mismatch");

namespace vault 
{

namespace 
{
    util::Expected<VaultHeader, VaultFileError> read_and_validate_header(
        std::istream& file
    )
    {
        VaultHeader header;
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        if (!file)
        {
            return VaultFileError::InvalidFormat;
        }

        // Check  versions and header information
        if (header.magic != VAULT_MAGIC || header.kdf_type != KDF_TYPE_ARGON2ID)
        {
            return VaultFileError::InvalidFormat;
        }

        if (header.version != VAULT_VERSION)
        {
            return VaultFileError::UnsupportedVersion;
        }

        return header;
    }
}

// Note that CryptoContext::init() must be called by app before this runs
util::Expected<void, VaultFileError> VaultFile::create_new (
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

    // Create nonce
    crypto::ByteBuffer nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    crypto::CryptoContext::random_bytes(nonce);

    // // Serialise empty entries
    Vault vault;
    auto plaintext = vault.serialise();
    auto encrypted = crypto::VaultCrypto::encrypt(key.value(), nonce, plaintext);
    if (!encrypted)
    {
        return VaultFileError::CryptoError;
    }

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
        reinterpret_cast<const char*>(encrypted.value().data()),
        encrypted.value().size()
    );

    if (!file)
    {
        return VaultFileError::IOError;
    }

    file.close();

    return {};
}

util::Expected<Vault, VaultFileError> VaultFile::load (
    const std::filesystem::path& path,
    const util::SecureString& password
)
{
    // Read file
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        return VaultFileError::IOError;
    }

    // Get header
    auto header = read_and_validate_header(file);
    if (!header)
    {
        return header.error(); 
    }

    // Derive key
    // TODO: Currently not read Argon2 params... this is something we will utilately have to 
    // address.
    auto key = crypto::VaultCrypto::derive_key(password, header.value().salt);
    if (!key)
    {
        return VaultFileError::CryptoError;
    }

    // Decrypt blob
    crypto::ByteBuffer payload(
       (std::istreambuf_iterator<char>(file)),
       std::istreambuf_iterator<char>()
    );
    if (payload.empty())
    {
        return VaultFileError::InvalidFormat;
    }
    auto plaintext = crypto::VaultCrypto::decrypt(key.value(), header.value().nonce, payload);
    if (!plaintext)
    {
        return VaultFileError::CryptoError;
    }

    // Return our deserialised vault
    return Vault::deserialise(plaintext.value());
}


util::Expected<void, VaultFileError> vault::VaultFile::save (
    const std::filesystem::path& path,
    const Vault& vault,
    const util::SecureString& password
)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        return VaultFileError::IOError;
    }
    
    // Read header
    auto header = read_and_validate_header(file);
    if (!header)
    {
        return header.error();
    }

    // Derive key
    auto key = crypto::VaultCrypto::derive_key(password, header.value().salt);
    if (!key)
    {
        return VaultFileError::CryptoError;
    }

    // Generate new nonce
    crypto::ByteBuffer nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES);
    crypto::CryptoContext::random_bytes(nonce);

    // Copy new nonce to header
    std::memcpy(header.value().nonce, nonce.data(), nonce.size());

    auto plaintext = vault.serialise();
    auto encrypted = crypto::VaultCrypto::encrypt(key.value(), nonce, plaintext);
    if (!encrypted)
    {
        return VaultFileError::CryptoError;
    }
    
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output)
    {
        return VaultFileError::IOError;
    }

    output.write(
        reinterpret_cast<const char*>(&header.value()),
        sizeof(VaultHeader)
    );
    
    output.write(
        reinterpret_cast<const char*>(encrypted.value().data()),
        encrypted.value().size()
    );

    if (!output)
    {
        return VaultFileError::IOError;
    }

    return {};    
}
} // namespace vault
