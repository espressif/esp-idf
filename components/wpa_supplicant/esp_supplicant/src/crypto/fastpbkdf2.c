/*
 * SPDX-FileCopyrightText: 2015 Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
 * fast-pbkdf2 - Optimal PBKDF2-HMAC calculation
 * Written in 2015 by Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to the
 * public domain worldwide. This software is distributed without any
 * warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication
 * along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#include "utils/common.h"
#include "fastpbkdf2.h"

#include <assert.h>
#include <string.h>
#if defined(__GNUC__)
#include <endian.h>
#endif

#include <mbedtls/sha1.h>
#include "mbedtls/esp_config.h"
#include "utils/wpa_debug.h"

/* --- MSVC doesn't support C99 --- */
#ifdef _MSC_VER
#define restrict
#define _Pragma __pragma
#endif

/* --- Common useful things --- */
#ifndef MIN
#define MIN(a, b) ((a) > (b)) ? (b) : (a)
#endif

static inline void write32_be(uint32_t n, uint8_t out[4])
{
#if defined(__GNUC__) && __GNUC__ >= 4 && __BYTE_ORDER == __LITTLE_ENDIAN
  *(uint32_t *)(out) = __builtin_bswap32(n);
#else
  out[0] = (n >> 24) & 0xff;
  out[1] = (n >> 16) & 0xff;
  out[2] = (n >> 8) & 0xff;
  out[3] = n & 0xff;
#endif
}

/* Prepare block (of blocksz bytes) to contain md padding denoting a msg-size
 * message (in bytes).  block has a prefix of used bytes.
 *
 * Message length is expressed in 32 bits (so suitable for sha1, sha256, sha512). */
static inline void md_pad(uint8_t *block, size_t blocksz, size_t used, size_t msg)
{
  memset(block + used, 0, blocksz - used - 4);
  block[used] = 0x80;
  block += blocksz - 4;
  write32_be((uint32_t) (msg * 8), block);
}

/* Internal function/type names for hash-specific things. */
#define HMAC_CTX(_name) HMAC_ ## _name ## _ctx
#define HMAC_INIT(_name) HMAC_ ## _name ## _init
#define HMAC_UPDATE(_name) HMAC_ ## _name ## _update
#define HMAC_FINAL(_name) HMAC_ ## _name ## _final

#define PBKDF2_F(_name) pbkdf2_f_ ## _name
#define PBKDF2(_name) pbkdf2_ ## _name

/* This macro expands to decls for the whole implementation for a given
 * hash function.  Arguments are:
 *
 * _name like 'sha1', added to symbol names
 * _blocksz block size, in bytes
 * _hashsz digest output, in bytes
 * _ctx hash context type
 * _init hash context initialisation function
 *    args: (_ctx *c)
 * _update hash context update function
 *    args: (_ctx *c, const void *data, size_t ndata)
 * _final hash context finish function
 *    args: (_ctx *c, void *out)
 * _xform hash context raw block update function
 *    args: (_ctx *c, const void *data)
 * _xcpy hash context raw copy function (only need copy hash state)
 *    args: (_ctx * restrict out, const _ctx *restrict in)
 * _xtract hash context state extraction
 *    args: args (_ctx *restrict c, uint8_t *restrict out)
 * _xxor hash context xor function (only need xor hash state)
 *    args: (_ctx *restrict out, const _ctx *restrict in)
 *
 * The resulting function is named PBKDF2(_name).
 */
#define DECL_PBKDF2(_name, _blocksz, _hashsz, _ctx,                           \
                    _init, _update, _xform, _final, _xcpy, _xtract, _xxor)    \
  typedef struct {                                                            \
    _ctx inner;                                                               \
    _ctx outer;                                                               \
  } HMAC_CTX(_name);                                                          \
                                                                              \
  static inline void HMAC_INIT(_name)(HMAC_CTX(_name) *ctx,                   \
                                      const uint8_t *key, size_t nkey)        \
  {                                                                           \
    /* Prepare key: */                                                        \
    uint8_t k[_blocksz];                                                      \
                                                                              \
    /* Shorten long keys. */                                                  \
    if (nkey > _blocksz)                                                      \
    {                                                                         \
      _init(&ctx->inner);                                                     \
      _update(&ctx->inner, key, nkey);                                        \
      _final(&ctx->inner, k);                                                 \
                                                                              \
      key = k;                                                                \
      nkey = _hashsz;                                                         \
    }                                                                         \
                                                                              \
    /* Standard doesn't cover case where blocksz < hashsz. */                 \
    assert(nkey <= _blocksz);                                                 \
                                                                              \
    /* Right zero-pad short keys. */                                          \
    if (k != key)                                                             \
      memcpy(k, key, nkey);                                                   \
    if (_blocksz > nkey)                                                      \
      memset(k + nkey, 0, _blocksz - nkey);                                   \
                                                                              \
    /* Start inner hash computation */                                        \
    uint8_t blk_inner[_blocksz];                                              \
    uint8_t blk_outer[_blocksz];                                              \
                                                                              \
    for (size_t i = 0; i < _blocksz; i++)                                     \
    {                                                                         \
      blk_inner[i] = 0x36 ^ k[i];                                             \
      blk_outer[i] = 0x5c ^ k[i];                                             \
    }                                                                         \
                                                                              \
    _init(&ctx->inner);                                                       \
    _update(&ctx->inner, blk_inner, sizeof blk_inner);                        \
                                                                              \
    /* And outer. */                                                          \
    _init(&ctx->outer);                                                       \
    _update(&ctx->outer, blk_outer, sizeof blk_outer);                        \
  }                                                                           \
                                                                              \
  static inline void HMAC_UPDATE(_name)(HMAC_CTX(_name) *ctx,                 \
                                        const void *data, size_t ndata)       \
  {                                                                           \
    _update(&ctx->inner, data, ndata);                                        \
  }                                                                           \
                                                                              \
  static inline void HMAC_FINAL(_name)(HMAC_CTX(_name) *ctx,                  \
                                       uint8_t out[_hashsz])                  \
  {                                                                           \
    _final(&ctx->inner, out);                                                 \
    _update(&ctx->outer, out, _hashsz);                                       \
    _final(&ctx->outer, out);                                                 \
  }                                                                           \
                                                                              \
                                                                              \
  /* --- PBKDF2 --- */                                                        \
  static inline void PBKDF2_F(_name)(const HMAC_CTX(_name) *startctx,         \
                                     uint32_t counter,                        \
                                     const uint8_t *salt, size_t nsalt,       \
                                     uint32_t iterations,                     \
                                     uint8_t *out)                            \
  {                                                                           \
    uint8_t countbuf[4];                                                      \
    write32_be(counter, countbuf);                                            \
                                                                              \
    /* Prepare loop-invariant padding block. */                               \
    uint8_t Ublock[_blocksz];                                                 \
    md_pad(Ublock, _blocksz, _hashsz, _blocksz + _hashsz);                    \
                                                                              \
    /* First iteration:                                                       \
     *   U_1 = PRF(P, S || INT_32_BE(i))                                      \
     */                                                                       \
    HMAC_CTX(_name) ctx = *startctx;                                          \
    HMAC_UPDATE(_name)(&ctx, salt, nsalt);                                    \
    HMAC_UPDATE(_name)(&ctx, countbuf, sizeof countbuf);                      \
    HMAC_FINAL(_name)(&ctx, Ublock);                                          \
    _ctx result = ctx.outer;                                                  \
                                                                              \
    /* Subsequent iterations:                                                 \
     *   U_c = PRF(P, U_{c-1})                                                \
     */                                                                       \
    for (uint32_t i = 1; i < iterations; i++)                                 \
    {                                                                         \
      /* Complete inner hash with previous U */                               \
      _xcpy(&ctx.inner, &startctx->inner);                                    \
      _xform(&ctx.inner, Ublock);                                             \
      _xtract(&ctx.inner, Ublock);                                            \
      /* Complete outer hash with inner output */                             \
      _xcpy(&ctx.outer, &startctx->outer);                                    \
      _xform(&ctx.outer, Ublock);                                             \
      _xtract(&ctx.outer, Ublock);                                            \
      _xxor(&result, &ctx.outer);                                             \
    }                                                                         \
                                                                              \
    /* Reform result into output buffer. */                                   \
    _xtract(&result, out);                                                    \
  }                                                                           \
                                                                              \
  static inline void PBKDF2(_name)(const uint8_t *pw, size_t npw,             \
                     const uint8_t *salt, size_t nsalt,                       \
                     uint32_t iterations,                                     \
                     uint8_t *out, size_t nout)                               \
  {                                                                           \
    assert(iterations);                                                       \
    assert(out && nout);                                                      \
                                                                              \
    /* Starting point for inner loop. */                                      \
    HMAC_CTX(_name) ctx;                                                      \
    HMAC_INIT(_name)(&ctx, pw, npw);                                          \
                                                                              \
    /* How many blocks do we need? */                                         \
    uint32_t blocks_needed = (uint32_t)(nout + _hashsz - 1) / _hashsz;        \
                                                                              \
    for (uint32_t counter = 1; counter <= blocks_needed; counter++)           \
    {                                                                         \
      uint8_t block[_hashsz];                                                 \
      PBKDF2_F(_name)(&ctx, counter, salt, nsalt, iterations, block);         \
                                                                              \
      size_t offset = (counter - 1) * _hashsz;                                \
      size_t taken = MIN(nout - offset, _hashsz);                             \
      memcpy(out + offset, block, taken);                                     \
    }                                                                         \
  }

static inline void sha1_extract(mbedtls_sha1_context *restrict ctx, uint8_t *restrict out)
{
#if defined(MBEDTLS_SHA1_ALT)
#if CONFIG_IDF_TARGET_ESP32
  /* ESP32 stores internal SHA state in BE format similar to software */
  write32_be(ctx->state[0], out);
  write32_be(ctx->state[1], out + 4);
  write32_be(ctx->state[2], out + 8);
  write32_be(ctx->state[3], out + 12);
  write32_be(ctx->state[4], out + 16);
#else
  *(uint32_t *)(out) = ctx->state[0];
  *(uint32_t *)(out + 4) = ctx->state[1];
  *(uint32_t *)(out + 8) = ctx->state[2];
  *(uint32_t *)(out + 12) = ctx->state[3];
  *(uint32_t *)(out + 16) = ctx->state[4];
#endif
#else
  write32_be(ctx->MBEDTLS_PRIVATE(state)[0], out);
  write32_be(ctx->MBEDTLS_PRIVATE(state)[1], out + 4);
  write32_be(ctx->MBEDTLS_PRIVATE(state)[2], out + 8);
  write32_be(ctx->MBEDTLS_PRIVATE(state)[3], out + 12);
  write32_be(ctx->MBEDTLS_PRIVATE(state)[4], out + 16);
#endif
}

static inline void sha1_cpy(mbedtls_sha1_context *restrict out, const mbedtls_sha1_context *restrict in)
{
#if defined(MBEDTLS_SHA1_ALT)
  out->state[0] = in->state[0];
  out->state[1] = in->state[1];
  out->state[2] = in->state[2];
  out->state[3] = in->state[3];
  out->state[4] = in->state[4];
#else
  out->MBEDTLS_PRIVATE(state)[0] = in->MBEDTLS_PRIVATE(state)[0];
  out->MBEDTLS_PRIVATE(state)[1] = in->MBEDTLS_PRIVATE(state)[1];
  out->MBEDTLS_PRIVATE(state)[2] = in->MBEDTLS_PRIVATE(state)[2];
  out->MBEDTLS_PRIVATE(state)[3] = in->MBEDTLS_PRIVATE(state)[3];
  out->MBEDTLS_PRIVATE(state)[4] = in->MBEDTLS_PRIVATE(state)[4];
#endif
}

static inline void sha1_xor(mbedtls_sha1_context *restrict out, const mbedtls_sha1_context *restrict in)
{
#if defined(MBEDTLS_SHA1_ALT)
  out->state[0] ^= in->state[0];
  out->state[1] ^= in->state[1];
  out->state[2] ^= in->state[2];
  out->state[3] ^= in->state[3];
  out->state[4] ^= in->state[4];
#else
  out->MBEDTLS_PRIVATE(state)[0] ^= in->MBEDTLS_PRIVATE(state)[0];
  out->MBEDTLS_PRIVATE(state)[1] ^= in->MBEDTLS_PRIVATE(state)[1];
  out->MBEDTLS_PRIVATE(state)[2] ^= in->MBEDTLS_PRIVATE(state)[2];
  out->MBEDTLS_PRIVATE(state)[3] ^= in->MBEDTLS_PRIVATE(state)[3];
  out->MBEDTLS_PRIVATE(state)[4] ^= in->MBEDTLS_PRIVATE(state)[4];
#endif
}

static int mbedtls_sha1_init_start(mbedtls_sha1_context *ctx)
{
  mbedtls_sha1_init(ctx);
  mbedtls_sha1_starts(ctx);
#if defined(CONFIG_IDF_TARGET_ESP32) && defined(MBEDTLS_SHA1_ALT)
  /* Use software mode for esp32 since hardware can't give output more than 20 */
  esp_mbedtls_set_sha1_mode(ctx, ESP_MBEDTLS_SHA1_SOFTWARE);
#endif
  return 0;
}

#ifndef MBEDTLS_SHA1_ALT
static int sha1_finish(mbedtls_sha1_context *ctx,
                        unsigned char output[20])
{
    int ret = -1;
    uint32_t used;
    uint32_t high, low;

    /*
     * Add padding: 0x80 then 0x00 until 8 bytes remain for the length
     */
    used = ctx->MBEDTLS_PRIVATE(total)[0] & 0x3F;

    ctx->MBEDTLS_PRIVATE(buffer)[used++] = 0x80;

    if (used <= 56) {
        /* Enough room for padding + length in current block */
        memset(ctx->MBEDTLS_PRIVATE(buffer) + used, 0, 56 - used);
    } else {
        /* We'll need an extra block */
        memset(ctx->MBEDTLS_PRIVATE(buffer) + used, 0, 64 - used);

        if ((ret = mbedtls_internal_sha1_process(ctx, ctx->MBEDTLS_PRIVATE(buffer))) != 0) {
            goto exit;
        }

        memset(ctx->MBEDTLS_PRIVATE(buffer), 0, 56);
    }

    /*
     * Add message length
     */
    high = (ctx->MBEDTLS_PRIVATE(total)[0] >> 29)
           | (ctx->MBEDTLS_PRIVATE(total)[1] <<  3);
    low  = (ctx->MBEDTLS_PRIVATE(total)[0] <<  3);

    write32_be(high, ctx->MBEDTLS_PRIVATE(buffer) + 56);
    write32_be(low, ctx->MBEDTLS_PRIVATE(buffer) + 60);

    if ((ret = mbedtls_internal_sha1_process(ctx, ctx->MBEDTLS_PRIVATE(buffer))) != 0) {
        goto exit;
    }

    /*
     * Output final state
     */
    write32_be(ctx->MBEDTLS_PRIVATE(state)[0], output);
    write32_be(ctx->MBEDTLS_PRIVATE(state)[1], output + 4);
    write32_be(ctx->MBEDTLS_PRIVATE(state)[2], output + 8);
    write32_be(ctx->MBEDTLS_PRIVATE(state)[3], output + 12);
    write32_be(ctx->MBEDTLS_PRIVATE(state)[4], output + 16);

    ret = 0;

exit:
    return ret;
}
#endif

DECL_PBKDF2(sha1,                           // _name
            64,                             // _blocksz
            20,                             // _hashsz
            mbedtls_sha1_context,           // _ctx
            mbedtls_sha1_init_start,        // _init
            mbedtls_sha1_update,            // _update
            mbedtls_internal_sha1_process,  // _xform
#if defined(MBEDTLS_SHA1_ALT)
            mbedtls_sha1_finish,            // _final
#else
            sha1_finish,                   // _final
#endif
            sha1_cpy,                       // _xcpy
            sha1_extract,                   // _xtract
            sha1_xor)                       // _xxor

void fastpbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                          const uint8_t *salt, size_t nsalt,
                          uint32_t iterations,
                          uint8_t *out, size_t nout)
{
  PBKDF2(sha1)(pw, npw, salt, nsalt, iterations, out, nout);
}
