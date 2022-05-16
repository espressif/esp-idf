/* Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#define ESP_OPENSSL_SUPPRESS_LEGACY_WARNING
#include "openssl/ssl.h"
#include "unity.h"

/**
 * @brief This simple test suite is taken from OpenSSL err_test.cc and bio_test.cc, the relevant test
 * cases were adopted to the supported fraction of OpenSSL port in esp-idf
 */

//
// Basic error stack support and test
//
#define ERR_NUM_ERRORS 4

TEST_CASE("ErrTest, Overflow", "[openssl]")
{

    for (unsigned i = 0; i < ERR_NUM_ERRORS*2; i++) {
        ERR_put_error(1, 0 /* unused */, i+1, "test", 1);
    }

    for (unsigned i = 0; i < ERR_NUM_ERRORS - 1; i++) {
        uint32_t err = ERR_get_error();
        /* Errors are returned in order they were pushed, with the least recent ones
         * removed, up to |ERR_NUM_ERRORS - 1| errors. So the errors returned are
         * |ERR_NUM_ERRORS + 2| through |ERR_NUM_ERRORS * 2|, inclusive. */
        TEST_ASSERT_NOT_EQUAL(0u, err);
        TEST_ASSERT_EQUAL(i + ERR_NUM_ERRORS + 2, ERR_GET_REASON(err));
    }

    TEST_ASSERT_EQUAL(0u, ERR_get_error());
}

TEST_CASE("ErrTest, PutError", "[openssl]")
{
    TEST_ASSERT_EQUAL(0u, ERR_get_error()); // ERR_get_error returned value before an error was added.

    ERR_put_error(1, 0 /* unused */, 2, "test", 4);

    int peeked_line, line, peeked_flags, flags;
    const char *peeked_file, *file, *peeked_data, *data;
    uint32_t peeked_packed_error =
            ERR_peek_error_line_data(&peeked_file, &peeked_line, &peeked_data,
                                     &peeked_flags);
    uint32_t packed_error = ERR_get_error_line_data(&file, &line, &data, &flags);

    TEST_ASSERT_EQUAL(peeked_packed_error, packed_error);
    TEST_ASSERT_EQUAL(peeked_file, file);

    TEST_ASSERT_EQUAL_STRING("test", file);
    TEST_ASSERT_EQUAL(4, line);
    TEST_ASSERT_EQUAL(1, ERR_GET_LIB(packed_error));
    TEST_ASSERT_EQUAL(2, ERR_GET_REASON(packed_error));
}

TEST_CASE("ErrTest, ClearError", "[openssl]")
{
    TEST_ASSERT_EQUAL(0u, ERR_get_error()); // ERR_get_error returned value before an error was added.

    ERR_put_error(1, 0 /* unused */, 2, "test", 4);
    ERR_clear_error();

    // The error queue should be cleared.
    TEST_ASSERT_EQUAL(0u, ERR_get_error());
}

//
// Simplified BIO support and check
//
TEST_CASE("BioTest, TestPair", "[openssl]")
{
    BIO *bio1, *bio2;
    TEST_ASSERT_NOT_EQUAL(0, BIO_new_bio_pair(&bio1, 10, &bio2, 10));
    TEST_ASSERT_EQUAL(BIO_ctrl_get_write_guarantee(bio1), 10);

    // Data written in one end may be read out the other.
    char buf[20];
    TEST_ASSERT_EQUAL(5, BIO_write(bio1, "12345", 5));
    TEST_ASSERT_EQUAL(5, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(5, BIO_read(bio2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("12345", buf, 5);
    TEST_ASSERT_EQUAL(10, BIO_ctrl_get_write_guarantee(bio1));

    // Attempting to write more than 10 bytes will write partially.
    TEST_ASSERT_EQUAL(10, BIO_write(bio1, "1234567890___", 13));
    TEST_ASSERT_EQUAL(0, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(-1, BIO_write(bio1, "z", 1));
    TEST_ASSERT_TRUE(BIO_should_write(bio1));
    TEST_ASSERT_EQUAL(10, BIO_read(bio2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("1234567890", buf, 10);
    TEST_ASSERT_EQUAL(10, BIO_ctrl_get_write_guarantee(bio1));

    // Unsuccessful reads update the read request.
    TEST_ASSERT_EQUAL(-1, BIO_read(bio2, buf, 5));
    TEST_ASSERT_TRUE(BIO_should_read(bio2));

    // The read request is clamped to the size of the buffer.
    TEST_ASSERT_EQUAL(-1, BIO_read(bio2, buf, 20));
    TEST_ASSERT_TRUE(BIO_should_read(bio2));

    // Data may be written and read in chunks.
    TEST_ASSERT_EQUAL(BIO_write(bio1, "12345", 5), 5);
    TEST_ASSERT_EQUAL(5, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(5, BIO_write(bio1, "67890___", 8));
    TEST_ASSERT_EQUAL(0, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(3, BIO_read(bio2, buf, 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("123", buf, 3);
    TEST_ASSERT_EQUAL(3, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(7, BIO_read(bio2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("4567890", buf, 7);
    TEST_ASSERT_EQUAL(10, BIO_ctrl_get_write_guarantee(bio1));

    // Test writes and reads starting in the middle of the ring buffer and
    // wrapping to front.
    TEST_ASSERT_EQUAL(8, BIO_write(bio1, "abcdefgh", 8));
    TEST_ASSERT_EQUAL(2, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(3, BIO_read(bio2, buf, 3));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("abc", buf, 3);
    TEST_ASSERT_EQUAL(5, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(5, BIO_write(bio1, "ijklm___", 8));
    TEST_ASSERT_EQUAL(0, BIO_ctrl_get_write_guarantee(bio1));
    TEST_ASSERT_EQUAL(10, BIO_read(bio2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("defghijklm", buf, 10);
    TEST_ASSERT_EQUAL(10, BIO_ctrl_get_write_guarantee(bio1));

    // Data may flow from both ends in parallel.
    TEST_ASSERT_EQUAL(5, BIO_write(bio1, "12345", 5));
    TEST_ASSERT_EQUAL(5, BIO_write(bio2, "67890", 5));
    TEST_ASSERT_EQUAL(5, BIO_read(bio2, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("12345", buf, 5);
    TEST_ASSERT_EQUAL(5, BIO_read(bio1, buf, sizeof(buf)));
    TEST_ASSERT_EQUAL_UINT8_ARRAY("67890", buf, 5);

    // Other tests below not imported since BIO_shutdown_wr() not supported
    // - Closing the write end causes an EOF on the read half, after draining.
    // - A closed write end may not be written to.
    // - The other end is still functional.
}
