#pragma once

#include <cstddef>

namespace crypto {

constexpr std::size_t KEY_SIZE = 32;
constexpr std::size_t NONCE_SIZE = 24;
constexpr std::size_t SALT_SIZE = 16; // Argon2 recommended min
} // namespace crypto
