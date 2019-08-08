#include "unity.h"
#include "sodium/crypto_hash_sha256.h"
#include "sodium/crypto_hash_sha512.h"

/* Note: a lot of these libsodium test programs assert() things, but they're not complete unit tests - most expect
   output to be compared to the matching .exp file.

   We don't do this automatically yet, maybe once we have more options for
   internal filesystem storage.
*/

extern int aead_chacha20poly1305_xmain(void);

TEST_CASE("aead_chacha20poly1305 test vectors", "[libsodium]")
{
    printf("Running aead_chacha20poly1305\n");
    TEST_ASSERT_EQUAL(0, aead_chacha20poly1305_xmain());
}

extern int chacha20_xmain(void);

TEST_CASE("chacha20 test vectors", "[libsodium]")
{
    printf("Running chacha20\n");
    TEST_ASSERT_EQUAL(0, chacha20_xmain());
}

extern int box_xmain(void);
extern int box2_xmain(void);

TEST_CASE("box tests", "[libsodium]")
{
    printf("Running box\n");
    TEST_ASSERT_EQUAL(0, box_xmain());

    printf("Running box2\n");
    TEST_ASSERT_EQUAL(0, box2_xmain());
}

extern int ed25519_convert_xmain(void);

TEST_CASE("ed25519_convert tests", "[libsodium][timeout=60]")
{
    printf("Running ed25519_convert\n");
    TEST_ASSERT_EQUAL(0, ed25519_convert_xmain() );
}

extern int sign_xmain(void);

TEST_CASE("sign tests", "[libsodium]")
{
    printf("Running sign\n");
    TEST_ASSERT_EQUAL(0, sign_xmain() );
}

extern int hash_xmain(void);

TEST_CASE("hash tests", "[libsodium]")
{
    printf("Running hash\n");
    TEST_ASSERT_EQUAL(0, hash_xmain() );
}

TEST_CASE("sha256 sanity check", "[libsodium]")
{
    const uint8_t expected[] = { 0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41,
                                 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23, 0xb0, 0x03,
                                 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff,
                                 0x61, 0xf2, 0x00, 0x15, 0xad, };
    uint8_t calculated[32];
    crypto_hash_sha256_state state;

    const uint8_t *in = (const uint8_t *)"abc";
    const size_t inlen = 3;

    // One-liner version
    crypto_hash_sha256(calculated, in, inlen);
    TEST_ASSERT_EQUAL(sizeof(calculated), sizeof(expected));
    TEST_ASSERT_EQUAL(sizeof(calculated), crypto_hash_sha256_bytes());
    TEST_ASSERT_EQUAL_MEMORY(expected, calculated, crypto_hash_sha256_bytes());

    // Multi-line version
    crypto_hash_sha256_init(&state);
    crypto_hash_sha256_update(&state, in, inlen - 1); // split into two updates
    crypto_hash_sha256_update(&state, in + (inlen -1), 1);
    crypto_hash_sha256_final(&state, calculated);
    TEST_ASSERT_EQUAL_MEMORY(expected, calculated, crypto_hash_sha256_bytes());
}

TEST_CASE("sha512 sanity check", "[libsodium]")
{
    const uint8_t expected[] = { 0xdd, 0xaf, 0x35, 0xa1, 0x93, 0x61, 0x7a, 0xba, 0xcc,
                                 0x41, 0x73, 0x49, 0xae, 0x20, 0x41, 0x31, 0x12, 0xe6,
                                 0xfa, 0x4e, 0x89, 0xa9, 0x7e, 0xa2, 0x0a, 0x9e, 0xee,
                                 0xe6, 0x4b, 0x55, 0xd3, 0x9a, 0x21, 0x92, 0x99, 0x2a,
                                 0x27, 0x4f, 0xc1, 0xa8, 0x36, 0xba, 0x3c, 0x23, 0xa3,
                                 0xfe, 0xeb, 0xbd, 0x45, 0x4d, 0x44, 0x23, 0x64, 0x3c,
                                 0xe8, 0x0e, 0x2a, 0x9a, 0xc9, 0x4f, 0xa5, 0x4c, 0xa4,
                                 0x9f };

    uint8_t calculated[64];
    crypto_hash_sha512_state state;

    const uint8_t *in = (const uint8_t *)"abc";
    const size_t inlen = 3;

    // One-liner version
    crypto_hash_sha512(calculated, in, inlen);
    TEST_ASSERT_EQUAL(sizeof(calculated), sizeof(expected));
    TEST_ASSERT_EQUAL(sizeof(calculated), crypto_hash_sha512_bytes());
    TEST_ASSERT_EQUAL_MEMORY(expected, calculated, crypto_hash_sha512_bytes());

    // Multi-line version
    crypto_hash_sha512_init(&state);
    crypto_hash_sha512_update(&state, in, inlen - 1); // split into two updates
    crypto_hash_sha512_update(&state, in + (inlen -1), 1);
    crypto_hash_sha512_final(&state, calculated);
    TEST_ASSERT_EQUAL_MEMORY(expected, calculated, crypto_hash_sha512_bytes());
}
