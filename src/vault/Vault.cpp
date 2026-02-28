#include "vault/Vault.h"
#include "crypto/CryptoTypes.h"
#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/Entry.h"
#include "vault/VaultError.h"
#include "vault/VaultFileError.h"
#include <cstdint>
#include <cstring>
#include <utility>


namespace vault
{

namespace
{

bool read_u32(
    const crypto::ByteBuffer& data,
    size_t& offset,
    uint32_t& out
)
{
    if (offset + sizeof(uint32_t) > data.size())
        return false;

    std::memcpy(&out, data.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    return true;
}

bool read_secure_string(
    const crypto::ByteBuffer& data,
    size_t& offset,
    util::SecureString& out
)
{
    uint32_t len;
    if (!read_u32(data, offset, len))
    {
        return false;
    }

    if (offset + len > data.size())
    {
        return false;
    }

    out.assign( 
        reinterpret_cast<const char*>(data.data() + offset),
        len
    );
    offset += len;
    return true;
}

} // unnamed namespace

util::Expected<void, VaultError> Vault::add_entry (Entry entry)
{
    for (const auto& e : entries_)
    {
        if (e.name == entry.name)
        {
            return VaultError::DuplicateEntry;
        }
    }
    entries_.push_back(std::move(entry));
    return {};
}

util::Expected<void, VaultError> Vault::update_entry(
    size_t index,
    Entry updated
)
{
    if (index >= entries_.size())
    {
        return VaultError::EntryNotFound;
    }

    entries_[index] = std::move(updated);

    return {};
}

util::Expected<void, VaultError> Vault::remove_entry(
    size_t index
)
{
    if (index >= entries_.size())
    {
        return VaultError::EntryNotFound;
    }

    entries_.erase(entries_.begin() + index);
    return {};
}

crypto::ByteBuffer Vault::serialise() const
{
    crypto::ByteBuffer out;

    auto append_u32 = [&out](uint32_t v)
    {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        out.insert(out.end(), p, p + sizeof(v));
    };

    auto append_string = [&out, &append_u32](const util::SecureString& s)
    {
        append_u32(static_cast<uint32_t>(s.size())); 
        out.insert(out.end(), s.data(), s.data() + s.size()); 
    };

    // Entry count
    append_u32(static_cast<uint32_t>(entries_.size()));

    // Entries
    for (const Entry& e : entries_)
    {
        append_string(e.name); 
        append_string(e.username); 
        append_string(e.secret); 
    }

    return out;
}

util::Expected<Vault, VaultFileError> Vault::deserialise(
    const crypto::ByteBuffer& data
)
{
    Vault vault;
    size_t offset = 0;

    uint32_t count;
    if (!read_u32(data, offset, count))
    {
        return VaultFileError::InvalidFormat;
    }

    for (uint32_t i = 0; i < count; ++i)
    {
        util::SecureString name{""};
        util::SecureString username{""};
        util::SecureString secret{""};

        if (!read_secure_string(data, offset, name) || 
            !read_secure_string(data, offset, username) || 
            !read_secure_string(data, offset, secret)) 
        {
            return VaultFileError::InvalidFormat;
        }

        vault.add_entry(Entry{
            std::move(name),
            std::move(username),
            std::move(secret)
        });
    }

    // Extra trailing garbage = corruption
    if (offset != data.size())
    {
        return VaultFileError::InvalidFormat;
    }

    return vault;
}

}
