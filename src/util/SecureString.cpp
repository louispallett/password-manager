#include "util/SecureString.h"

#include <cstring>   // std::strlen, std::memcpy
#include <sodium/utils.h>
#include <utility>   // std::move
#include <cstddef>   // std::size_t
#include <algorithm> // std::fill

namespace util {

// --- Constructor from C-string ---
SecureString::SecureString(const char* str)
{
    if (str) {
        std::size_t len = std::strlen(str);
        buffer_.resize(len + 1);
        std::memcpy(buffer_.data(), str, len);
        buffer_[len] = '\0';
    }
}

// --- Constructor from string_view ---
SecureString::SecureString(std::string_view str)
{
    buffer_.resize(str.size() + 1);
    std::memcpy(buffer_.data(), str.data(), str.size());
    buffer_[str.size()] = '\0';
}

// --- Move constructor ---
SecureString::SecureString(SecureString&& other) noexcept
    : buffer_(std::move(other.buffer_))
{
    // Zero out the source to avoid leaving sensitive data
    std::fill(other.buffer_.begin(), other.buffer_.end(), 0);
}

// --- Move assignment ---
SecureString& SecureString::operator=(SecureString&& other) noexcept
{
    if (this != &other) {
        // Zero out existing buffer
        std::fill(buffer_.begin(), buffer_.end(), 0);
        buffer_ = std::move(other.buffer_);
        std::fill(other.buffer_.begin(), other.buffer_.end(), 0);
    }
    return *this;
}

void SecureString::assign(const char* data, std::size_t size)
{
    std::fill(buffer_.begin(), buffer_.end(), 0);
    if (!data || size == 0) 
    { 
      buffer_.clear(); return; 
    }

    buffer_.resize(size + 1);
    std::memcpy(buffer_.data(), data, size);
    buffer_[size] = '\0';
}

// --- Destructor ---
SecureString::~SecureString()
{
    sodium_memzero(buffer_.data(), buffer_.size()); 
}

// --- Accessors ---
const std::uint8_t* SecureString::data() const noexcept
{
    return buffer_.data();
}

std::size_t SecureString::size() const noexcept
{
    return buffer_.empty() ? 0 : buffer_.size() - 1;
}

const char* SecureString::c_str() const noexcept
{
  return reinterpret_cast<const char*>(buffer_.data());
}

} // namespace util
