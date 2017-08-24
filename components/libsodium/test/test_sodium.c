#include "unity.h"

/* Note: a lot of these libsodium test programs assert() things, but they're not complete unit tests - most expect
   output to be compared to the matching .exp file.

   We don't do this automatically yet, maybe once we have more options for
   internal filesystem storage.
*/

extern int aead_chacha20poly1305_xmain();

TEST_CASE("aead_chacha20poly1305 test vectors", "[libsodium]")
{
    printf("Running aead_chacha20poly1305\n");
    TEST_ASSERT_EQUAL(0, aead_chacha20poly1305_xmain());
}

extern int chacha20_xmain();

TEST_CASE("chacha20 test vectors", "[libsodium]")
{
    printf("Running chacha20\n");
    TEST_ASSERT_EQUAL(0, chacha20_xmain());
}

extern int box_xmain();
extern int box2_xmain();

TEST_CASE("box tests", "[libsodium]")
{
    printf("Running box\n");
    TEST_ASSERT_EQUAL(0, box_xmain());

    printf("Running box2\n");
    TEST_ASSERT_EQUAL(0, box2_xmain());
}

extern int ed25519_convert_xmain();

TEST_CASE("ed25519_convert tests", "[libsodium]")
{
    printf("Running ed25519_convert\n");
    TEST_ASSERT_EQUAL(0, ed25519_convert_xmain() );
}

extern int sign_xmain();

TEST_CASE("sign tests", "[libsodium]")
{
    printf("Running sign\n");
    TEST_ASSERT_EQUAL(0, sign_xmain() );
}

extern int hash_xmain();

TEST_CASE("hash tests", "[libsodium]")
{
    printf("Running hash\n");
    TEST_ASSERT_EQUAL(0, hash_xmain() );
}


