#pragma once

#include <stdexcept>
#include <string>
namespace crypto 
{
enum class CryptoError 
{
    InvalidKey,
    InvalidNonce,
    InvalidSalt,
    KeyDerivationFailed,
    EncryptionFailed,
    DecryptionFailed,
    AuthenticationFailed,
    CryptoInitFailed
};

inline std::string to_string (CryptoError error)
{
    switch (error)
    {
        case CryptoError::InvalidKey:
            return "Invalid Key";
        case CryptoError::InvalidNonce:
            return "Invalid Nonce";
        case CryptoError::InvalidSalt:
            return "Invalid Salt";
        case CryptoError::KeyDerivationFailed:
            return "Key Derivation Failed";
        case CryptoError::EncryptionFailed:
            return "Encryption Failed";
        case CryptoError::DecryptionFailed:
            return "Decryption Failed";
        case CryptoError::AuthenticationFailed:
            return "Authentication Failed";
        case CryptoError::CryptoInitFailed:
            return "Crypto Initialisation Failed";
        default:
            throw std::invalid_argument("Unknown CryptoError value");
    }
}
}
