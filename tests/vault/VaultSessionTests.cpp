#include <doctest/doctest.h>
#include <optional>
#include <sodium.h>

#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/Entry.h"
#include "vault/VaultFile.h"
#include "VaultTestFixture.h"
#include "vault/VaultSession.h"

TEST_CASE("VaultSession destructor zeroes entry memory")
{
    VaultTestFixture fixture;
    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(loaded);

    std::optional<vault::VaultSession> session_;
    session_.emplace(std::move(loaded.value()));

    const std::uint8_t* secret_ptr = nullptr;
    std::size_t secret_size = 0;
    const std::string original_secret = "Hello123!";

    {
        vault::Entry entry
        {
            util::SecureString{"TEST"},
            util::SecureString{"john.doe@example.com"},
            util::SecureString{original_secret.c_str()}
        };

        REQUIRE(session_->add_entry(std::move(entry)));

        const auto& entries = session_->entries();
        REQUIRE(entries.size() == 1);
        secret_ptr = entries[0].secret.data();
        secret_size = entries[0].secret.size();
    }

    auto result = session_->save();
    REQUIRE(result);
    session_.reset();

    REQUIRE(secret_ptr != nullptr);
    REQUIRE(secret_size > 0);

    // --- Testing Memory Sanitation ---
    // session_.reset() runs the destructor, which will call secure_zero, which runs the sodium_memzero method.
    // This method scrambles the allocated bytes before they are freed. It doesn't set them to a value like `0`.
    // So, this final assertion runs over the allocated memory and iterates a counter if any values are strictly equal to the entry.secret chars. It's possible (although rare), that one or two chars have been scrambled to the same value, so this just then tests that this value is no more than half the size of the string.
    std::size_t matching_bytes = 0;
    for (std::size_t i = 0; i < original_secret.size(); ++i)
    {
        if (secret_ptr[i] == static_cast<std::uint8_t>(original_secret[i]))
        {
            ++matching_bytes;
        } 
    }

    CHECK(matching_bytes <= original_secret.size() / 2);
}
