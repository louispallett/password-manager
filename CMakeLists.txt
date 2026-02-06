cmake_minimum_required(VERSION 3.16)

project(vault
    VERSION 0.1.0
    LANGUAGES CXX
)

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(vault_lib
    src/crypto/VaultCrypto.cpp
    src/crypto/CryptoContext.cpp
    src/util/SecureString.cpp
    src/util/FileUtil.cpp
    src/vault/Vault.cpp
    src/vault/VaultFile.cpp
)

target_include_directories(vault_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)

target_link_libraries(vault_lib
    PRIVATE
        ${SODIUM_LIBRARIES}
)

target_include_directories(vault_lib
    PRIVATE
        ${SODIUM_INCLUDE_DIRS}
)

add_executable(vault
    src/main.cpp
)

target_link_libraries(vault
    PRIVATE
        vault_lib
)

target_include_directories(vault
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(FetchContent)

FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)

FetchContent_MakeAvailable(doctest)

enable_testing()
add_subdirectory(tests)
