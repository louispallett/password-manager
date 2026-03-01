#include <doctest/doctest.h>
#include <filesystem>
#include <fstream>
#include <sodium.h>

#include "util/Expected.h"
#include "util/SecureString.h"
#include "vault/VaultFile.h"
#include "vault/VaultFileError.h"
#include "VaultTestFixture.h"
#include "vault/VaultSession.h"

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

        // Corrupt first byte of payload
        file.seekg(vault::VAULT_HEADER_SIZE);   // Skip header bytes
        char byte;
        file.read(&byte, 1);
        file.seekp(vault::VAULT_HEADER_SIZE);
        byte ^= 0xFF;                           // Flip bits
        file.write(&byte, 1);
    }

    auto result = vault::VaultFile::load(fixture.file_path, fixture.password);

    CHECK_FALSE(result);
}
