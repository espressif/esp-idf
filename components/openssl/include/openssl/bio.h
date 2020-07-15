// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _OPENSSL_BIO_H
#define _OPENSSL_BIO_H

#include <stddef.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* These are the 'types' of BIOs */
#define BIO_TYPE_NONE 0
#define BIO_TYPE_MEM (1 | 0x0400)
#define BIO_TYPE_BIO (19 | 0x0400)        /* (half a) BIO pair */

/* Bio object flags */
#define BIO_FLAGS_READ		0x01
#define BIO_FLAGS_WRITE		0x02

#define BIO_should_read(a)		BIO_test_flags(a, BIO_FLAGS_READ)
#define BIO_should_write(a)		BIO_test_flags(a, BIO_FLAGS_WRITE)

typedef struct bio_st BIO;
typedef struct bio_method_st BIO_METHOD;

/**
 * @brief Create a BIO object as a file type
 * Current implementation return NULL as file types are discouraged on ESP platform
 *
 * @param filename Filename
 * @param mode Mode
 *
 * @return BIO object
 */
BIO *BIO_new_file(const char *filename, const char *mode);

/**
 * @brief Create a BIO object as a membuf type
 * Current implementation takes a shallow copy of the buffer
 *
 * @param buf Pointer to the buffer
 * @param len Length of the buffer
 *
 * @return BIO object
 */
BIO *BIO_new_mem_buf(void *buf, int len);

/**
 * @brief create a BIO object
 *
 * @param method - pointer to BIO_METHOD
 *
 * @return pointer to BIO object
 */
BIO *BIO_new(BIO_METHOD * method);

/**
 * @brief get the memory BIO method function
 */
void *BIO_s_mem(void);

/**
 * @brief free a BIO  object
 *
 * @param x - pointer to BIO object
 */
void BIO_free(BIO *b);

/**
 * @brief Create a connected pair of BIOs bio1, bio2 with write buffer sizes writebuf1 and writebuf2
 *
 * @param out1    pointer to BIO1
 * @param writebuf1    write size of BIO1 (0 means default size will be used)
 * @param out2    pointer to BIO2
 * @param writebuf2    write size of BIO2 (0 means default size will be used)
 *
 * @return result
 *      0 : failed
 *      1 : OK
 */
int BIO_new_bio_pair(BIO **out1, size_t writebuf1, BIO **out2, size_t writebuf2);

/**
 * @brief Write data to BIO
 *
 * BIO_TYPE_BIO behaves the same way as OpenSSL bio object, other BIO types mock
 * this functionality to avoid excessive allocation/copy, so the 'data' cannot
 * be freed after the function is called, it should remain valid until BIO object is in use.
 *
 * @param b    - pointer to BIO
 * @param data - pointer to data
 * @param dlen - data bytes
 *
 * @return result
 *      -1, 0 : failed
 *      1 : OK
 */
int BIO_write(BIO *b, const void *data, int dlen);

/**
 * @brief Read data from BIO
 *
 * BIO_TYPE_BIO behaves the same way as OpenSSL bio object.
 * Other types just hold pointer
 *
 * @param b    - pointer to BIO
 * @param data - pointer to data
 * @param dlen - data bytes
 *
 * @return result
 *      -1, 0 : failed
 *      1 : OK
 */
int BIO_read(BIO *bio, void *data, int len);

/**
 * @brief Get number of pending characters in the BIOs write buffers.
 *
 * @param b Pointer to BIO
 *
 * @return Amount of pending data
 */
size_t BIO_wpending(const BIO *bio);

/**
 * @brief Get number of pending characters in the BIOs read buffers.
 *
 * @param b Pointer to BIO
 *
 * @return Amount of pending data
 */
size_t BIO_ctrl_pending(const BIO *bio);

/**
 * @brief Get the maximum length of data that can be currently written to the BIO
 *
 * @param b Pointer to BIO
 *
 * @return Max length of writable data
 */
size_t BIO_ctrl_get_write_guarantee(BIO *bio);

/**
 * @brief Returns the type of a BIO.
 *
 * @param b Pointer to BIO
 *
 * @return Type of the BIO object
 */
int BIO_method_type(const BIO *b);

/**
 * @brief Test flags of a BIO.
 *
 * @param b Pointer to BIO
 * @param flags Flags
 *
 * @return BIO object flags masked with the supplied flags
 */
int  BIO_test_flags(const BIO *b, int flags);

#ifdef __cplusplus
}
#endif

#endif //_OPENSSL_BIO_H
