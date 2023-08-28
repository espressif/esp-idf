/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "soc/soc_caps.h"
#include "hal/sha_types.h"

#if SOC_SHA_SUPPORTED

#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}

#define PUT_UINT64_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 56 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 48 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 40 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 32 );       \
    (b)[(i) + 4] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 5] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 6] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 7] = (unsigned char) ( (n)       );       \
}

#define BUFFER_SZ 1030 // NB: not an exact multiple of SHA block size

static inline size_t block_length(esp_sha_type type)
{
    switch (type) {
    case SHA1:
    case SHA2_224:
    case SHA2_256:
        return 64;
#if SOC_SHA_SUPPORT_SHA384
    case SHA2_384:
#endif
#if SOC_SHA_SUPPORT_SHA512
    case SHA2_512:
#endif
#if SOC_SHA_SUPPORT_SHA512_T
    case SHA2_512224:
    case SHA2_512256:
    case SHA2_512T:
#endif
        return 128;
    default:
        return 0;
    }
}

#if defined(SOC_SHA_SUPPORT_SHA1)

static const unsigned char sha1_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[5];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;            /*!< if first then true else false */
} sha1_ctx;

#endif /* defined(SOC_SHA_SUPPORT_SHA1) */


#if defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256)

static const unsigned char sha256_padding[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

typedef struct {
    uint32_t total[2];          /*!< number of bytes processed  */
    uint32_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[64];   /*!< data block being processed */
    int first_block;            /*!< if first then true, else false */
} sha256_ctx;

#endif /* defined(SOC_SHA_SUPPORT_SHA224) || defined(SOC_SHA_SUPPORT_SHA256) */

#if defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512)

static const unsigned char sha512_padding[128] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

typedef struct {
    uint64_t total[2];          /*!< number of bytes processed  */
    uint64_t state[8];          /*!< intermediate digest state  */
    unsigned char buffer[128];  /*!< data block being processed */
    int first_block;
    uint32_t t_val;             /*!< t_val for 512/t mode */
} sha512_ctx;

#if SOC_SHA_SUPPORT_SHA512_T
int sha_512_t_init_hash_block(uint16_t t);

/*
 * FIPS-180-2 test vectors
 */
static const unsigned char sha512T_test_buf[2][113] = {
    { "abc" },
    {
        "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmn"
        "hijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"
    }
};

static const size_t sha512T_test_buflen[2] = {
    3, 112
};

static const esp_sha_type sha512T_algo[4] = {
    SHA2_512224, SHA2_512256, SHA2_512T, SHA2_512T
};

static const size_t sha512T_t_len[4] = { 224, 256, 224, 256 };

static const unsigned char sha512_test_sum[4][32] = {
    /* SHA512-224 */
    {
        0x46, 0x34, 0x27, 0x0f, 0x70, 0x7b, 0x6a, 0x54,
        0xda, 0xae, 0x75, 0x30, 0x46, 0x08, 0x42, 0xe2,
        0x0e, 0x37, 0xed, 0x26, 0x5c, 0xee, 0xe9, 0xa4,
        0x3e, 0x89, 0x24, 0xaa
    },
    {
        0x23, 0xfe, 0xc5, 0xbb, 0x94, 0xd6, 0x0b, 0x23,
        0x30, 0x81, 0x92, 0x64, 0x0b, 0x0c, 0x45, 0x33,
        0x35, 0xd6, 0x64, 0x73, 0x4f, 0xe4, 0x0e, 0x72,
        0x68, 0x67, 0x4a, 0xf9
    },

    /* SHA512-256 */
    {
        0x53, 0x04, 0x8e, 0x26, 0x81, 0x94, 0x1e, 0xf9,
        0x9b, 0x2e, 0x29, 0xb7, 0x6b, 0x4c, 0x7d, 0xab,
        0xe4, 0xc2, 0xd0, 0xc6, 0x34, 0xfc, 0x6d, 0x46,
        0xe0, 0xe2, 0xf1, 0x31, 0x07, 0xe7, 0xaf, 0x23
    },
    {
        0x39, 0x28, 0xe1, 0x84, 0xfb, 0x86, 0x90, 0xf8,
        0x40, 0xda, 0x39, 0x88, 0x12, 0x1d, 0x31, 0xbe,
        0x65, 0xcb, 0x9d, 0x3e, 0xf8, 0x3e, 0xe6, 0x14,
        0x6f, 0xea, 0xc8, 0x61, 0xe1, 0x9b, 0x56, 0x3a
    }

    /* For SHA512_T testing we use t=224 & t=256
     * so the hash digest should be same as above
     */
};

#endif //SOC_SHA_SUPPORT_SHA512_T

#endif /* defined(SOC_SHA_SUPPORT_SHA384) || defined(SOC_SHA_SUPPORT_SHA512) */

#endif /* SOC_SHA_SUPPORTED */
