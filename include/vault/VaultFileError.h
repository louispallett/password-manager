#pragma once

#include <string>
#include <stdexcept>

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

inline std::string to_string(VaultFileError error)
{
    switch (error)
        {
            case VaultFileError::FileNotFound:       
                return "File not found";
            case VaultFileError::FileAlreadyExists:  
                return "File already exists";
            case VaultFileError::InvalidFormat:      
                return "Invalid file format";
            case VaultFileError::UnsupportedVersion: 
                return "Unsupported file version";
            case VaultFileError::CryptoError:        
                return "Cryptographic error";
            case VaultFileError::IOError:            
                return "I/O error";
            default: 
                throw std::invalid_argument("Unknown VaultFileError value");
        }
}
}
