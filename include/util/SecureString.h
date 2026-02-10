#pragma once

#include <cstddef>
#include <cstring>
#include <sodium/utils.h>
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
        SecureString& operator=(SecureString&&) noexcept;

        ~SecureString();

        // Access as bytes (KDF input)
        const std::uint8_t* data() const noexcept;
        std::size_t size() const noexcept;
        bool operator==(const SecureString& other) const noexcept
        {
            return size() == other.size() && std::memcmp(data(), other.data(), size()) == 0;
        }

        void assign(const char* data, std::size_t size);

    private:
        std::vector<std::uint8_t> buffer_;

};
}
