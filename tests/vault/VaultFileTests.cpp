#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include <sodium.h>

#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/VaultFile.h"
#include "vault/VaultFileError.h"

struct VaultTestFixture 
{
    std::filesystem::path file_path;
    util::SecureString password;

    VaultTestFixture()
        : file_path(
            std::filesystem::temp_directory_path() /
            ("vault_test_" + std::to_string(randombytes_random()) + ".vault")
        ),
        password("lFY7WX#y!iy32d3!&IsAf%2s71U@J4j")
    {}

    ~VaultTestFixture() 
    {
        std::error_code ec;
        std::filesystem::remove(file_path, ec);
    }
};

TEST_CASE("Creating a new vault succeeds")
{
    VaultTestFixture fixture;

    auto result = vault::VaultFile::create_new(fixture.file_path, fixture.password);

    REQUIRE(result);
    CHECK(std::filesystem::exists(fixture.file_path));
    CHECK(std::filesystem::is_regular_file(fixture.file_path));
}

TEST_CASE("Creating over an existing file fails")
{
    VaultTestFixture fixture;

    auto result1 = vault::VaultFile::create_new(fixture.file_path, fixture.password);
    REQUIRE(result1);

    auto result2 = vault::VaultFile::create_new(fixture.file_path, fixture.password);
    CHECK_FALSE(result2);
    CHECK(result2.error() == vault::VaultFileError::FileAlreadyExists);
}

TEST_CASE("Loading with correct password succeeds")
{
    VaultTestFixture fixture;
    
    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));
    
    auto result = vault::VaultFile::load(fixture.file_path, fixture.password);
    CHECK(result);
}

TEST_CASE("Loading with wrong password fails")
{
    VaultTestFixture fixture;

    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto result = vault::VaultFile::load(fixture.file_path, util::SecureString("HelloWorld123!"));
    CHECK(result.error() == vault::VaultFileError::CryptoError);
}

TEST_CASE("Saving and reloading preserves entries")
{
    VaultTestFixture fixture;

    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    auto loaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(loaded);

    vault::Entry entry {
        "Email",
        "john.doe@example.com",
        "HelloWorld123!"
    };

    loaded.value().add_entry(entry);

    REQUIRE(vault::VaultFile::save(
        fixture.file_path,
        loaded.value(),
        fixture.password
    ));

    auto reloaded = vault::VaultFile::load(fixture.file_path, fixture.password);
    REQUIRE(reloaded);

    const auto& entries = reloaded.value().entries();
    REQUIRE(entries.size() == 1);
    CHECK(entries[0] == entry);
}

TEST_CASE("Corrupted file fails cleanly")
{
    VaultTestFixture fixture;

    REQUIRE(vault::VaultFile::create_new(fixture.file_path, fixture.password));

    // Corrupt the file
    {
        std::fstream file(
            fixture.file_path,
            std::ios::in | std::ios::out | std::ios::binary
        );
        REQUIRE(file);

        file.seekg(10);          // Skip header bytes
        char byte;
        file.read(&byte, 1);
        file.seekp(10);
        byte ^= 0xFF;            // Flip bits
        file.write(&byte, 1);
    }

    auto result = vault::VaultFile::load(fixture.file_path, fixture.password);

    CHECK_FALSE(result);
    CHECK(
        result.error() == vault::VaultFileError::InvalidFormat ||
        result.error() == vault::VaultFileError::CryptoError
    );
}
