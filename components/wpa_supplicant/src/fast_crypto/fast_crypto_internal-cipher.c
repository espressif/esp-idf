// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//#include "wpa/includes.h"

//#include "wpa/common.h"
#include "crypto/common.h"
#include "crypto/crypto.h"
#include "crypto/aes.h"
#if defined(CONFIG_DES) || defined(CONFIG_DES3)
#include "crypto/des_i.h"
#endif
#include "mbedtls/aes.h"

struct fast_crypto_cipher {
    enum crypto_cipher_alg alg;
    union {
        struct {
            size_t used_bytes;
            uint8_t key[16];
            size_t keylen;
        } rc4;
        struct {
            uint8_t cbc[32];
            mbedtls_aes_context ctx_enc;
            mbedtls_aes_context ctx_dec;
        } aes;
#ifdef CONFIG_DES3
        struct {
            struct des3_key_s key;
	    uint8_t cbc[8];
        } des3;
#endif
#ifdef CONFIG_DES
	struct {
	    uint32_t ek[32];
	    uint32_t dk[32];
	    uint32_t cbc[8];
	} des;
#endif
    } u;
};


struct crypto_cipher *  fast_crypto_cipher_init(enum crypto_cipher_alg alg,
					  const uint8_t *iv, const uint8_t *key,
					  size_t key_len)
{
    struct fast_crypto_cipher *ctx;

    ctx = (struct fast_crypto_cipher *)os_zalloc(sizeof(*ctx));
    if (ctx == NULL) {
        return NULL;
    }
    
    ctx->alg = alg;

    switch (alg) {
        case CRYPTO_CIPHER_ALG_RC4:
            if (key_len > sizeof(ctx->u.rc4.key)) {
	        os_free(ctx);
	        return NULL;
            }
            ctx->u.rc4.keylen = key_len;
            os_memcpy(ctx->u.rc4.key, key, key_len);
            break;
        case CRYPTO_CIPHER_ALG_AES:                
            mbedtls_aes_init(&(ctx->u.aes.ctx_enc));
            mbedtls_aes_setkey_enc(&(ctx->u.aes.ctx_enc), key, 256);
            mbedtls_aes_init(&(ctx->u.aes.ctx_dec));
            mbedtls_aes_setkey_dec(&(ctx->u.aes.ctx_dec), key, 256);               
            os_memcpy(ctx->u.aes.cbc, iv, AES_BLOCK_SIZE);
            break;
#ifdef CONFIG_DES3
        case CRYPTO_CIPHER_ALG_3DES:
            if (key_len != 24) {
	        os_free(ctx);
	        return NULL;
            }
            des3_key_setup(key, &ctx->u.des3.key);
            os_memcpy(ctx->u.des3.cbc, iv, 8);
            break;
#endif
#ifdef CONFIG_DES
        case CRYPTO_CIPHER_ALG_DES:
            if (key_len != 8) {
	        os_free(ctx);
	        return NULL;
            }
            des_key_setup(key, ctx->u.des.ek, ctx->u.des.dk);
            os_memcpy(ctx->u.des.cbc, iv, 8);
            break;
#endif
        default:
            os_free(ctx);
            return NULL;
    }

    return (struct crypto_cipher *)ctx;
}


int  fast_crypto_cipher_encrypt(struct crypto_cipher *ctx, const uint8_t *plain,
			  uint8_t *crypt, size_t len)
{
    size_t i, j, blocks;
    struct fast_crypto_cipher *fast_ctx;

    fast_ctx = (struct fast_crypto_cipher *)ctx;

    switch (fast_ctx->alg) {
        case CRYPTO_CIPHER_ALG_RC4:
            if (plain != crypt) {
                os_memcpy(crypt, plain, len);
            }  
            rc4_skip(fast_ctx->u.rc4.key, fast_ctx->u.rc4.keylen,
            fast_ctx->u.rc4.used_bytes, crypt, len);
            fast_ctx->u.rc4.used_bytes += len;
            break;
        case CRYPTO_CIPHER_ALG_AES:
            if (len % AES_BLOCK_SIZE) {
                return -1;
            }
            blocks = len / AES_BLOCK_SIZE;
            for (i = 0; i < blocks; i++) {
                for (j = 0; j < AES_BLOCK_SIZE; j++)
                    fast_ctx->u.aes.cbc[j] ^= plain[j];
                mbedtls_aes_encrypt(&(fast_ctx->u.aes.ctx_enc), fast_ctx->u.aes.cbc, fast_ctx->u.aes.cbc);
                os_memcpy(crypt, fast_ctx->u.aes.cbc, AES_BLOCK_SIZE);
                plain += AES_BLOCK_SIZE;
                crypt += AES_BLOCK_SIZE;
            }
            break;
#ifdef CONFIG_DES3
        case CRYPTO_CIPHER_ALG_3DES:
            if (len % 8) {
                return -1;
            }
            blocks = len / 8;
            for (i = 0; i < blocks; i++) {
                for (j = 0; j < 8; j++)
	            fast_ctx->u.des3.cbc[j] ^= plain[j];
                des3_encrypt(fast_ctx->u.des3.cbc, &fast_ctx->u.des3.key,
	                     fast_ctx->u.des3.cbc);
                os_memcpy(crypt, fast_ctx->u.des3.cbc, 8);
                plain += 8;
                crypt += 8;
            }
            break;
#endif
#ifdef CONFIG_DES
        case CRYPTO_CIPHER_ALG_DES:
            if (len % 8) {
                return -1;
            }
            blocks = len / 8;
            for (i = 0; i < blocks; i++) {
                for (j = 0; j < 8; j++)
	            fast_ctx->u.des3.cbc[j] ^= plain[j];
                des_block_encrypt(fast_ctx->u.des.cbc, fast_ctx->u.des.ek,
		                  fast_ctx->u.des.cbc);
                os_memcpy(crypt, fast_ctx->u.des.cbc, 8);
                plain += 8;
                crypt += 8;
            }
            break;
#endif
        default:
            return -1;
    }

    return 0;
}


int  fast_crypto_cipher_decrypt(struct crypto_cipher *ctx, const uint8_t *crypt,
			  uint8_t *plain, size_t len)
{
    size_t i, j, blocks;
    uint8_t tmp[32];
    struct fast_crypto_cipher *fast_ctx;

    fast_ctx = (struct fast_crypto_cipher *)ctx;
    
    switch (fast_ctx->alg) {
        case CRYPTO_CIPHER_ALG_RC4:
            if (plain != crypt) {
                os_memcpy(plain, crypt, len);
            }
            rc4_skip(fast_ctx->u.rc4.key, fast_ctx->u.rc4.keylen,
            fast_ctx->u.rc4.used_bytes, plain, len);
            fast_ctx->u.rc4.used_bytes += len;
            break;
        case CRYPTO_CIPHER_ALG_AES:
            if (len % AES_BLOCK_SIZE) {
                return -1;
            }
            blocks = len / AES_BLOCK_SIZE;
            for (i = 0; i < blocks; i++) {
                os_memcpy(tmp, crypt, AES_BLOCK_SIZE);
                mbedtls_aes_decrypt(&(fast_ctx->u.aes.ctx_dec), crypt, plain);
                for (j = 0; j < AES_BLOCK_SIZE; j++)
                    plain[j] ^= fast_ctx->u.aes.cbc[j];
                os_memcpy(fast_ctx->u.aes.cbc, tmp, AES_BLOCK_SIZE);
                plain += AES_BLOCK_SIZE;
                crypt += AES_BLOCK_SIZE;
            }
            break;
#ifdef CONFIG_DES3
        case CRYPTO_CIPHER_ALG_3DES:
            if (len % 8) {
                return -1;
            }
            blocks = len / 8;
            for (i = 0; i < blocks; i++) {
                os_memcpy(tmp, crypt, 8);
                des3_decrypt(crypt, &fast_ctx->u.des3.key, plain);
                for (j = 0; j < 8; j++) {
                    plain[j] ^= fast_ctx->u.des3.cbc[j];
                }
                os_memcpy(fast_ctx->u.des3.cbc, tmp, 8);
                plain += 8;
                crypt += 8;
            }
            break;
#endif
#ifdef CONFIG_DES
        case CRYPTO_CIPHER_ALG_DES:
            if (len % 8) {
                return -1;
            }
            blocks = len / 8;
            for (i = 0; i < blocks; i++) {
                os_memcpy(tmp, crypt, 8);
                des_block_decrypt(crypt, fast_ctx->u.des.dk, plain);
                for (j = 0; j < 8; j++) {
                    plain[j] ^= fast_ctx->u.des.cbc[j];
                }
                os_memcpy(fast_ctx->u.des.cbc, tmp, 8);
                plain += 8;
                crypt += 8;
            }
            break;
#endif
        default:
            return -1;
}

return 0;
}


void  fast_crypto_cipher_deinit(struct crypto_cipher *ctx)
{
    struct fast_crypto_cipher *fast_ctx;

    fast_ctx = (struct fast_crypto_cipher *)ctx;

    switch (fast_ctx->alg) {
        case CRYPTO_CIPHER_ALG_AES:
            mbedtls_aes_free(&(fast_ctx->u.aes.ctx_enc));
            mbedtls_aes_free(&(fast_ctx->u.aes.ctx_dec));
            break;
#ifdef CONFIG_DES3
        case CRYPTO_CIPHER_ALG_3DES:
            break;
#endif
        default:
            break;
    }
    os_free(ctx);
}
