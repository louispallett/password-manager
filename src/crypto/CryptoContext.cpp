#include "crypto/CryptoContext.h"
#include <sodium.h>
#include <sodium/core.h>

namespace crypto 
{

bool CryptoContext::init () noexcept
{
    return sodium_init() >= 0;
}

void random_bytes (ByteBuffer& out)
{

}

void secure_zero (ByteBuffer& buffer)
{

}

} // namespace crypto
