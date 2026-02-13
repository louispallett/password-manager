#include <filesystem>
#include <sodium.h>

#include "util/SecureString.h"

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
