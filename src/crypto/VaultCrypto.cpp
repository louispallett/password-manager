#include "crypto/CryptoConstants.h"
#include "crypto/VaultCrypto.h"
#include "crypto/CryptoTypes.h"
#include <cstring>
#include <sodium.h>
#include <sodium/crypto_aead_chacha20poly1305.h>
#include <sodium/crypto_aead_xchacha20poly1305.h>
#include <sodium/utils.h>
#include <span>

namespace crypto 
{
util::Expected<ByteBuffer, CryptoError> VaultCrypto::derive_key (
    const util::SecureString& password,
    std::span<const uint8_t> salt
) 
{
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

util::Expected<ByteBuffer, CryptoError> VaultCrypto::encrypt (
    const ByteBuffer& key,
    const ByteBuffer& nonce,
    const ByteBuffer& plaintext
)
{
    if (key.size() != crypto_aead_xchacha20poly1305_ietf_KEYBYTES)
    {
        return CryptoError::InvalidKey;
    }

    ByteBuffer output(
        plaintext.size() + 
        crypto_aead_xchacha20poly1305_ietf_ABYTES
    );

    unsigned long long ciphertext_len = 0;

    int rc = crypto_aead_xchacha20poly1305_ietf_encrypt(
        output.data(),                // output buffer
        &ciphertext_len,         // output size counter
        plaintext.data(),             // plaintext to encrypt
        plaintext.size(),          // plaintext size
        nullptr,                     // additional authenticated data (not using)
        0,                        // additional authenticated data length
        nullptr,                   // Secret nonce - for XChaCha20-Poly1305 this should always be NULL
        nonce.data(),              // Our nonce
        key.data()                    // Our key
    );

    if (rc != 0)
    {
        sodium_memzero(output.data(), output.size());
        return CryptoError::EncryptionFailed;
    }
    if (ciphertext_len != output.size())
    {
        output.resize(ciphertext_len);
    }
    return output; 
}

util::Expected<ByteBuffer, CryptoError> VaultCrypto::decrypt (
    const ByteBuffer& key,
    std::span<const uint8_t> nonce,
    const ByteBuffer& ciphertext
)
{
    if (key.size() != crypto_aead_xchacha20poly1305_ietf_KEYBYTES)
    {
        return CryptoError::InvalidKey;
    }

    constexpr std::size_t TAG_SIZE = crypto_aead_xchacha20poly1305_ietf_ABYTES;

    if (ciphertext.size() < TAG_SIZE) 
    {
        return CryptoError::DecryptionFailed;
    }

    ByteBuffer output(
        ciphertext.size() - crypto_aead_xchacha20poly1305_ietf_ABYTES
    );

    unsigned long long plaintext_len = 0;

    int rc = crypto_aead_xchacha20poly1305_ietf_decrypt(
        output.data(),
        &plaintext_len,
        nullptr,
        ciphertext.data(),
        ciphertext.size(),
        nullptr, 
        0,
        nonce.data(),
        key.data()
    );

    if (rc != 0)
    {
        sodium_memzero(output.data(), output.size());
        return CryptoError::DecryptionFailed;
    }

    
    if (plaintext_len != output.size()) 
    {
        output.resize(plaintext_len);
    }
    return output; 
}

} // namespace crypto
