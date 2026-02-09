#pragma once

#include "crypto/CryptoTypes.h"
#include "crypto/CryptoError.h"
#include "util/Expected.h"
#include "util/SecureString.h"
#include <span>

namespace crypto 
{
class VaultCrypto 
{
    public:
        // --- Key derivation ---
        // Argon2id(password, salt) -> symmetric key
        static util::Expected<ByteBuffer, CryptoError> derive_key (
	        const util::SecureString& password,
	        std::span<const uint8_t> salt
        );

        // --- Encryption ---
        // AEAD encrypt (XChaCha20-Poly1305)
        static util::Expected<ByteBuffer, CryptoError> encrypt (
	        const ByteBuffer& key,
            const ByteBuffer& nonce,
	        const ByteBuffer& plaintext
        );

         // --- Decryption ---   
        static util::Expected<ByteBuffer, CryptoError> decrypt (
	        const ByteBuffer& key,
            std::span<const uint8_t> nonce,
	        const ByteBuffer& ciphertext
        );
};
}
