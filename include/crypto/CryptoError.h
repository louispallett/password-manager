#pragma once

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
}
