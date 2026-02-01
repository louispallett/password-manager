add_executable(vault_tests
    crypto/VaultCryptoTests.cpp
    util/ExpectedTests.cpp
)

target_link_libraries(vault_tests
    PRIVATE
        doctest::doctest
)

target_include_directories(vault_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

add_test(
    NAME vault_tests
    COMMAND vault_tests
)
