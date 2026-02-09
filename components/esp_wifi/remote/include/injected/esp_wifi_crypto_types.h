/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_WIFI_CRYPTO_TYPES_H__
#define __ESP_WIFI_CRYPTO_TYPES_H__

/* This is an internal API header for configuring the implementation used for WiFi cryptographic
   operations.

   During normal operation, you don't need to use any of these types or functions in this header.
   See esp_wifi.h & esp_wifi_types.h instead.
*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_WIFI_CRYPTO_VERSION 0x00000001

/*
 * Enumeration for hash operations.
 * When WPA2 is connecting, this enum is used to
 * request a hash algorithm via crypto_hash_xxx functions.
 */
typedef enum {
    ESP_CRYPTO_HASH_ALG_MD5, ESP_CRYPTO_HASH_ALG_SHA1,
    ESP_CRYPTO_HASH_ALG_HMAC_MD5, ESP_CRYPTO_HASH_ALG_HMAC_SHA1,
    ESP_CRYPTO_HASH_ALG_SHA256, ESP_CRYPTO_HASH_ALG_HMAC_SHA256
} esp_crypto_hash_alg_t;

/*
 * Enumeration for block cipher operations.
 * When WPA2 is connecting, this enum is used to request a block
 * cipher algorithm via crypto_cipher_xxx functions.
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
  * @brief The AES 128 encrypt callback function used by esp_wifi.
  *
  * @param key  Encryption key.
  * @param iv  Encryption IV for CBC mode (16 bytes).
  * @param data  Data to encrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  */
typedef int (*esp_aes_128_encrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES 128 decrypt callback function used by esp_wifi.
  *
  * @param key  Decryption key.
  * @param iv  Decryption IV for CBC mode (16 bytes).
  * @param data  Data to decrypt in-place.
  * @param data_len  Length of data in bytes (must be divisible by 16)
  *
  */
typedef int (*esp_aes_128_decrypt_t)(const unsigned char *key, const unsigned char *iv, unsigned char *data, int data_len);

/**
  * @brief The AES wrap callback function used by esp_wifi.
  *
  * @param kek  16-octet Key encryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param plain  Plaintext key to be wrapped, n * 64 bits
  * @param cipher  Wrapped key, (n + 1) * 64 bits
  *
  */
typedef int (*esp_aes_wrap_t)(const unsigned char *kek, int n, const unsigned char *plain, unsigned char *cipher);

/**
  * @brief The AES unwrap callback function used by esp_wifi.
  *
  * @param kek  16-octet Key decryption key (KEK).
  * @param n  Length of the plaintext key in 64-bit units;
  * @param cipher  Wrapped key to be unwrapped, (n + 1) * 64 bits
  * @param plain  Plaintext key, n * 64 bits
  *
  */
typedef int (*esp_aes_unwrap_t)(const unsigned char *kek, int n, const unsigned char *cipher, unsigned char *plain);

/**
  * @brief The SHA256 callback function used by esp_wifi.
  *
  * @param key  Key for HMAC operations.
  * @param key_len  Length of the key in bytes.
  * @param num_elem  Number of elements in the data vector.
  * @param addr  Pointers to the data areas.
  * @param len  Lengths of the data blocks.
  * @param mac  Buffer for the hash (32 bytes).
  *
  */
typedef int (*esp_hmac_sha256_vector_t)(const unsigned char *key, int key_len, int num_elem,
                                        const unsigned char *addr[], const int *len, unsigned char *mac);

/**
  * @brief The SHA256 PRF callback function used by esp_wifi.
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
typedef int (*esp_sha256_prf_t)(const unsigned char *key, int key_len, const char *label,
                                const unsigned char *data, int data_len, unsigned char *buf, int buf_len);

/**
 * @brief HMAC-MD5 callback function over data buffer (RFC 2104)'
 *
 * @param key Key for HMAC operations
 * @param key_len Length of the key in bytes
 * @param data Pointers to the data area
 * @param data_len Length of the data area
 * @param mac Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_md5_t)(const unsigned char *key, unsigned int key_len, const unsigned char *data,
                              unsigned int data_len, unsigned char *mac);

/**
 * @brief HMAC-MD5 callback function over data vector (RFC 2104)
 *
 * @param key Key for HMAC operations
 * @param key_len Length of the key in bytes
 * @param num_elem Number of elements in the data vector
 * @param addr Pointers to the data areas
 * @param len Lengths of the data blocks
 * @param mac Buffer for the hash (16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_md5_vector_t)(const unsigned char *key, unsigned int key_len, unsigned int num_elem,
                                     const unsigned char *addr[], const unsigned int *len, unsigned char *mac);

/**
 * @brief HMAC-SHA1 callback function over data buffer (RFC 2104)
 *
 * @param key Key for HMAC operations
 * @param key_len Length of the key in bytes
 * @param data Pointers to the data area
 * @param data_len Length of the data area
 * @param mac Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 of failure
 */
typedef int (*esp_hmac_sha1_t)(const unsigned char *key, unsigned int key_len, const unsigned char *data,
                               unsigned int data_len, unsigned char *mac);

/**
 * @brief HMAC-SHA1 callback function over data vector (RFC 2104)
 *
 * @param key Key for HMAC operations
 * @param key_len Length of the key in bytes
 * @param num_elem Number of elements in the data vector
 * @param addr Pointers to the data areas
 * @param len Lengths of the data blocks
 * @param mac Buffer for the hash (20 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_hmac_sha1_vector_t)(const unsigned char *key, unsigned int key_len, unsigned int num_elem,
                                      const unsigned char *addr[], const unsigned int *len, unsigned char *mac);

/**
 * @brief SHA1-based Pseudo-Random Function (PRF) (IEEE 802.11i, 8.5.1.1) callback function
 *
 * @param key Key for PRF
 * @param key_len Length of the key in bytes
 * @param label A unique label for each purpose of the PRF
 * @param data Extra data to bind into the key
 * @param data_len Length of the data
 * @param buf Buffer for the generated pseudo-random key
 * @param buf_len Number of bytes of key to generate
 * Returns: 0 on success, -1 of failure
 *
 * This function is used to derive new, cryptographically separate keys from a
 * given key (e.g., PMK in IEEE 802.11i).
 */
typedef int (*esp_sha1_prf_t)(const unsigned char *key, unsigned int key_len, const char *label,
                              const unsigned char *data, unsigned int data_len, unsigned char *buf, unsigned int buf_len);

/**
 * @brief SHA-1 hash callback function for data vector
 *
 * @param num_elem Number of elements in the data vector
 * @param addr Pointers to the data areas
 * @param len Lengths of the data blocks
 * @param mac Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_sha1_vector_t)(unsigned int num_elem, const unsigned char *addr[], const unsigned int *len,
                                 unsigned char *mac);

/**
 * @brief SHA1-based key derivation function (PBKDF2) callback function for IEEE 802.11i
 *
 * @param passphrase ASCII passphrase
 * @param ssid SSID
 * @param ssid_len SSID length in bytes
 * @param iterations Number of iterations to run
 * @param buf Buffer for the generated key
 * @param buflen Length of the buffer in bytes
 * Returns: 0 on success, -1 of failure
 *
 * This function is used to derive PSK for WPA-PSK. For this protocol,
 * iterations is set to 4096 and buflen to 32. This function is described in
 * IEEE Std 802.11-2004, Clause H.4. The main construction is from PKCS#5 v2.0.
 */
typedef int (*esp_pbkdf2_sha1_t)(const char *passphrase, const char *ssid, unsigned int ssid_len,
                                 int iterations, unsigned char *buf, unsigned int buflen);

/**
 * @brief XOR RC4 stream callback function to given data with skip-stream-start
 *
 * @param key RC4 key
 * @param keylen RC4 key length
 * @param skip number of bytes to skip from the beginning of the RC4 stream
 * @param data data to be XOR'ed with RC4 stream
 * @param data_len buf length
 * Returns: 0 on success, -1 on failure
 *
 * Generate RC4 pseudo random stream for the given key, skip beginning of the
 * stream, and XOR the end result with the data buffer to perform RC4
 * encryption/decryption.
 */
typedef int (*esp_rc4_skip_t)(const unsigned char *key, unsigned int keylen, unsigned int skip,
                              unsigned char *data, unsigned int data_len);

/**
 * @brief MD5 hash callback function for data vector
 *
 * @param num_elem Number of elements in the data vector
 * @param addr Pointers to the data areas
 * @param len Lengths of the data blocks
 * @param mac Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_md5_vector_t)(unsigned int num_elem, const unsigned char *addr[], const unsigned int *len,
                                unsigned char *mac);

/**
 * @brief Encrypt one AES block callback function
 *
 * @param ctx Context pointer from aes_encrypt_init()
 * @param plain Plaintext data to be encrypted (16 bytes)
 * @param crypt Buffer for the encrypted data (16 bytes)
 */
typedef void (*esp_aes_encrypt_t)(void *ctx, const unsigned char *plain, unsigned char *crypt);

/**
 * @brief Initialize AES callback function for encryption
 *
 * @param key Encryption key
 * @param len Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
typedef void * (*esp_aes_encrypt_init_t)(const unsigned char *key,  unsigned int len);

/**
 * @brief Deinitialize AES encryption callback function
 *
 * @param ctx Context pointer from aes_encrypt_init()
 */
typedef void (*esp_aes_encrypt_deinit_t)(void *ctx);

/**
 * @brief Decrypt one AES block callback function
 *
 * @param ctx Context pointer from aes_encrypt_init()
 * @param crypt Encrypted data (16 bytes)
 * @param plain Buffer for the decrypted data (16 bytes)
 */
typedef void (*esp_aes_decrypt_t)(void *ctx, const unsigned char *crypt, unsigned char *plain);

/**
 * @brief Initialize AES callback function for decryption
 *
 * @param key Decryption key
 * @param len Key length in bytes (usually 16, i.e., 128 bits)
 * Returns: Pointer to context data or %NULL on failure
 */
typedef void * (*esp_aes_decrypt_init_t)(const unsigned char *key, unsigned int len);

/**
 * @brief Deinitialize AES decryption callback function
 *
 * @param ctx Context pointer from aes_encrypt_init()
 */
typedef void (*esp_aes_decrypt_deinit_t)(void *ctx);

/**
 * @brief One-Key CBC MAC (OMAC1) hash with AES-128 callback function for MIC computation
 *
 * @param key 128-bit key for the hash operation
 * @param data Data buffer for which a MIC is computed
 * @param data_len Length of data buffer in bytes
 * @param mic Buffer for MIC (128 bits, i.e., 16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_omac1_aes_128_t)(const uint8_t *key, const uint8_t *data, size_t data_len,
                                   uint8_t *mic);

/**
 * @brief Decrypt data callback function using CCMP (Counter Mode CBC-MAC Protocol OR
 *        Counter Mode Cipher Block Chaining Message Authentication
 *        Code Protocol) which is used in IEEE 802.11i RSN standard.
 * @param tk 128-bit Temporal Key for obtained during 4-way handshake
 * @param ieee80211_hdr Pointer to IEEE802.11 frame headeri needed for AAD
 * @param data Pointer to encrypted data buffer
 * @param data_len Encrypted data length in bytes
 * @param decrypted_len Length of decrypted data
 * @param espnow_pkt Indicates if it's an ESPNOW packet
 * Returns: Pointer to decrypted data on success, NULL on failure
 */
typedef uint8_t * (*esp_ccmp_decrypt_t)(const uint8_t *tk, const uint8_t *ieee80211_hdr,
                                        const uint8_t *data, size_t data_len,
                                        size_t *decrypted_len, bool espnow_pkt);

/**
 * @brief Encrypt data callback function using CCMP (Counter Mode CBC-MAC Protocol OR
 *        Counter Mode Cipher Block Chaining Message Authentication
 *        Code Protocol) which is used in IEEE 802.11i RSN standard.
 * @param tk 128-bit Temporal Key for obtained during 4-way handshake
 * @param frame Pointer to IEEE802.11 frame including header
 * @param len Length of the frame including header
 * @param hdrlen Length of the header
 * @param pn Packet Number counter
 * @param keyid Key ID to be mentioned in CCMP Vector
 * @param encrypted_len Length of the encrypted frame including header
 */
typedef uint8_t * (*esp_ccmp_encrypt_t)(const uint8_t *tk, uint8_t *frame, size_t len, size_t hdrlen,
                                        uint8_t *pn, int keyid, size_t *encrypted_len);

/**
 * @brief One-Key GMAC hash callback function with AES for MIC computation
 *
 * @param key key for the hash operation
 * @param keylen key length
 * @param iv initialization vector
 * @param iv_len initialization vector length
 * @param aad aad
 * @param aad_len aad length
 * @param mic Buffer for MIC (128 bits, i.e., 16 bytes)
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_aes_gmac_t)(const uint8_t *key, size_t keylen, const uint8_t *iv, size_t iv_len,
                              const uint8_t *aad, size_t aad_len, uint8_t *mic);

/**
 * @brief SHA256 hash callback function for data vector
 * @param num_elem Number of elements in the data vector
 * @param addr Pointers to the data areas
 * @param len Lengths of the data blocks
 * @param buf Buffer for the hash
 * Returns: 0 on success, -1 on failure
 */
typedef int (*esp_sha256_vector_t)(size_t num_elem, const uint8_t *addr[], const size_t *len, uint8_t *buf);

/**
 * @brief CRC32 value callback function in little endian.
 *
 * @param crc Initial CRC value (result of last calculation or 0 for the first time)
 * @param buf Data buffer that used to calculate the CRC value
 * @param len Length of the data buffer
 * @return CRC32 value
 */
typedef uint32_t (*esp_crc32_le_t)(uint32_t crc, uint8_t const *buf, uint32_t len);

/**
  * @brief The crypto callback function structure used by esp_wifi.
  *        The structure can be set as software crypto or the crypto optimized by device's
  *        hardware.
  */
typedef struct wpa_crypto_funcs_t {
    uint32_t size;                                   /**< The crypto callback function structure size */
    uint32_t version;                                /**< The crypto callback function structure version */
    esp_hmac_sha256_vector_t hmac_sha256_vector;     /**< The SHA256 callback function used by esp_wifi */
    esp_pbkdf2_sha1_t pbkdf2_sha1;                   /**< SHA1-based key derivation function (PBKDF2) callback function for IEEE 802.11 */
    esp_aes_128_encrypt_t aes_128_encrypt;           /**< The AES 128 encrypt callback function used by esp_wifi */
    esp_aes_128_decrypt_t aes_128_decrypt;           /**< The AES 128 decrypt callback function used by esp_wifi */
    esp_omac1_aes_128_t omac1_aes_128;               /**< One-Key CBC MAC (OMAC1) hash with AES-128 callback function for MIC computation */
    esp_ccmp_decrypt_t ccmp_decrypt;                 /**< Decrypt data callback function using CCMP */
    esp_ccmp_encrypt_t ccmp_encrypt;                 /**< Encrypt data callback function using CCMP */
    esp_aes_gmac_t aes_gmac;                         /**< One-Key GMAC hash callback function with AES for MIC computation */
    esp_sha256_vector_t sha256_vector;               /**< SHA256 hash callback function for data vector */
} wpa_crypto_funcs_t;

/**
  * @brief The crypto callback function structure used in mesh vendor IE encryption. The
  *        structure can be set as software crypto or the crypto optimized by device's
  *        hardware.
  */
typedef struct {
    esp_aes_128_encrypt_t aes_128_encrypt;          /**< Callback function used in mesh vendor IE encryption */
    esp_aes_128_decrypt_t aes_128_decrypt;          /**< Callback function used in mesh vendor IE decryption */
} mesh_crypto_funcs_t;

#ifdef __cplusplus
}
#endif
#endif
