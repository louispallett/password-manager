#include "util/Expected.h"
#include "vault/VaultFileError.h"
#include "vault/VaultSession.h"

namespace vault 
{

util::Expected<void, VaultFileError> VaultSession::save()
{
    return {};
}

}
