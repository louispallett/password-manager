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

void secure_zero (ByteBuffer& buffer)
{

}

} // namespace crypto
