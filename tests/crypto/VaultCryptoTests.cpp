#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <sodium/crypto_aead_xchacha20poly1305.h>

#include "crypto/VaultCrypto.h"
#include "crypto/CryptoConstants.h"
#include "crypto/CryptoTypes.h"
#include "util/SecureString.h"


static crypto::ByteBuffer fixed_salt() 
{
    return crypto::ByteBuffer(crypto::SALT_SIZE, 0x42);
}

static crypto::ByteBuffer fixed_nonce() 
{
    return crypto::ByteBuffer(crypto::NONCE_SIZE, 0x24);
}

// --- Test 1: Key derivation determinism ---
// This tells us that derive_key exists and it returns Expected<ByteBuffer, CryptoError>
TEST_CASE("Key derivation is deterministic for same password and salt") 
{
    util::SecureString password("correct horse battery staple");
    crypto::ByteBuffer salt = fixed_salt();

    auto key1 = crypto::VaultCrypto::derive_key(password, salt);
    auto key2 = crypto::VaultCrypto::derive_key(password, salt);

    CHECK(key1);
    CHECK(key2);
    CHECK(key1.value() == key2.value());
}

// --- Test 2: Wrong password != same key
TEST_CASE("Different passwords produce different keys") 
{
    util::SecureString p1("password1");
    util::SecureString p2("password2");
    crypto::ByteBuffer salt = fixed_salt();

    auto k1 = crypto::VaultCrypto::derive_key(p1, salt);
    auto k2 = crypto::VaultCrypto::derive_key(p2, salt);

    CHECK(k1);
    CHECK(k2);
    CHECK(k1.value() != k2.value());
}

// --- Test 3: encrypt -> decrypt round-trip
TEST_CASE("Encrypt then decrypt returns original plaintext") 
{
    util::SecureString password("secret");
    crypto::ByteBuffer salt = fixed_salt();
    crypto::ByteBuffer plaintext = {'h', 'e', 'l', 'l', 'o'};

    auto key = crypto::VaultCrypto::derive_key(password, salt);
    REQUIRE(key);

    auto encrypted = crypto::VaultCrypto::encrypt(key.value(), plaintext);
    REQUIRE(encrypted);

    // FIXME: We can remove this once we have done `decrypt` - this function should check this anyway!
    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    REQUIRE(encrypted->size() > NONCE_SIZE);

    auto decrypted = crypto::VaultCrypto::decrypt(key.value(), encrypted.value());
    REQUIRE(decrypted);

    CHECK(decrypted.value() == plaintext);
}

// --- Test 4: Wrong key fails decryption
TEST_CASE("Decrypting with wrong key fails") 
{
    crypto::ByteBuffer correct_key(32, 0x01);   // 32-byte key
    crypto::ByteBuffer wrong_key(32, 0x02);     // different key
    crypto::ByteBuffer plaintext = {'s', 'e', 'c', 'r', 'e', 't'};

    auto encrypted = crypto::VaultCrypto::encrypt(correct_key, plaintext);
    REQUIRE(encrypted);

    auto decrypted = crypto::VaultCrypto::decrypt(wrong_key, encrypted.value());
    CHECK_FALSE(decrypted);
}

// Test 5: Tampering detection
TEST_CASE("Tampered ciphertext fails authentication") 
{
    crypto::ByteBuffer key(32, 0x01);
    crypto::ByteBuffer plaintext = {'s', 'e', 'c', 'r', 'e', 't'};

    auto encrypted = crypto::VaultCrypto::encrypt(key, plaintext);
    REQUIRE(encrypted);

    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    // Flip one bit in the ciphertext
    encrypted.value()[NONCE_SIZE] ^= 0xFF;

    auto decrypted = crypto::VaultCrypto::decrypt(key, encrypted.value());

    CHECK_FALSE(decrypted);
}
