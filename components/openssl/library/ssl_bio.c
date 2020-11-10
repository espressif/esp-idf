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

#include "ssl_lib.h"
#include "openssl/bio.h"
#include "ssl_dbg.h"
#include "openssl/err.h"

#define DEFAULT_BIO_SIZE 1024

BIO *BIO_new_mem_buf(void *buf, int len)
{
    BIO_METHOD m = { .type = BIO_TYPE_MEM, .size = 0 };
    BIO *b = BIO_new(&m);
    if (b) {
        b->dlen = len;
        b->data = buf;
    }
    return b;
}

/**
 * @brief create a BIO object
 */
BIO *BIO_new(BIO_METHOD * method)
{
    BIO *b = (BIO *)ssl_mem_zalloc(sizeof(BIO));
    if (!b) {
        OPENSSL_PUT_LIB_ERROR(ERR_LIB_BIO, ERR_R_MALLOC_FAILURE);
        goto err;
    }
    if (method) {
        b->size = method->size;
        b->type = method->type;
    } else {
        b->type = BIO_TYPE_NONE;
    }
    if ((b->type & BIO_TYPE_BIO) && b->size) {
        b->data = ssl_mem_zalloc(b->size);
        if (!b->data) {
            OPENSSL_PUT_LIB_ERROR(ERR_LIB_BIO, ERR_R_MALLOC_FAILURE);
            goto err;
        }
    }
    return b;

err:
    if (b && (b->type&BIO_TYPE_BIO)) {
        ssl_mem_free(b->data);
    }
    ssl_mem_free(b);
    return NULL;
}

/**
 * @brief free a BIO object
 */
void BIO_free(BIO *b)
{
    if (b && (b->type&BIO_TYPE_BIO)) {
        ssl_mem_free(b->data);
    }
    ssl_mem_free(b);
}

int BIO_new_bio_pair(BIO **out1, size_t writebuf1, BIO **out2, size_t writebuf2)
{
    BIO *bio1 = NULL;
    BIO *bio2 = NULL;
    if (!writebuf1) {
        writebuf1 = DEFAULT_BIO_SIZE;
    }
    if (!writebuf2) {
        writebuf2 = DEFAULT_BIO_SIZE;
    }
    BIO_METHOD m1 = {
            .size = writebuf1,
            .type = BIO_TYPE_BIO,
    };
    BIO_METHOD m2 = {
            .size = writebuf1,
            .type = BIO_TYPE_BIO,
    };
    bio1 = BIO_new(&m1);
    if (!bio1) {
        goto err;
    }
    bio2 = BIO_new(&m2);
    if (!bio2) {
        goto err;
    }
    *out1 = bio1;
    *out2 = bio2;
    bio1->peer = bio2;
    bio1->size = writebuf1;
    bio2->peer = bio1;
    bio2->size = writebuf2;
    return 1;

err:
    if (bio1)
    {
        BIO_free(bio1);
        *out1 = NULL;
    }
    if (bio2)
    {
        BIO_free(bio2);
        *out2 = NULL;
    }
    return 0;

}

/**
 * @brief get the memory BIO method function
 */
void *BIO_s_mem(void)
{
    return NULL;
}

int BIO_method_type(const BIO *b)
{
    SSL_ASSERT1(b);
    return b->type;
}

/**
 * @brief load data into BIO.
 *
 */
int BIO_write(BIO *b, const void * data, int dlen)
{
    SSL_ASSERT1(b);
    int remaining = b->size - b->offset;
    if (remaining <= 0) {
        b->flags |= BIO_FLAGS_WRITE;
        return -1;
    }
    int len_to_write = dlen > remaining?remaining:dlen;
    memcpy(b->data + b->offset, data, len_to_write);
    b->offset += len_to_write;
    b->dlen = b->offset;
    if (len_to_write == dlen) {
        b->flags &= ~BIO_FLAGS_WRITE;
    }
    return len_to_write;
}

/**
 * @brief Read from BIO.
 *
 */
int BIO_read(BIO *bio, void *data, int len)
{
    SSL_ASSERT1(bio);
    BIO *peer = bio->peer;
    int remaining = peer->dlen - peer->roffset;
    if (remaining <= 0) {
        bio->flags |= BIO_FLAGS_READ;
        return -1;
    }
    int len_to_read = remaining > len ? len : remaining;
    memcpy(data, peer->data + peer->roffset, len_to_read);
    peer->roffset += len_to_read;
    if (len_to_read == len) {
        bio->flags &= ~BIO_FLAGS_READ;
    }
    if (peer->offset) {
        // shift data back to the beginning of the buffer
        memmove(peer->data, peer->data+peer->roffset, peer->offset - peer->roffset);
        peer->offset -= peer->roffset;
        peer->roffset = 0;
        peer->dlen = peer->offset;
    }
    return len_to_read;
}

size_t BIO_wpending(const BIO *bio)
{
    return bio->dlen - bio->roffset;
}

size_t BIO_ctrl_pending(const BIO *bio)
{
    return bio->peer->dlen - bio->peer->roffset;
}

size_t BIO_ctrl_get_write_guarantee(BIO *b)
{
    return (long)b->size - b->dlen;
}

int  BIO_test_flags(const BIO *b, int flags)
{
    return (b->flags & flags);
}
