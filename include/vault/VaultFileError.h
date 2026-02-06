#pragma once

namespace vault 
{
enum class VaultFileError 
{
    FileNotFound,
    FileAlreadyExists,
    InvalidFormat,
    UnsupportedVersion,
    CryptoError,
    IOError,
};
}
