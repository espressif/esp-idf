/* aes.h - TinyCrypt interface to an AES-128 implementation */

/*
 *  Copyright (C) 2017 by Intel Corporation, All Rights Reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *    - Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *    - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 *    - Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief -- Interface to an AES-128 implementation.
 *
 *  Overview:   AES-128 is a NIST approved block cipher specified in
 *              FIPS 197. Block ciphers are deterministic algorithms that
 *              perform a transformation specified by a symmetric key in fixed-
 *              length data sets, also called blocks.
 *
 *  Security:   AES-128 provides approximately 128 bits of security.
 *
 *  Usage:      1) call tc_aes128_set_encrypt/decrypt_key to set the key.
 *
 *              2) call tc_aes_encrypt/decrypt to process the data.
 */

#ifndef _BLE_MESH_AES_ENCRYPT_H_
#define _BLE_MESH_AES_ENCRYPT_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define Nb (4)  /* number of columns (32-bit words) comprising the state */
#define Nk (4)  /* number of 32-bit words comprising the key */
#define Nr (10) /* number of rounds */
#define TC_AES_BLOCK_SIZE   (Nb*Nk)
#define TC_AES_KEY_SIZE     (Nb*Nk)

#define TC_CRYPTO_SUCCESS   1
#define TC_CRYPTO_FAIL      0

#define TC_ZERO_BYTE        0x00

/* padding for last message block */
#define TC_CMAC_PADDING     0x80

typedef struct tc_aes_key_sched_struct {
    unsigned int words[Nb * (Nr + 1)];
} *TCAesKeySched_t;

/* struct tc_cmac_struct represents the state of a CMAC computation */
typedef struct tc_cmac_struct {
    /* initialization vector */
    uint8_t iv[TC_AES_BLOCK_SIZE];
    /* used if message length is a multiple of block_size bytes */
    uint8_t K1[TC_AES_BLOCK_SIZE];
    /* used if message length isn't a multiple block_size bytes */
    uint8_t K2[TC_AES_BLOCK_SIZE];
    /* where to put bytes that didn't fill a block */
    uint8_t leftover[TC_AES_BLOCK_SIZE];
    /* identifies the encryption key */
    unsigned int keyid;
    /* next available leftover location */
    unsigned int leftover_offset;
    /* AES key schedule */
    TCAesKeySched_t sched;
    /* calls to tc_cmac_update left before re-key */
    uint64_t countdown;
} *TCCmacState_t;

/**
 *  @brief Set AES-128 encryption key
 *  Uses key k to initialize s
 *  @return  returns TC_CRYPTO_SUCCESS (1)
 *           returns TC_CRYPTO_FAIL (0) if: s == NULL or k == NULL
 *  @note       This implementation skips the additional steps required for keys
 *              larger than 128 bits, and must not be used for AES-192 or
 *              AES-256 key schedule -- see FIPS 197 for details
 *  @param      s IN/OUT -- initialized struct tc_aes_key_sched_struct
 *  @param      k IN -- points to the AES key
 */
int tc_aes128_set_encrypt_key(TCAesKeySched_t s, const uint8_t *k);

/**
 *  @brief AES-128 Encryption procedure
 *  Encrypts contents of in buffer into out buffer under key;
 *              schedule s
 *  @note Assumes s was initialized by aes_set_encrypt_key;
 *              out and in point to 16 byte buffers
 *  @return  returns TC_CRYPTO_SUCCESS (1)
 *           returns TC_CRYPTO_FAIL (0) if: out == NULL or in == NULL or s == NULL
 *  @param out IN/OUT -- buffer to receive ciphertext block
 *  @param in IN -- a plaintext block to encrypt
 *  @param s IN -- initialized AES key schedule
 */
int tc_aes_encrypt(uint8_t *out, const uint8_t *in,
                   const TCAesKeySched_t s);

/**
 *  @brief Set the AES-128 decryption key
 *  Uses key k to initialize s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if: s == NULL or k == NULL
 *  @note       This is the implementation of the straightforward inverse cipher
 *              using the cipher documented in FIPS-197 figure 12, not the
 *              equivalent inverse cipher presented in Figure 15
 *  @warning    This routine skips the additional steps required for keys larger
 *              than 128, and must not be used for AES-192 or AES-256 key
 *              schedule -- see FIPS 197 for details
 *  @param s  IN/OUT -- initialized struct tc_aes_key_sched_struct
 *  @param k  IN -- points to the AES key
 */
int tc_aes128_set_decrypt_key(TCAesKeySched_t s, const uint8_t *k);

/**
 *  @brief AES-128 Encryption procedure
 *  Decrypts in buffer into out buffer under key schedule s
 *  @return returns TC_CRYPTO_SUCCESS (1)
 *          returns TC_CRYPTO_FAIL (0) if: out is NULL or in is NULL or s is NULL
 *  @note   Assumes s was initialized by aes_set_encrypt_key
 *          out and in point to 16 byte buffers
 *  @param out IN/OUT -- buffer to receive ciphertext block
 *  @param in IN -- a plaintext block to encrypt
 *  @param s IN -- initialized AES key schedule
 */
int tc_aes_decrypt(uint8_t *out, const uint8_t *in,
                   const TCAesKeySched_t s);

int tc_cmac_setup(TCCmacState_t s, const uint8_t *key, TCAesKeySched_t sched);

void gf_double(uint8_t *out, uint8_t *in);

int tc_cmac_init(TCCmacState_t s);

int tc_cmac_update(TCCmacState_t s, const uint8_t *data, size_t data_length);

int tc_cmac_final(uint8_t *tag, TCCmacState_t s);

int tc_cmac_erase(TCCmacState_t s);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_AES_ENCRYPT_H_ */
