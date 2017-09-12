// Hardware crypto support Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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


#ifndef __ESP_WIFI_CRYPTO_TYPES_H__
#define __ESP_WIFI_CRYPTO_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This enumation is about alorigthm will be set when do crypt hash 
 * operation.When do wpa2 connecting, after invoke crypto_hash_xxx of
 * fast_crypto_hash_xxx API, it will do relation crypt operation according
 * to the enumation.
 */
typedef enum {
    ESP_CRYPTO_HASH_ALG_MD5, ESP_CRYPTO_HASH_ALG_SHA1,
    ESP_CRYPTO_HASH_ALG_HMAC_MD5, ESP_CRYPTO_HASH_ALG_HMAC_SHA1,
    ESP_CRYPTO_HASH_ALG_SHA256, ESP_CRYPTO_HASH_ALG_HMAC_SHA256
}esp_crypto_hash_alg_t;

/*
 * This enumation is about alorigthm will be set when do crypt cipher 
 * operation.When do wpa2 connecting, after invoke crypto_cipher_xxx of
 * fast_crypto_cipher_xxx API, it will do relation crypt operation according
 * to the enumation.
 */
typedef enum {
    ESP_CRYPTO_CIPHER_NULL, ESP_CRYPTO_CIPHER_ALG_AES, ESP_CRYPTO_CIPHER_ALG_3DES,
    ESP_CRYPTO_CIPHER_ALG_DES, ESP_CRYPTO_CIPHER_ALG_RC2, ESP_CRYPTO_CIPHER_ALG_RC4
} esp_crypto_cipher_alg_t;

/*
 * This structure is about the algorithm when do crypto_hash operation, for detail, 
 * please reference to the structure crypto_hash.
 */
typedef struct crypto_hash esp_crypto_hash_t;

/*
 * This structure is about the algorithm when do crypto_cipher operation, for detail, 
 * please reference to the structure crypto_cipher.
 */
typedef struct crypto_cipher esp_crypto_cipher_t;

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect.
  *        Initialize a esp_crypto_hash_t structure.
  *
  * @param alg  Hash algorithm.
  * @param key  Key for keyed hash (e.g., HMAC) or %NULL if not needed.
  * @param key_len  Length of the key in bytes
  *
  */
typedef esp_crypto_hash_t * (*esp_crypto_hash_init_t)(esp_crypto_hash_alg_t alg, const unsigned char *key, int key_len);

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect. 
  *        Add data to hash calculation.
  *
  * @param ctz  Context pointer from esp_crypto_hash_init_t function.
  * @param data  Data buffer to add.
  * @param len  Length of the buffer.
  *
  */
typedef void * (*esp_crypto_hash_update_t)(esp_crypto_hash_t *ctx, const unsigned char *data, int len);

/**
  * @brief The crypto callback function used in wpa enterprise hash operation when connect. 
  *        Complete hash calculation.
  *
  * @param ctz  Context pointer from esp_crypto_hash_init_t function.
  * @param hash   Buffer for hash value or %NULL if caller is just freeing the hash
  *               context.
  * @param len  Pointer to length of the buffer or %NULL if caller is just freeing the
  *             hash context; on return, this is set to the actual length of the hash value
  *             Returns: 0 on success, -1 if buffer is too small (len set to needed length),
  *             or -2 on other failures (including failed crypto_hash_update() operations)
  *
  */
typedef int * (*esp_crypto_hash_finish_t)(esp_crypto_hash_t *ctx, unsigned char *hash, int *len);

/**
  * @brief The AES callback function when do WPS connect. 
  *
  * @param key  Encryption key.
  * @param iv  Encryption IV for CBC mode (16 bytes).
  * @param data  Data to encrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  */
typedef int * (*esp_aes_128_encrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES callback function when do WPS connect. 
  *
  * @param key  Decryption key.
  * @param iv  Decryption IV for CBC mode (16 bytes).
  * @param data  Data to decrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  *
  */
typedef int * (*esp_aes_128_decrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param kek  16-octet Key encryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param plain  Plaintext key to be wrapped, n * 64 bits
  * @param cipher  Wrapped key, (n + 1) * 64 bits
  *
  */
typedef int * (*esp_aes_wrap_t)(const unsigned char *kek, int n, const unsigned char *plain, unsigned char *cipher);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param kek  16-octet Key decryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param cipher  Wrapped key to be unwrapped, (n + 1) * 64 bits
  * @param plain  Plaintext key, n * 64 bits
  *
  */
typedef int * (*esp_aes_unwrap_t)(const unsigned char *kek, int n, const unsigned char *cipher, unsigned char *plain);

/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Initialize a esp_crypto_cipher_t structure.
  *
  * @param alg  cipher algorithm.
  * @param iv  Initialization vector for block ciphers or %NULL for stream ciphers.
  * @param key  Cipher key
  * @param key_len  Length of key in bytes
  *
  */
typedef esp_crypto_cipher_t * (*esp_crypto_cipher_init_t)(esp_crypto_cipher_alg_t alg, const unsigned char *iv, const unsigned char *key, int key_len);

/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Cipher encrypt.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  * @param plain  Plaintext to cipher.
  * @param crypt  Resulting ciphertext.
  * @param len  Length of the plaintext.
  *
  */
typedef int * (*esp_crypto_cipher_encrypt_t)(esp_crypto_cipher_t *ctx,
				             const unsigned char *plain, unsigned char *crypt, int len);
/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Cipher decrypt.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  * @param crypt  Ciphertext to decrypt.
  * @param plain  Resulting plaintext.
  * @param len  Length of the cipher text.
  *
  */
typedef int * (*esp_crypto_cipher_decrypt_t)(esp_crypto_cipher_t *ctx,
				             const unsigned char *crypt, unsigned char *plain, int len);
/**
  * @brief The crypto callback function used in wpa enterprise cipher operation when connect.
  *        Free cipher context.
  *
  * @param ctx  Context pointer from esp_crypto_cipher_init_t callback function.
  *
  */
typedef void * (*esp_crypto_cipher_deinit_t)(esp_crypto_cipher_t *ctx);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param key  Key for HMAC operations.
  * @param key_len  Length of the key in bytes.
  * @param data  Pointers to the data area.
  * @param data_len  Length of the data area.
  * @param mac  Buffer for the hash (20 bytes).
  *
  */
typedef void * (*esp_hmac_sha256_t)(const unsigned char *key, int key_len, const unsigned char *data,
		                    int data_len, unsigned char *mac);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param key  Key for HMAC operations.
  * @param key_len  Length of the key in bytes.
  * @param num_elem  Number of elements in the data vector.
  * @param addr  Pointers to the data areas.
  * @param len  Lengths of the data blocks.
  * @param mac  Buffer for the hash (32 bytes).
  *
  */
typedef void * (*esp_hmac_sha256_vector_t)(const unsigned char *key, int key_len, int num_elem,
			                   const unsigned char *addr[], const int *len, unsigned char *mac);

/**
  * @brief The AES callback function when do STA connect.
  *
  * @param key  Key for PRF.
  * @param key_len  Length of the key in bytes.
  * @param label  A unique label for each purpose of the PRF.
  * @param data  Extra data to bind into the key.
  * @param data_len  Length of the data.
  * @param buf  Buffer for the generated pseudo-random key.  
  * @param buf_len  Number of bytes of key to generate.
  *
  */
typedef void * (*esp_sha256_prf_t)(const unsigned char *key, int key_len, const char *label,
	                           const unsigned char *data, int data_len, unsigned char *buf, int buf_len);

/**
  * @brief The SHA256 callback function when do WPS connect. 
  *
  * @param num_elem  Number of elements in the data vector.
  * @param addr  Pointers to the data areas.
  * @param len  Lengths of the data blocks.
  * @param mac  Buffer for the hash.
  *
  */
typedef int * (*esp_sha256_vector_t)(int num_elem, const unsigned char *addr[], const int *len,
		                     unsigned char *mac);

/**
  * @brief The bignum calculate callback function used when do connect. 
  *        In WPS process, it used to calculate public key and private key.
  *
  * @param base  Base integer (big endian byte array).
  * @param base_len  Length of base integer in bytes.
  * @param power  Power integer (big endian byte array).
  * @param power_len  Length of power integer in bytes.
  * @param modulus  Modulus integer (big endian byte array).
  * @param modulus_len  Length of modulus integer in bytes.
  * @param result  Buffer for the result.
  * @param result_len  Result length (max buffer size on input, real len on output).
  *
  */
typedef int * (*esp_crypto_mod_exp_t)(const unsigned char *base, int base_len,
		                      const unsigned char *power, int power_len,
		                      const unsigned char *modulus, int modulus_len,
		                      unsigned char *result, unsigned int *result_len);
/**
  * @brief The crypto callback function structure used when do station security connect.
  *        The structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct {
    esp_aes_wrap_t aes_wrap;                         /**< station connect function used when send EAPOL frame */
    esp_aes_unwrap_t aes_unwrap;                     /**< station connect function used when decrypt key data */
    esp_hmac_sha256_vector_t hmac_sha256_vector;     /**< station connect function used when check MIC */
    esp_sha256_prf_t sha256_prf;                     /**< station connect function used when check MIC */
}wpa_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used when do WPS process. The 
  *        structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct{
    esp_aes_128_encrypt_t aes_128_encrypt;          /**< function used to process message when do WPS */
    esp_aes_128_decrypt_t aes_128_decrypt;          /**< function used to process message when do WPS */
    esp_crypto_mod_exp_t crypto_mod_exp;            /**< function used to calculate public key and private key */
    esp_hmac_sha256_t hmac_sha256;                  /**< function used to get attribute */
    esp_hmac_sha256_vector_t hmac_sha256_vector;    /**< function used to process message when do WPS */
    esp_sha256_vector_t sha256_vector;              /**< function used to process message when do WPS */
}wps_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used when do WPA enterprise connect.
  *        The structure can be set as software crypto or the crypto optimized by ESP32
  *        hardware.
  */
typedef struct {
    esp_crypto_hash_init_t crypto_hash_init;                  /**< function used to initialize a crypto_hash structure when use TLSV1 */
    esp_crypto_hash_update_t crypto_hash_update;              /**< function used to calculate hash data when use TLSV1 */
    esp_crypto_hash_finish_t crypto_hash_finish;              /**< function used to finish the hash calculate when use TLSV1 */
    esp_crypto_cipher_init_t crypto_cipher_init;              /**< function used to initialize a crypt_cipher structure when use TLSV1 */
    esp_crypto_cipher_encrypt_t crypto_cipher_encrypt;        /**< function used to encrypt cipher when use TLSV1 */
    esp_crypto_cipher_decrypt_t crypto_cipher_decrypt;        /**< function used to decrypt cipher when use TLSV1 */
    esp_crypto_cipher_deinit_t crypto_cipher_deinit;          /**< function used to free context when use TLSV1 */
    esp_sha256_vector_t sha256_vector;                        /**< function used to do X.509v3 certificate parsing and processing */
    esp_crypto_mod_exp_t crypto_mod_exp;                      /**< function used to do key exchange when use TLSV1 */
}wpa2_crypto_funcs_t;

#ifdef __cplusplus
}
#endif
#endif
