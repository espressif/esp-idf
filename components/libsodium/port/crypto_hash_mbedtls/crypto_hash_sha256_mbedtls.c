/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crypto_hash_sha256.h"
#include "mbedtls/sha256.h"
#include <string.h>

#ifdef MBEDTLS_SHA256_ALT
/* Wrapper only works if the libsodium context structure can be mapped
   directly to the mbedTLS context structure.

   See extended comments in crypto_hash_sha512_mbedtls.c
*/
#error "This wrapper only support standard software mbedTLS SHA"
#endif

/* Sanity check that all the context fields have identical sizes
   (this should be more or less given from the SHA256 algorithm)

   Note that the meaning of the fields is *not* all the same. In libsodium, SHA256 'count' is a 64-bit *bit* count. In
   mbedTLS, 'total' is a 2x32-bit *byte* count (count[0] == MSB).

   For this implementation, we don't convert so the libsodium state structure actually holds a binary copy of the
   mbedTLS totals. This doesn't matter inside libsodium's documented API, but would matter if any callers try to use
   the state's bit count.
*/
_Static_assert(sizeof(((crypto_hash_sha256_state *)0)->state) == sizeof(((mbedtls_sha256_context *)0)->state), "state mismatch");
_Static_assert(sizeof(((crypto_hash_sha256_state *)0)->count) == sizeof(((mbedtls_sha256_context *)0)->total), "count mismatch");
_Static_assert(sizeof(((crypto_hash_sha256_state *)0)->buf) == sizeof(((mbedtls_sha256_context *)0)->buffer), "buf mismatch");

/* Inline functions to convert between mbedTLS & libsodium
   context structures
*/

static void sha256_mbedtls_to_libsodium(crypto_hash_sha256_state *ls_state, const mbedtls_sha256_context *mb_ctx)
{
    memcpy(&ls_state->count, mb_ctx->total, sizeof(ls_state->count));
    memcpy(ls_state->state, mb_ctx->state, sizeof(ls_state->state));
    memcpy(ls_state->buf, mb_ctx->buffer, sizeof(ls_state->buf));
}

static void sha256_libsodium_to_mbedtls(mbedtls_sha256_context *mb_ctx, crypto_hash_sha256_state *ls_state)
{
    memcpy(mb_ctx->total, &ls_state->count, sizeof(mb_ctx->total));
    memcpy(mb_ctx->state, ls_state->state, sizeof(mb_ctx->state));
    memcpy(mb_ctx->buffer, ls_state->buf, sizeof(mb_ctx->buffer));
    mb_ctx->is224 = 0;
}

int
crypto_hash_sha256_init(crypto_hash_sha256_state *state)
{
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    int ret = mbedtls_sha256_starts_ret(&ctx, 0);
    if (ret != 0) {
        return ret;
    }
    sha256_mbedtls_to_libsodium(state, &ctx);
    return 0;
}

int
crypto_hash_sha256_update(crypto_hash_sha256_state *state,
                          const unsigned char *in, unsigned long long inlen)
{
    mbedtls_sha256_context ctx;
    sha256_libsodium_to_mbedtls(&ctx, state);
    int ret = mbedtls_sha256_update_ret(&ctx, in, inlen);
    if (ret != 0) {
        return ret;
    }
    sha256_mbedtls_to_libsodium(state, &ctx);
    return 0;
}

int
crypto_hash_sha256_final(crypto_hash_sha256_state *state, unsigned char *out)
{
    mbedtls_sha256_context ctx;
    sha256_libsodium_to_mbedtls(&ctx, state);
    return mbedtls_sha256_finish_ret(&ctx, out);
}

int
crypto_hash_sha256(unsigned char *out, const unsigned char *in,
                   unsigned long long inlen)
{
    return mbedtls_sha256_ret(in, inlen, out, 0);
}
