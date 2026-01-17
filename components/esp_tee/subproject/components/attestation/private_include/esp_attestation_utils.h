/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <sys/queue.h>
#include "esp_err.h"

#include "esp_flash_partitions.h"
#include "esp_app_desc.h"

#include "esp_attestation.h"
#include "psa/crypto.h"
#include "psa/initial_attestation.h"

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_ATT_TK_MAGIC      0x44FEF7CC
#define ESP_ATT_TK_MAGIC_STR  "44fef7cc"
#define ESP_ATT_TK_SIGN_ALG   "ecdsa_secp256r1_sha256"

#define SECP256R1_ECDSA_KEY_LEN  (32)
#define SECP256R1_ECDSA_SIG_LEN  (64)

#define MAX_ECDSA_KEY_LEN  (32)
#define MAX_ECDSA_SIG_LEN  (64)

#define SHA256_DIGEST_SZ  (32)
#define MAX_DIGEST_SZ     (32)

#define ESP_ATT_HDR_JSON_MAX_SZ     (128)

#define ESP_ATT_EAT_DEV_ID_SZ       (32)
#define ESP_ATT_CLAIM_JSON_MAX_SZ   (448)
#define ESP_ATT_EAT_JSON_MAX_SZ     (1344)

#define ESP_ATT_PUBKEY_JSON_MAX_SZ  (128)
#define ESP_ATT_SIGN_JSON_MAX_SZ    (192)

#define ESP_ATT_TK_MIN_SIZE         (ESP_ATT_HDR_JSON_MAX_SZ + ESP_ATT_EAT_JSON_MAX_SZ + ESP_ATT_PUBKEY_JSON_MAX_SZ + ESP_ATT_SIGN_JSON_MAX_SZ)
#if ESP_ATT_TK_MIN_SIZE > PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE
#error "Attestation token size may exceed the bounds set by the PSA interface"
#endif

#if ESP_TEE_BUILD && CONFIG_SECURE_TEE_ATTESTATION
#define ESP_ATT_TK_KEY_ID        (CONFIG_SECURE_TEE_ATT_KEY_STR_ID)
#define ESP_ATT_TK_PSA_CERT_REF  (CONFIG_SECURE_TEE_ATT_PSA_CERT_REF)
#else
#define ESP_ATT_TK_KEY_ID        ("NULL")
#define ESP_ATT_TK_PSA_CERT_REF  ("NULL")
#endif

/**
 * @brief Enumeration of partition types for attestation
 */
typedef enum {
    ESP_ATT_PART_TYPE_BOOTLOADER = 0,
    ESP_ATT_PART_TYPE_TEE        = 1,
    ESP_ATT_PART_TYPE_APP        = 2,
    ESP_ATT_PART_TYPE_OTHER      = 3,
    ESP_ATT_PART_TYPE_MAX        = 4,
} esp_att_part_type_t;

/**
 * @brief Enumeration of digest types for attestation
 */
typedef enum {
    ESP_ATT_DIGEST_TYPE_SHA256 = 0, /**< SHA-256 digest type */
} esp_att_part_digest_type_t;

/**
 * @brief Structure to hold digest information for a partition
 */
typedef struct {
    esp_att_part_digest_type_t type;     /**< Type of digest */
    uint8_t calc_digest[MAX_DIGEST_SZ];  /**< Calculated digest */
    bool digest_validated;               /**< Flag indicating if digest is validated */
    bool sign_verified;                  /**< Flag indicating if signature is verified */
    bool secure_padding;                 /**< Flag indicating if secure padding is present */
} esp_att_part_digest_info_t;

/**
 * @brief Structure to hold chip revision information
 */
typedef struct {
    uint16_t min_chip_rev;  /**< Minimum chip revision */
    uint16_t max_chip_rev;  /**< Maximum chip revision */
} esp_att_part_chip_rev_t;

/**
 * @brief Structure to hold EAT claim metadata for a partition
 */
typedef struct {
    esp_att_part_type_t type;                /**< Type of partition */
    char ver[32];                            /**< Version string */
    char idf_ver[32];                        /**< ESP-IDF version string */
    uint32_t secure_ver;                     /**< Secure version number */
    esp_att_part_chip_rev_t part_chip_rev;   /**< Chip revision information */
    esp_att_part_digest_info_t part_digest;  /**< Digest information */
} esp_att_part_metadata_t;

/**
 * @brief Structure to hold token header information
 */
typedef struct {
    uint32_t magic; /**< Magic number for token identification */
    char encr_alg[32]; /**< Encryption algorithm */
    char sign_alg[32]; /**< Signing algorithm */
    uint16_t key_id; /**< Key identifier */
} esp_att_token_hdr_t;

/**
 * @brief Structure to hold the Entity Attestation Token initial configuration
 */
typedef struct {
    uint8_t *auth_challenge;               /**< Authentication challenge */
    size_t challenge_size;                 /**< Challenge size */
    uint32_t client_id;                    /**< Client identifier (Attestation relying party) */
    uint32_t device_ver;                   /**< Device version */
    uint8_t device_id[SHA256_DIGEST_SZ];   /**< Device identifier */
    uint8_t instance_id[SHA256_DIGEST_SZ]; /**< Instance identifier */
    char psa_cert_ref[32];                 /**< PSA certificate reference */
    uint8_t device_stat;                   /**< Flags indicating device status */
} esp_att_token_cfg_t;

/**
 * @brief Structure to hold an ECDSA key pair
 */
typedef struct {
    uint32_t curve;                        /**< The elliptic curve used */
    uint8_t pvt_key[MAX_ECDSA_KEY_LEN];    /**< The private key */
    uint8_t pub_key_x[MAX_ECDSA_KEY_LEN];  /**< The x-coordinate of the public key */
    uint8_t pub_key_y[MAX_ECDSA_KEY_LEN];  /**< The y-coordinate of the public key */
} __attribute__((aligned(4))) __attribute__((__packed__)) esp_att_ecdsa_keypair_t;

/**
 * @brief Structure for linked list element of software claims
 */
typedef struct _esp_att_sw_claim_list {
    esp_att_part_metadata_t metadata; /**< Metadata for the partition */
    SLIST_ENTRY(_esp_att_sw_claim_list) next; /**< Pointer to next item in the list */
} esp_att_sw_claim_list_t;

/**
 * @brief Linked list of software claims
 */
struct esp_att_sw_claim_list {
    struct _esp_att_sw_claim_list *slh_first;
};

/**
 * @brief Convert a hexadecimal buffer to a hexadecimal string
 *
 * @param hexbuf      Input hexadecimal buffer
 * @param hexbuf_sz   Size of the input hexadecimal buffer
 * @param hexstr      Output hexadecimal string buffer
 * @param hexstr_len  Length of the output hexadecimal string buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_hexbuf_to_hexstr(const void *hexbuf, size_t hexbuf_sz, char *hexstr, size_t hexstr_len);

/**
 * @brief Get claim data for the bootloader
 *
 * @param btl_metadata Bootloader metadata output context
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_get_btl_claim_data(esp_att_part_metadata_t *btl_metadata);

/**
 * @brief Get claim data for the REE/user application
 *
 * @param app_metadata REE/user application metadata output context
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_get_app_claim_data(esp_att_part_metadata_t *app_metadata);

/**
 * @brief Get claim data for the TEE application
 *
 * @param tee_metadata TEE metadata output context
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_get_tee_claim_data(esp_att_part_metadata_t *tee_metadata);

/**
 * @brief Convert token header to JSON format
 *
 * @param tk_hdr       Token header structure
 * @param header_json  Output buffer to store the JSON string
 * @param len          Length of the generated JSON string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_header_to_json(const esp_att_token_hdr_t *tk_hdr, char **header_json, int *len);

/**
 * @brief Convert token claim data to JSON
 *
 * @param head         Software claim list
 * @param cfg          Token configuration
 * @param eat_json     Output buffer to store the JSON string
 * @param len          Length of the generated JSON string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_eat_data_to_json(struct esp_att_sw_claim_list *head, const esp_att_token_cfg_t *cfg, char **eat_json, int *len);

/**
 * @brief Convert token public key to JSON
 *
 * @param keypair      ECDSA key pair
 * @param pubkey_json  Output buffer to store the JSON string
 * @param len          Length of the generated JSON string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_pubkey_to_json(const esp_att_ecdsa_keypair_t *keypair, char **pubkey_json, int *len);

/**
 * @brief Convert token signature to JSON
 *
 * @param keypair      ECDSA key pair
 * @param digest       Digest to be signed
 * @param digest_len   Length of the digest
 * @param sign_json    Output buffer to store the JSON string
 * @param len          Length of the generated JSON string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_sign_to_json(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t digest_len, char **sign_json, int *len);

/**
 * @brief Generate an ECDSA key pair using the secp256r1 curve
 *
 * @param keypair Context to store the generated key pair
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_ecdsa_gen_keypair_secp256r1(esp_att_ecdsa_keypair_t *keypair);

/**
 * @brief Get ECDSA signature for a given digest
 *
 * @param keypair         ECDSA key pair
 * @param digest          Digest to be signed
 * @param len             Length of the digest
 * @param sign_r_hexstr   Output buffer holding the signature r component as a hex string
 * @param sign_s_hexstr   Output buffer holding the signature s component as a hex string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_ecdsa_get_sign(const esp_att_ecdsa_keypair_t *keypair, const uint8_t *digest, const size_t len,
                                       char **sign_r_hexstr, char **sign_s_hexstr);

/**
 * @brief Get the public key as a hex string
 *
 * @param keypair       ECDSA key pair
 * @param pubkey_hexstr Output buffer holding the public key as a hex string
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_ecdsa_get_pubkey(const esp_att_ecdsa_keypair_t *keypair, char **pubkey_hexstr);

/**
 * @brief Get the digest of the public key
 *
 * @param keypair  ECDSA key pair
 * @param digest   Digest buffer
 * @param len      Length of the digest buffer
 *
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t esp_att_utils_ecdsa_get_pubkey_digest(const esp_att_ecdsa_keypair_t *keypair, uint8_t *digest, const size_t len);

#ifdef __cplusplus
}
#endif
