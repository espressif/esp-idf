// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");


#include "crypto/includes.h"
#include "crypto/common.h"
#include "crypto/aes.h"
#include "crypto/aes_wrap.h"
#include "mbedtls/aes.h"

/**
 * fast_aes_128_cbc_encrypt - AES-128 CBC encryption
 * @key: Encryption key
 * @iv: Encryption IV for CBC mode (16 bytes)
 * @data: Data to encrypt in-place
 * @data_len: Length of data in bytes (must be divisible by 16)
 * Returns: 0 on success, -1 on failure
 */
int 
fast_aes_128_cbc_encrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len)
{     
    int ret = 0;
    mbedtls_aes_context ctx;
    uint8_t cbc[AES_BLOCK_SIZE];

    mbedtls_aes_init(&ctx);
        
    ret = mbedtls_aes_setkey_enc(&ctx, key, 128);
  
    if(ret < 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }
        
    os_memcpy(cbc, iv, AES_BLOCK_SIZE);
        
    ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, data_len, cbc, data, data);

    mbedtls_aes_free(&ctx);
        
    return ret;
}


/**
 * fast_aes_128_cbc_decrypt - AES-128 CBC decryption
 * @key: Decryption key
 * @iv: Decryption IV for CBC mode (16 bytes)
 * @data: Data to decrypt in-place
 * @data_len: Length of data in bytes (must be divisible by 16)
 * Returns: 0 on success, -1 on failure
 */
int 
fast_aes_128_cbc_decrypt(const uint8_t *key, const uint8_t *iv, uint8_t *data, size_t data_len)
{
    int ret = 0;
    mbedtls_aes_context ctx;
    uint8_t cbc[AES_BLOCK_SIZE];

    mbedtls_aes_init(&ctx);
        
    ret = mbedtls_aes_setkey_dec(&ctx, key, 128);
  
    if(ret < 0) {
        mbedtls_aes_free(&ctx);
        return ret;
    }
        
    os_memcpy(cbc, iv, AES_BLOCK_SIZE);
        
    ret = mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, data_len, cbc, data, data);

    mbedtls_aes_free(&ctx);
        
    return ret;

}
