#pragma once

#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include <filesystem>
#include "vault/Vault.h"

namespace vault { enum class VaultFileError; }

namespace vault
{

class VaultSession
{
    public:
        VaultSession(
                Vault vault,
                crypto::ByteBuffer key,
                std::filesystem::path path
        );

        ~VaultSession();

        Vault& vault();
        const Vault& vault() const;

        util::Expected<void, VaultFileError> save();

    private:
        Vault vault_;
        crypto::ByteBuffer key_;
        std::filesystem::path path_;
};

}
