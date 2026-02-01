#include "crypto/VaultCrypto.h"
#include <sodium.h>

namespace crypto 
{
util::Expected<ByteBuffer, CryptoError> VaultCrypto::derive_key(
    const util::SecureString& password,
    const ByteBuffer& salt
) 
{
    if (sodium_init() < 0) 
    {
        return CryptoError::CryptoInitFailed;
    }

    // Validate salt size
    if (salt.size() != SALT_SIZE) 
    {
        return CryptoError::InvalidSalt;
    }
    
    // Prepare output buffer
    ByteBuffer derived_key(crypto::KEY_SIZE);
    
    // Argon2id parameters (moderate security - adjust based on your needs)
    // opslimit: number of computational operations
    // memlimit: maximum memory usage in bytes
    constexpr unsigned long long opslimit = crypto_pwhash_OPSLIMIT_INTERACTIVE;
    constexpr std::size_t memlimit = crypto_pwhash_MEMLIMIT_INTERACTIVE;
    
    // Perform key derivation
    int result = crypto_pwhash(
        derived_key.data(),                    // output buffer
        derived_key.size(),                    // output length
        reinterpret_cast<const char*>(password.data()),  // password
        password.size(),                       // password length
        salt.data(),                          // salt
        opslimit,                             // computational cost
        memlimit,                             // memory cost
        crypto_pwhash_ALG_ARGON2ID13          // algorithm (Argon2id v1.3)
    );
    
    if (result != 0) 
    {
        sodium_memzero(derived_key.data(), derived_key.size());
        return CryptoError::KeyDerivationFailed;
    }
    
    return derived_key;
}
} // namespace crypto
