#include "vault/Vault.h"
#include "crypto/CryptoTypes.h"
#include <cstdint>
#include <cstring>

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

bool read_string(
    const crypto::ByteBuffer& data,
    size_t& offset,
    std::string& out
)
{
    uint32_t len;
    if (!read_u32(data, offset, len))
        return false;

    if (offset + len > data.size())
        return false;

    out.assign(
        reinterpret_cast<const char*>(data.data() + offset),
        len
    );
    offset += len;
    return true;
}
}

namespace vault
{

crypto::ByteBuffer Vault::serialise() const
{
    crypto::ByteBuffer out;

    auto append_u32 = [&out](uint32_t v)
    {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(&v);
        out.insert(out.end(), p, p + sizeof(v));
    };

    auto append_string = [&out, &append_u32](const std::string& s)
    {
        append_u32(static_cast<uint32_t>(s.size()));
        out.insert(out.end(), s.begin(), s.end());
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
        Entry e;

        if (!read_string(data, offset, e.name) ||
            !read_string(data, offset, e.username) ||
            !read_string(data, offset, e.secret))
        {
            return VaultFileError::InvalidFormat;
        }

        vault.add_entry(std::move(e));
    }

    // Extra trailing garbage = corruption
    if (offset != data.size())
    {
        return VaultFileError::InvalidFormat;
    }

    return vault;
}

}
