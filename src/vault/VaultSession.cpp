#include "crypto/CryptoContext.h"
#include "util/Expected.h"
#include "vault/VaultError.h"
#include "vault/VaultFile.h"
#include <sys/types.h>
#include <utility>
#include "vault/VaultSession.h"

namespace vault 
{

VaultSession::~VaultSession()
{
    crypto::CryptoContext::secure_zero(key_);
}

bool VaultSession::is_empty() const
{
    return vault_.entries().empty();
}

const std::vector<Entry>& VaultSession::entries () const noexcept
{
    return vault_.entries();
}

util::Expected<void, VaultError> VaultSession::add_entry (Entry entry)
{
    return vault_.add_entry(std::move(entry));
}

util::Expected<void, VaultError> VaultSession::remove_entry(size_t index)
{
    return vault_.remove_entry(index);
}

util::Expected<void, VaultFileError> VaultSession::save()
{
    return vault::VaultFile::save(path_, vault_, key_);
}

}
