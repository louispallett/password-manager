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

    const auto& encrypted_vec = encrypted.value();
    REQUIRE(!encrypted_vec.empty());

    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    crypto::ByteBuffer nonce(encrypted_vec.begin(), encrypted_vec.begin() + NONCE_SIZE);
    crypto::ByteBuffer cipher(encrypted_vec.begin() + NONCE_SIZE, encrypted_vec.end());

    auto decrypted = crypto::VaultCrypto::decrypt(key.value(), nonce, cipher);
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

    const auto& encrypted_vec = encrypted.value();
    REQUIRE(!encrypted_vec.empty());

    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    crypto::ByteBuffer nonce(encrypted_vec.begin(), encrypted_vec.begin() + NONCE_SIZE);
    crypto::ByteBuffer cipher(encrypted_vec.begin() + NONCE_SIZE, encrypted_vec.end());

    auto decrypted = crypto::VaultCrypto::decrypt(wrong_key, nonce, cipher);
    CHECK_FALSE(decrypted);
}

// Test 5: Tampering detection
TEST_CASE("Tampered ciphertext fails authentication") 
{
    crypto::ByteBuffer key(32, 0x01);
    crypto::ByteBuffer plaintext = {'s', 'e', 'c', 'r', 'e', 't'};

    auto encrypted = crypto::VaultCrypto::encrypt(key, plaintext);
    REQUIRE(encrypted);

    const auto& encrypted_vec = encrypted.value();
    REQUIRE(!encrypted_vec.empty());

    constexpr std::size_t NONCE_SIZE = crypto_aead_xchacha20poly1305_ietf_NPUBBYTES;
    crypto::ByteBuffer nonce(encrypted_vec.begin(), encrypted_vec.begin() + NONCE_SIZE);
    crypto::ByteBuffer cipher(encrypted_vec.begin() + NONCE_SIZE, encrypted_vec.end());

    // Flip one bit in the ciphertext
    cipher[0] ^= 0xFF;

    auto decrypted = crypto::VaultCrypto::decrypt(key, nonce, cipher);

    CHECK_FALSE(decrypted);
}
