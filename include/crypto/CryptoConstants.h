#pragma once

#include <cstddef>
#include <cstdint>
#include <sodium.h>

namespace crypto 
{

constexpr std::size_t KEY_SIZE = 32;
constexpr std::size_t NONCE_SIZE = 24;
constexpr std::size_t SALT_SIZE = 32;

// --- Argon2id parameters (interactive / vault-grade) ---
constexpr uint32_t ARGON_MEM_KIB = crypto_pwhash_MEMLIMIT_INTERACTIVE / 1024;
constexpr uint32_t ARGON_ITERS = crypto_pwhash_OPSLIMIT_INTERACTIVE;
constexpr uint32_t ARGON_PARALLELISM = 1;

} // namespace crypto
