#pragma once

#include "crypto/CryptoConstants.h"
#include "crypto/CryptoTypes.h"

namespace crypto 
{
class CryptoContext 
{
    public:
        static bool init() noexcept;

        static void random_bytes(ByteBuffer& buffer);

        static void secure_zero(ByteBuffer& buffer) noexcept;
};
}
