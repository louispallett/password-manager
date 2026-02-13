#include <doctest/doctest.h>
#include <sodium.h>

#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/Entry.h"
#include "vault/VaultFile.h"
#include "VaultTestFixture.h"

TEST_CASE("Returns all entries")
{
    VaultTestFixture fixture;
    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(loaded);

    vault::Entry entry1 
    {
        util::SecureString{"Email"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld123!"}
    };

    vault::Entry entry2 
    {
        util::SecureString{"Froogle"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld1234!"}
    };

    loaded.value().add_entry(std::move(entry1));
    loaded.value().add_entry(std::move(entry2));

    auto& entries = loaded.value().entries();
    CHECK(entries.size() == 2);
    CHECK(entries[0].name == util::SecureString("Email"));
    CHECK(entries[1].secret == util::SecureString("HelloWorld1234!"));
}

TEST_CASE("Saving and reloading preserves entries")
{
    VaultTestFixture fixture;

    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(loaded);

    vault::Entry expected 
    {
        util::SecureString{"Email"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld123!"}
    };

    vault::Entry entry 
    {
        util::SecureString{"Email"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld123!"}
    };

    loaded.value().add_entry(std::move(entry));

    REQUIRE(vault::VaultFile::save(
        fixture.file_path,
        loaded.value(),
        fixture.password
    ));

    auto reloaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(reloaded);

    const auto& entries = reloaded.value().entries();
    REQUIRE(entries.size() == 1);
    CHECK(entries[0] == expected);
    CHECK(entries[0].name == expected.name);
    CHECK(entries[0].username == expected.username);
    CHECK(entries[0].secret == expected.secret);
}

TEST_CASE("Updates an entry")
{
    VaultTestFixture fixture;
    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(loaded);

    vault::Entry entry 
    {
        util::SecureString{"Email"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld123!"}
    };

    auto& entries = loaded.value().entries();
    loaded.value().add_entry(std::move(entry));
    REQUIRE(entries.size() == 1);

    vault::Entry updated_entry 
    {
        util::SecureString{"Froogle"},
        util::SecureString{"john.doe@example.com"},
        util::SecureString{"HelloWorld1234!"}
    };

    auto result = loaded.value().update_entry(0, std::move(updated_entry));
    REQUIRE(result);

    CHECK(entries[0].name == util::SecureString("Froogle"));
    CHECK(entries[0].username == util::SecureString("john.doe@example.com"));
    CHECK(entries[0].secret == util::SecureString("HelloWorld1234!"));
}

// TEST_CASE("Deletes an entry")
// {
//     VaultTestFixture fixture;
//     REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

//     auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
//     REQUIRE(loaded);

//     vault::Entry entry 
//     {
//         util::SecureString{"Email"},
//         util::SecureString{"john.doe@example.com"},
//         util::SecureString{"HelloWorld123!"}
//     };

//     auto& entries = loaded.value().entries();
//     loaded.value().add_entry(std::move(entry));
//     REQUIRE(entries.size() == 1);

//     auto result = loaded.value().remove_entry(0);
//     REQUIRE(result);

// };
