#pragma once

#include <cstddef>
#include <cstdint>
#include <sodium.h>

namespace crypto 
{

constexpr std::size_t KEY_SIZE = crypto_box_SEEDBYTES;
constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
constexpr std::size_t SALT_SIZE = crypto_pwhash_SALTBYTES;
constexpr std::size_t TAG_SIZE = crypto_aead_xchacha20poly1305_ietf_ABYTES;

// --- Argon2id parameters (interactive / vault-grade) ---
constexpr uint32_t ARGON_MEM_KIB = crypto_pwhash_MEMLIMIT_INTERACTIVE / 1024;
constexpr uint32_t ARGON_ITERS = crypto_pwhash_OPSLIMIT_INTERACTIVE;
constexpr uint32_t ARGON_PARALLELISM = 1;

} // namespace crypto
