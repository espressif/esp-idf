/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crypto_hash_sha512.h"
#include "mbedtls/sha512.h"
#include <string.h>

#ifdef MBEDTLS_SHA512_ALT
/* Wrapper only works if the libsodium context structure can be mapped
   directly to the mbedTLS context structure.

   For ESP32 hardware SHA, the problems are fitting all the data in
   the libsodium state structure, and also that libsodium doesn't
   have mbedtls_sha512_free() or mbedtls_sha512_clone() so we can't
   manage the hardware state in a clean way.
*/
#error "This wrapper only support standard software mbedTLS SHA"
#endif

/* Sanity check that all the context fields have identical sizes
   (this should be more or less given from the SHA512 algorithm)

   Note that the meaning of the fields is *not* all the same. In libsodium,
   SHA512 'count' is a 2xuin64_t *bit* count where count[0] == MSB. In mbedTLS,
   SHA512 'total' is a 2xuint64_t *byte* count where count[0] == LSB.

   For this implementation, we don't convert so the libsodium state structure actually holds a binary copy of the
   mbedTLS totals. This doesn't matter inside libsodium's documented API, but would matter if any callers try to use
   the state's bit count.
*/
_Static_assert(sizeof(((crypto_hash_sha512_state *)0)->state) == sizeof(((mbedtls_sha512_context *)0)->state), "state mismatch");
_Static_assert(sizeof(((crypto_hash_sha512_state *)0)->count) == sizeof(((mbedtls_sha512_context *)0)->total), "count mismatch");
_Static_assert(sizeof(((crypto_hash_sha512_state *)0)->buf) == sizeof(((mbedtls_sha512_context *)0)->buffer), "buf mismatch");

/* Inline functions to convert between mbedTLS & libsodium
   context structures
*/

static void sha512_mbedtls_to_libsodium(crypto_hash_sha512_state *ls_state, const mbedtls_sha512_context *mb_ctx)
{
    memcpy(ls_state->count, mb_ctx->total, sizeof(ls_state->count));
    memcpy(ls_state->state, mb_ctx->state, sizeof(ls_state->state));
    memcpy(ls_state->buf, mb_ctx->buffer, sizeof(ls_state->buf));
}

static void sha512_libsodium_to_mbedtls(mbedtls_sha512_context *mb_ctx, crypto_hash_sha512_state *ls_state)
{
    memcpy(mb_ctx->total, ls_state->count, sizeof(mb_ctx->total));
    memcpy(mb_ctx->state, ls_state->state, sizeof(mb_ctx->state));
    memcpy(mb_ctx->buffer, ls_state->buf, sizeof(mb_ctx->buffer));
    mb_ctx->is384 = 0;
}

int
crypto_hash_sha512_init(crypto_hash_sha512_state *state)
{
    mbedtls_sha512_context ctx;
    mbedtls_sha512_init(&ctx);
    int ret = mbedtls_sha512_starts_ret(&ctx, 0);
    if (ret != 0) {
        return ret;
    }
    sha512_mbedtls_to_libsodium(state, &ctx);
    return 0;
}

int
crypto_hash_sha512_update(crypto_hash_sha512_state *state,
                          const unsigned char *in, unsigned long long inlen)
{
    mbedtls_sha512_context ctx;
    sha512_libsodium_to_mbedtls(&ctx, state);
    int ret = mbedtls_sha512_update_ret(&ctx, in, inlen);
    if (ret != 0) {
        return ret;
    }
    sha512_mbedtls_to_libsodium(state, &ctx);
    return 0;
}

int
crypto_hash_sha512_final(crypto_hash_sha512_state *state, unsigned char *out)
{
    mbedtls_sha512_context ctx;
    sha512_libsodium_to_mbedtls(&ctx, state);
    return mbedtls_sha512_finish_ret(&ctx, out);
}

int
crypto_hash_sha512(unsigned char *out, const unsigned char *in,
                   unsigned long long inlen)
{
    return mbedtls_sha512_ret(in, inlen, out, 0);
}
