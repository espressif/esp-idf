/*
 * SPDX-FileCopyrightText: 2017 Intel Corporation
 * SPDX-FileCopyrightText: 2023 Nordic Semiconductor ASA
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _BLE_MESH_CRYPTO_H_
#define _BLE_MESH_CRYPTO_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "sdkconfig.h"

#include "mesh/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Key sizes */
#define PRIV_KEY_SIZE   32
#define PUB_KEY_SIZE    64
#define DH_KEY_SIZE     32

/**
 * @brief Crypto module endianness definitions
 *
 * BLE Mesh protocol uses big-endian format for ECC operations.
 * These macros define whether the underlying crypto implementation
 * expects big-endian or little-endian input.
 *
 * - BT_MESH_CRYPTO_ENDIAN_BIG: Module expects big-endian (TinyCrypt, MbedTLS, PSA)
 * - BT_MESH_CRYPTO_ENDIAN_LITTLE: Module expects little-endian (future implementations)
 */
#define BT_MESH_CRYPTO_ENDIAN_BIG       0
#define BT_MESH_CRYPTO_ENDIAN_LITTLE    1

/**
 * @brief Current crypto module endianness
 *
 * This macro should be defined by the crypto implementation.
 * Default is big-endian (matching TinyCrypt, MbedTLS, PSA).
 */
#ifndef BT_MESH_CRYPTO_ENDIAN
#define BT_MESH_CRYPTO_ENDIAN   BT_MESH_CRYPTO_ENDIAN_BIG
#endif

/**
 * @brief Parameter conversion macros for endianness handling
 *
 * These macros automatically handle byte order conversion between
 * BLE Mesh's big-endian format and the crypto module's expected format.
 *
 * For public keys (64 bytes = X + Y), the X and Y coordinates must be
 * swapped separately, not as a single 64-byte block.
 */
#if (BT_MESH_CRYPTO_ENDIAN == BT_MESH_CRYPTO_ENDIAN_LITTLE)

/* Little-endian mode: need byte order conversion */
#define CRYPTO_PARAM_DECLARE(name, size) \
    uint8_t _##name[size]

#define CRYPTO_PARAM_IN(name, size) \
    sys_memcpy_swap(_##name, name, size)

#define CRYPTO_PARAM_OUT(name, size) \
    sys_memcpy_swap((uint8_t *)(name), _##name, size)

#define CRYPTO_PARAM(name) (_##name)

/* Public key (64 bytes) needs special handling: swap X and Y separately */
#define CRYPTO_PARAM_PUBKEY_IN(name) \
    do { \
        sys_memcpy_swap(_##name, name, 32); \
        sys_memcpy_swap(_##name + 32, (name) + 32, 32); \
    } while (0)

#define CRYPTO_PARAM_PUBKEY_OUT(name) \
    do { \
        sys_memcpy_swap((uint8_t *)(name), _##name, 32); \
        sys_memcpy_swap((uint8_t *)(name) + 32, _##name + 32, 32); \
    } while (0)

#else /* BT_MESH_CRYPTO_ENDIAN_BIG */

/* Big-endian mode: no conversion needed, use original parameters directly */
#define CRYPTO_PARAM_DECLARE(name, size) \
    (void)0

#define CRYPTO_PARAM_IN(name, size) \
    (void)0

#define CRYPTO_PARAM_OUT(name, size) \
    (void)0

#define CRYPTO_PARAM(name) (name)

#define CRYPTO_PARAM_PUBKEY_IN(name) \
    (void)0

#define CRYPTO_PARAM_PUBKEY_OUT(name) \
    (void)0

#endif /* BT_MESH_CRYPTO_ENDIAN */

/**
 * @brief Scatter-Gather data structure for cryptographic operations
 */
struct bt_mesh_sg {
    const void *data;
    size_t len;
};

/**
 * @brief Mesh key type enumeration
 */
enum bt_mesh_key_type {
    BT_MESH_KEY_TYPE_ECB,   /**< AES-ECB encryption/decryption */
    BT_MESH_KEY_TYPE_CCM,   /**< AES-CCM AEAD */
    BT_MESH_KEY_TYPE_CMAC,  /**< AES-CMAC */
    BT_MESH_KEY_TYPE_NET,   /**< Network key */
    BT_MESH_KEY_TYPE_APP,   /**< Application key */
    BT_MESH_KEY_TYPE_DEV,   /**< Device key */
};

/**
 * @brief Mesh key structure
 *
 * For TinyCrypt/MbedTLS: stores raw key bytes
 * For PSA: stores PSA key ID
 */
struct bt_mesh_key {
#if CONFIG_BT_SMP_CRYPTO_STACK_MBEDTLS && CONFIG_MBEDTLS_VER_4_X_SUPPORT
    uint32_t key;  /* PSA key ID */
#else
    uint8_t key[16];  /* Raw key bytes */
#endif
};

/**
 * @brief Initialize the cryptographic subsystem
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_crypto_init(void);

/**
 * @brief AES-ECB encryption (big-endian)
 *
 * @param key       Pointer to mesh key structure
 * @param plaintext 16-byte plaintext
 * @param enc_data  16-byte output buffer for encrypted data
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_encrypt(const struct bt_mesh_key *key, const uint8_t plaintext[16],
                    uint8_t enc_data[16]);

/**
 * @brief AES-CCM encryption
 *
 * @param key       Pointer to mesh key structure
 * @param nonce     13-byte nonce
 * @param plaintext Input plaintext
 * @param len       Length of plaintext
 * @param aad       Additional authenticated data
 * @param aad_len   Length of AAD
 * @param enc_data  Output buffer (must be len + mic_size bytes)
 * @param mic_size  Size of MIC (4, 8, or 16 bytes)
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_ccm_encrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *plaintext, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *enc_data, size_t mic_size);

/**
 * @brief AES-CCM decryption
 *
 * @param key       Pointer to mesh key structure
 * @param nonce     13-byte nonce
 * @param enc_data  Encrypted data with MIC
 * @param len       Length of plaintext (without MIC)
 * @param aad       Additional authenticated data
 * @param aad_len   Length of AAD
 * @param plaintext Output buffer for plaintext
 * @param mic_size  Size of MIC (4, 8, or 16 bytes)
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_ccm_decrypt(const struct bt_mesh_key *key, uint8_t nonce[13],
                        const uint8_t *enc_data, size_t len,
                        const uint8_t *aad, size_t aad_len,
                        uint8_t *plaintext, size_t mic_size);

/**
 * @brief AES-CCM encryption using raw key bytes
 *
 * @param key       16-byte raw key
 * @param nonce     13-byte nonce
 * @param plaintext Input plaintext
 * @param len       Length of plaintext
 * @param aad       Additional authenticated data
 * @param aad_len   Length of AAD
 * @param enc_data  Output buffer (must be len + mic_size bytes)
 * @param mic_size  Size of MIC (4, 8, or 16 bytes)
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_ccm_encrypt_raw_key(const uint8_t key[16], uint8_t nonce[13],
                                const uint8_t *plaintext, size_t len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *enc_data, size_t mic_size);

/**
 * @brief AES-CCM decryption using raw key bytes
 *
 * @param key       16-byte raw key
 * @param nonce     13-byte nonce
 * @param enc_data  Encrypted data with MIC
 * @param len       Length of plaintext (without MIC)
 * @param aad       Additional authenticated data
 * @param aad_len   Length of AAD
 * @param plaintext Output buffer for plaintext
 * @param mic_size  Size of MIC (4, 8, or 16 bytes)
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_ccm_decrypt_raw_key(const uint8_t key[16], uint8_t nonce[13],
                                const uint8_t *enc_data, size_t len,
                                const uint8_t *aad, size_t aad_len,
                                uint8_t *plaintext, size_t mic_size);

/**
 * @brief AES-CMAC using mesh key structure
 *
 * @param key    Pointer to mesh key structure
 * @param sg     Scatter-gather list of input data
 * @param sg_len Number of scatter-gather entries
 * @param mac    16-byte output buffer for MAC
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_aes_cmac_mesh_key(const struct bt_mesh_key *key,
                              struct bt_mesh_sg *sg, size_t sg_len,
                              uint8_t mac[16]);

/**
 * @brief AES-CMAC using raw key bytes
 *
 * @param key    16-byte raw key
 * @param sg     Scatter-gather list of input data
 * @param sg_len Number of scatter-gather entries
 * @param mac    16-byte output buffer for MAC
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_aes_cmac_raw_key(const uint8_t key[16], struct bt_mesh_sg *sg,
                             size_t sg_len, uint8_t mac[16]);

/**
 * @brief AES-CMAC with single data input (convenience wrapper)
 */
static inline int bt_mesh_aes_cmac_one(const uint8_t key[16], const void *m,
                                       size_t len, uint8_t mac[16])
{
    struct bt_mesh_sg sg = { m, len };
    return bt_mesh_aes_cmac_raw_key(key, &sg, 1, mac);
}

/**
 * @brief HMAC-SHA256 using raw key bytes
 *
 * @param key    32-byte raw key
 * @param sg     Scatter-gather list of input data
 * @param sg_len Number of scatter-gather entries
 * @param mac    32-byte output buffer for MAC
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_sha256_hmac_raw_key(const uint8_t key[32], struct bt_mesh_sg *sg,
                                size_t sg_len, uint8_t mac[32]);

/**
 * @brief HMAC-SHA256 with single data input (convenience wrapper)
 */
static inline int bt_mesh_sha256_hmac_one(const uint8_t key[32], const void *m,
                                          size_t len, uint8_t mac[32])
{
    struct bt_mesh_sg sg = { m, len };
    return bt_mesh_sha256_hmac_raw_key(key, &sg, 1, mac);
}

/* ============================================================================
 * Internal Low-Level ECC APIs
 * ============================================================================
 * These functions operate in the crypto module's native byte order.
 * Do NOT call these directly - use the public wrapper functions below.
 */
const uint8_t *bt_mesh_pub_key_get_raw(void);
void bt_mesh_set_private_key_raw(const uint8_t pri_key[32]);
bool bt_mesh_check_public_key_raw(const uint8_t key[64]);
int bt_mesh_dhkey_gen_raw(const uint8_t *pub_key, const uint8_t *priv_key,
                          uint8_t *dhkey);

/* ============================================================================
 * Public ECC APIs (with automatic endianness conversion)
 * ============================================================================
 * All public key and DHKey parameters use big-endian format (BLE Mesh protocol).
 * For little-endian crypto modules, conversion is handled automatically.
 */

/**
 * @brief Generate a new ECC public key pair
 *
 * Generates a new P-256 key pair for ECDH operations.
 * The private key is stored internally.
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_pub_key_gen(void);

/**
 * @brief Get the current public key (raw pointer)
 *
 * Returns pointer to the internal public key storage.
 * Note: The byte order depends on crypto module's native format.
 * Use bt_mesh_pub_key_copy() to get big-endian format.
 *
 * @return Pointer to 64-byte public key, or NULL if not ready
 */
#define bt_mesh_pub_key_get() bt_mesh_pub_key_get_raw()

/**
 * @brief Copy current public key to user buffer (big-endian output)
 *
 * Copies the public key to the provided buffer, converting to big-endian
 * format (BLE Mesh protocol format) if necessary.
 *
 * @param buf 64-byte output buffer for public key (big-endian output)
 *
 * @return 0 on success, -EAGAIN if public key not ready
 */
static inline int bt_mesh_pub_key_copy(uint8_t *buf)
{
    const uint8_t *key = bt_mesh_pub_key_get_raw();

    if (key == NULL || buf == NULL) {
        return -EAGAIN;
    }

#if (BT_MESH_CRYPTO_ENDIAN == BT_MESH_CRYPTO_ENDIAN_LITTLE)
    /* Swap X and Y coordinates separately to big-endian */
    sys_memcpy_swap(buf, key, 32);
    sys_memcpy_swap(buf + 32, key + 32, 32);
#else
    memcpy(buf, key, PUB_KEY_SIZE);
#endif

    return 0;
}

/**
 * @brief Set a custom private key
 *
 * @param pri_key 32-byte private key (big-endian, BLE Mesh format)
 */
static inline void bt_mesh_set_private_key(const uint8_t pri_key[32])
{
    CRYPTO_PARAM_DECLARE(pri_key, PRIV_KEY_SIZE);
    CRYPTO_PARAM_IN(pri_key, PRIV_KEY_SIZE);
    bt_mesh_set_private_key_raw(CRYPTO_PARAM(pri_key));
}

/**
 * @brief Check if a public key is valid
 *
 * Verifies that the given public key is a valid point on the P-256 curve.
 *
 * @param key 64-byte public key (X || Y, big-endian, BLE Mesh format)
 *
 * @return true if valid, false otherwise
 */
static inline bool bt_mesh_check_public_key(const uint8_t key[64])
{
    CRYPTO_PARAM_DECLARE(key, PUB_KEY_SIZE);
    CRYPTO_PARAM_PUBKEY_IN(key);
    return bt_mesh_check_public_key_raw(CRYPTO_PARAM(key));
}

/**
 * @brief Generate ECDH shared secret (DHKey)
 *
 * @param pub_key  64-byte remote public key (X || Y, big-endian, BLE Mesh format)
 * @param priv_key 32-byte private key (NULL to use internal key)
 * @param dhkey    32-byte output buffer for shared secret (big-endian output)
 *
 * @return 0 on success, negative error code otherwise
 */
static inline int bt_mesh_dhkey_gen(const uint8_t *pub_key, const uint8_t *priv_key,
                                    uint8_t *dhkey)
{
    int rc;
    CRYPTO_PARAM_DECLARE(pub_key, PUB_KEY_SIZE);
    CRYPTO_PARAM_DECLARE(priv_key, PRIV_KEY_SIZE);
    CRYPTO_PARAM_DECLARE(dhkey, DH_KEY_SIZE);

    /* Public key: swap X and Y coordinates separately */
    CRYPTO_PARAM_PUBKEY_IN(pub_key);
    if (priv_key != NULL) {
        CRYPTO_PARAM_IN(priv_key, PRIV_KEY_SIZE);
    }

    rc = bt_mesh_dhkey_gen_raw(CRYPTO_PARAM(pub_key),
                               priv_key ? CRYPTO_PARAM(priv_key) : NULL,
                               CRYPTO_PARAM(dhkey));
    if (rc) {
        return rc;
    }

    CRYPTO_PARAM_OUT(dhkey, DH_KEY_SIZE);
    return rc;
}

#define bt_mesh_dh_key_gen(pubkey, dhkey) bt_mesh_dhkey_gen(pubkey, NULL, dhkey)

/**
 * @brief Import a raw key into mesh key structure
 *
 * @param type Key type (determines key usage)
 * @param in   16-byte raw key
 * @param out  Output mesh key structure
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_key_import(enum bt_mesh_key_type type, const uint8_t in[16],
                       struct bt_mesh_key *out);

/**
 * @brief Export raw key bytes from mesh key structure
 *
 * @param out 16-byte output buffer
 * @param in  Input mesh key structure
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_key_export(uint8_t out[16], const struct bt_mesh_key *in);

/**
 * @brief Assign (copy) a mesh key
 *
 * @param dst Destination mesh key
 * @param src Source mesh key
 */
void bt_mesh_key_assign(struct bt_mesh_key *dst, const struct bt_mesh_key *src);

/**
 * @brief Destroy a mesh key
 *
 * Releases any resources associated with the key.
 *
 * @param key Mesh key to destroy
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_key_destroy(const struct bt_mesh_key *key);

/**
 * @brief Compare a raw key with a mesh key
 *
 * @param raw_key 16-byte raw key
 * @param key     Mesh key structure to compare
 *
 * @return 0 if equal, non-zero otherwise
 */
int bt_mesh_key_compare(const uint8_t raw_key[16], const struct bt_mesh_key *key);

/**
 * @brief AES encryption (little-endian byte order)
 *
 * The key and plaintext are byte-swapped before encryption,
 * and the result is byte-swapped after encryption.
 *
 * @param key       16-byte key
 * @param plaintext 16-byte plaintext
 * @param enc_data  16-byte output buffer
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_encrypt_le(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16]);

/**
 * @brief AES encryption (big-endian byte order)
 *
 * Standard AES-ECB encryption without byte swapping.
 *
 * @param key       16-byte key
 * @param plaintext 16-byte plaintext
 * @param enc_data  16-byte output buffer
 *
 * @return 0 on success, negative error code otherwise
 */
int bt_mesh_encrypt_be(const uint8_t key[16], const uint8_t plaintext[16],
                       uint8_t enc_data[16]);
#ifdef __cplusplus
}
#endif

#endif /* _BLE_MESH_CRYPTO_H_ */
