#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <algorithm>
#include "crypto/CryptoContext.h"

// --- GROUP 1: Initialisation ---
// Test 1: Crypto initialises successfully
TEST_CASE("CryptoContext initialises successfully")
{
    CHECK(crypto::CryptoContext::init());
}

// Test 2: Initialisation is idempotent
TEST_CASE("CryptoContext initialisation is idempotent")
{
    REQUIRE(crypto::CryptoContext::init());
    CHECK(crypto::CryptoContext::init());
}

// --- GROUP 3: Secure Radnomness ---
// Test 3: random_bytes fills buffer
TEST_CASE("CryptoContext generates random bytes")
{
    REQUIRE(crypto::CryptoContext::init());

    crypto::ByteBuffer buf(32, 0);
    crypto::CryptoContext::random_bytes(buf);

    CHECK(std::any_of(buf.begin(), buf.end(), [](unsigned char b) 
    { 
        return b != 0; 
	}));
}

// --- GROUP 4: Secure Memory Wiping ---
// Test 4: secure_zero wipes memory
TEST_CASE("CryptoContext securely wipes memory")
{
   	crypto::ByteBuffer buf(32, 0xAA);
	
	crypto::CryptoContext::secure_zero(buf);
	
	CHECK(std::all_of(buf.begin(), buf.end(), [](unsigned char b) 
	{
		return b == 0;
	}));
}
