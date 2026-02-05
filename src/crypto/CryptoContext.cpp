#include "crypto/CryptoContext.h"
#include <sodium.h>
#include <sodium/core.h>
#include <sodium/utils.h>

namespace crypto 
{

bool CryptoContext::init () noexcept 
{
    return sodium_init() >= 0;
}

void CryptoContext::random_bytes (ByteBuffer& buffer)
{
    randombytes_buf(buffer.data(), buffer.size());
}

void CryptoContext::secure_zero (ByteBuffer& buffer) noexcept
{
    sodium_memzero(buffer.data(), buffer.size());
}

} // namespace crypto
