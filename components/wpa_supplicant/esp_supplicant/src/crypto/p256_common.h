/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Shared P-256 (secp256r1) word-level and Montgomery arithmetic used by
 * both the bignum and EC fast paths.  All helpers are static inline so
 * each translation unit gets its own copy without linkage issues.
 *
 * Prerequisites: the including .c file must already provide u8/u32/u64
 * typedefs, os_memcmp/os_memset/os_memcpy (via utils/common.h), and
 * mbedtls/bignum.h.
 */

#pragma once

#define P256_WORDS     8
#define P256_LEN_BYTES 32

static const u8 p256_p_be[P256_LEN_BYTES] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

static const u32 p256_p_le[P256_WORDS] = {
    0xffffffffU, 0xffffffffU, 0xffffffffU, 0x00000000U,
    0x00000000U, 0x00000000U, 0x00000001U, 0xffffffffU
};

/* R^2 mod p in little-endian word order, for Montgomery domain entry. */
static const u32 p256_r2_le[P256_WORDS] = {
    0x00000003U, 0x00000000U, 0xffffffffU, 0xfffffffbU,
    0xfffffffeU, 0xffffffffU, 0xfffffffdU, 0x00000004U
};

static inline int p256_words_is_zero(const u32 *a)
{
    size_t i;

    for (i = 0; i < P256_WORDS; i++) {
        if (a[i] != 0) {
            return 0;
        }
    }

    return 1;
}

static inline size_t p256_words_bitlen(const u32 *a)
{
    int i;

    for (i = P256_WORDS - 1; i >= 0; i--) {
        if (a[i] != 0) {
            return (size_t) i * 32 + 32 - __builtin_clz(a[i]);
        }
    }

    return 0;
}

static inline int p256_words_from_mpi(const mbedtls_mpi *in, u32 *out)
{
    u8 in_be[P256_LEN_BYTES];
    size_t i;

    if (!in || in->MBEDTLS_PRIVATE(s) < 0 ||
            mbedtls_mpi_size(in) > P256_LEN_BYTES ||
            mbedtls_mpi_write_binary(in, in_be, sizeof(in_be)) != 0) {
        return -1;
    }

    for (i = 0; i < P256_WORDS; i++) {
        size_t off = P256_LEN_BYTES - (i + 1) * 4;

        out[i] = ((u32) in_be[off] << 24) |
                 ((u32) in_be[off + 1] << 16) |
                 ((u32) in_be[off + 2] << 8) |
                 (u32) in_be[off + 3];
    }

    return 0;
}

static inline int p256_words_from_mpi_reduced(const mbedtls_mpi *in, u32 *out)
{
    u8 in_be[P256_LEN_BYTES];
    size_t i;
    size_t in_size;

    if (!in || in->MBEDTLS_PRIVATE(s) < 0) {
        return -1;
    }

    in_size = mbedtls_mpi_size(in);
    if (in_size > P256_LEN_BYTES) {
        return -1;
    }

    if (mbedtls_mpi_write_binary(in, in_be, sizeof(in_be)) != 0) {
        return -1;
    }

    if (in_size == P256_LEN_BYTES &&
            os_memcmp(in_be, p256_p_be, sizeof(in_be)) >= 0) {
        return -1;
    }

    for (i = 0; i < P256_WORDS; i++) {
        size_t off = P256_LEN_BYTES - (i + 1) * 4;

        out[i] = ((u32) in_be[off] << 24) |
                 ((u32) in_be[off + 1] << 16) |
                 ((u32) in_be[off + 2] << 8) |
                 (u32) in_be[off + 3];
    }

    return 0;
}

static inline int p256_words_to_mpi(const u32 *in, mbedtls_mpi *out)
{
    u8 out_be[P256_LEN_BYTES];
    size_t i;

    for (i = 0; i < P256_WORDS; i++) {
        size_t off = P256_LEN_BYTES - (i + 1) * 4;

        out_be[off] = (u8)(in[i] >> 24);
        out_be[off + 1] = (u8)(in[i] >> 16);
        out_be[off + 2] = (u8)(in[i] >> 8);
        out_be[off + 3] = (u8) in[i];
    }

    return mbedtls_mpi_read_binary(out, out_be, sizeof(out_be));
}

static inline u32 p256_words_sub_borrow(u32 *z, const u32 *x, const u32 *y)
{
    size_t i;
    u32 borrow = 0;

    for (i = 0; i < P256_WORDS; i++) {
        u64 diff = (u64) x[i] - y[i] - borrow;

        z[i] = (u32) diff;
        borrow = -(u32)(diff >> 32);
    }

    return borrow;
}

static inline void p256_words_cmov(u32 *z, const u32 *x, u32 c)
{
    size_t i;
    u32 mask = (u32) - (int) c;

    for (i = 0; i < P256_WORDS; i++) {
        z[i] = (z[i] & ~mask) | (x[i] & mask);
    }
}

static inline u64 p256_u32_muladd64(u32 x, u32 y, u32 z, u32 t)
{
    return (u64) x * y + z + t;
}

static inline u32 p256_u288_muladd(u32 z[P256_WORDS + 1], u32 x,
                                   const u32 y[P256_WORDS])
{
    size_t i;
    u32 carry = 0;

    for (i = 0; i < P256_WORDS; i++) {
        u64 prod = p256_u32_muladd64(x, y[i], z[i], carry);

        z[i] = (u32) prod;
        carry = (u32)(prod >> 32);
    }

    {
        u64 sum = (u64) z[P256_WORDS] + carry;
        z[P256_WORDS] = (u32) sum;
        carry = (u32)(sum >> 32);
    }

    return carry;
}

static inline void p256_u288_rshift32(u32 z[P256_WORDS + 1], u32 c)
{
    size_t i;

    for (i = 0; i < P256_WORDS; i++) {
        z[i] = z[i + 1];
    }
    z[P256_WORDS] = c;
}

/*
 * CIOS Montgomery multiplication for secp256r1.
 *
 * The Montgomery constant mu = -p^{-1} mod 2^{32} equals 1 for this prime
 * because p[0] = 0xFFFFFFFF, i.e. p ≡ -1 (mod 2^{32}).  That simplifies
 * the reduction factor to u = new_a[0] * 1 = new_a[0], which we compute
 * early as a[0] + x[i]*y[0] (the low word of the partial accumulator after
 * the multiply step) to break the data dependency.
 */
static inline void p256_mont_mul(u32 z[P256_WORDS],
                                 const u32 x[P256_WORDS],
                                 const u32 y[P256_WORDS])
{
    u32 a[P256_WORDS + 1] = {0};
    u32 reduced[P256_WORDS];
    size_t i;

    for (i = 0; i < P256_WORDS; i++) {
        u32 u = a[0] + x[i] * y[0];
        u32 c = p256_u288_muladd(a, x[i], y);
        c += p256_u288_muladd(a, u, p256_p_le);
        p256_u288_rshift32(a, c);
    }

    {
        u32 carry_add = a[P256_WORDS];
        u32 carry_sub = p256_words_sub_borrow(reduced, a, p256_p_le);
        u32 use_sub = carry_add | (1U - carry_sub);

        os_memcpy(z, a, sizeof(u32) * P256_WORDS);
        p256_words_cmov(z, reduced, use_sub);
    }
}
