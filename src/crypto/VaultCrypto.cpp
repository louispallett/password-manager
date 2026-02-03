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
    if (salt.size() != crypto_pwhash_SALTBYTES) 
    {
        return CryptoError::InvalidSalt;
    }
    
    // Prepare output buffer
    ByteBuffer derived_key(crypto::KEY_SIZE);
    
    // Perform key derivation
    int result = crypto_pwhash(
        derived_key.data(),                   // output buffer
        derived_key.size(),                   // output length
        reinterpret_cast<const char*>(password.data()),  // password
        password.size(),                      // password length
        salt.data(),                          // salt
        // crypto_pwhash_OPSLIMIT_SENSITIVE,     // computational cost
        // crypto_pwhash_MEMLIMIT_SENSITIVE,     // memory cost
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE,
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
