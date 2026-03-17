#pragma once

#include "crypto/CryptoTypes.h"

namespace util { class SecureString; }

namespace crypto 
{
class CryptoContext 
{
    public:
        static bool init() noexcept;

        static void random_bytes(ByteBuffer& buffer);

        static void secure_zero(ByteBuffer& buffer) noexcept;

        static void secure_zero(util::SecureString& str) noexcept;
};
}
