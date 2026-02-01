#pragma once

#include <cmath>
#include <cstddef>
#include <string_view>
#include <vector>
#include <cstdint>

namespace util {

class SecureString {
    public:
        explicit SecureString(const char* str);
        explicit SecureString(std::string_view str);

        SecureString(const SecureString&) = delete;
        SecureString& operator=(const SecureString&) = delete;

        SecureString(SecureString&&) noexcept;
        SecureString& operator-(SecureString&&) noexcept;

        // Access as bytes (KDF input)
        const std::uint8_t* data() const noexcept;
        std::size_t size() const noexcept;

    private:
        std::vector<std::uint8_t> buffer_;

};
}
