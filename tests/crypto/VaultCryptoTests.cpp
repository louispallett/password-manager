#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>


static crypto::ByteBuffer fixed_salt() {
    return crypto::ByteBuffer(crypto::SALT_SIZE, 0x42);
}

static crypto::ByteBuffer fixed_nonce() {
    return crypto::ByteBuffer(crypto::NONCE_SIZE, 0x24);
}

// --- Test 1: Key derivation determinism ---
// This tells us that derive_key exists and it returns Expected<ByteBuffer, CryptoError>
TEST_CASE("Key derivation is deterministic for same password and salt") 
{
    SecureString password("correct horse battery staple");
    ByteBuffer salt = fixed_salt();

    auto key1 = VaultCrypto::derive_key(password, salt);
    auto key2 = VaultCrypto::derive_key(password, salt);

    CHECK(key1);
    CHECK(key2);
    CHECK(key1.value() == key2.value());
}

// --- Test 2: Wrong password != same key
TEST_CASE("Different passwords produce different keys") 
{
    SecureString p1("password1");
    SecureString p2("password2");
    ByteBuffer salt = fixed_salt();

    auto k1 = VaultCrypto::derive_key(p1, salt);
    auto k2 = VaultCrypto::derive_key(p2, salt);

    CHECK(k1);
    CHECK(k2);
    CHECK(k1.value() != k2.value());
}

// --- Test 3: encrypt -> decrypt round-trip
TEST_CASE("Encrypt then decrypt returns original plaintext") 
{
   SecureString password("secret");
   ByteBuffer salt = fixed_salt();
   ByteBuffer nonce = fixed_nonce();
   ByteBuffer plaintext = {'h', 'e', 'l', 'l', 'o'};

   auto key = VaultCrypto::derive_key(password, salt);
   REQUIRE(key);

    auto encrypted = VaultCrypto::encrypt(key.value(), plaintext, nonce);
    REQUIRE(encrypted);

    auto decrypted = VaultCrypto::decrypt(key.value(), encrypted.value(), nonce);
    REQUIRE(decrypted);

    CHECK(decrypted.value() == plaintext);
}

// --- Test 4: Wrong key fails decryption
TEST_CASE("Decrypting with wrong key fails") 
{
    ByteBuffer correct_key(32, 0x01);   // 32-byte key
    ByteBuffer wrong_key(32, 0x02);     // different key
    ByteBuffer nonce(24, 0x00);         // libsodium XChaCha nonce
    ByteBuffer plaintext = {'s', 'e', 'c', 'r', 'e', 't'};

    auto encrypted = VaultCrypto::encrypt(correct_key, plaintext, nonce);
    REQUIRE(encrypted);

    auto decrypted = VaultCrypto::decrypt(wrong_key, encrypted.value(), nonce);
    CHECK_FALSE(decrypted);
}

// Test 5: Tampering detection
TEST_CASE("Tampered ciphertext fails authentication") 
{
    ByteBuffer key(32, 0x01);
    ByteBuffer nonce(24, 0x00);
    ByteBuffer plaintext = {'s', 'e', 'c', 'r', 'e', 't'};

    auto encrypted = VaultCrypto::encrypt(key, plaintext, nonce);
    REQUIRE(encrypted);

    // Flip one bit in the ciphertext
    encrypted.value()[0] ^= 0xFF;

    auto decrypted = VaultCrypto::decrypt(key, encrypted.value(), nonce);

    CHECK_FALSE(decrypted);
}
