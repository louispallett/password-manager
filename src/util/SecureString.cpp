#include "util/SecureString.h"

#include <cstring>   // std::strlen, std::memcpy
#include <utility>   // std::move
#include <cstddef>   // std::size_t
#include <algorithm> // std::fill

namespace util {

// --- Constructor from C-string ---
SecureString::SecureString(const char* str)
{
    if (str) {
        std::size_t len = std::strlen(str);
        buffer_.resize(len);
        std::memcpy(buffer_.data(), str, len);
    }
}

// --- Constructor from string_view ---
SecureString::SecureString(std::string_view str)
{
    buffer_.assign(str.begin(), str.end());
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

// --- Destructor ---
SecureString::~SecureString()
{
    std::fill(buffer_.begin(), buffer_.end(), 0);
}

// --- Accessors ---
const std::uint8_t* SecureString::data() const noexcept
{
    return buffer_.data();
}

std::size_t SecureString::size() const noexcept
{
    return buffer_.size();
}

} // namespace util
