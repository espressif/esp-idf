/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * ESP ECDSA PSA Crypto Opaque Driver
 *
 * This driver provides PSA Crypto API integration for ESP ECDSA hardware.
 */

#include <string.h>
#include "soc/soc_caps.h"
#include "esp_log.h"

#include "mbedtls/ecp.h"
#include "mbedtls/bignum.h"

#include "mbedtls/platform_util.h"
#include "esp_assert.h"
#include "esp_fault.h"
#include "esp_crypto_lock.h"
#include "esp_crypto_periph_clk.h"

#include "esp_efuse.h"
#include "esp_efuse_chip.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include "esp_key_mgr.h"
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#if SOC_ECDSA_SUPPORTED
#include "hal/ecdsa_types.h"
#include "hal/ecdsa_hal.h"
#include "hal/ecdsa_ll.h"
#endif /* SOC_ECDSA_SUPPORTED */

#if SOC_ECC_SUPPORTED
#include "hal/ecc_ll.h"
#include "ecc_impl.h"
#endif /* SOC_ECC_SUPPORTED */

#if SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI
#include "hal/mpi_ll.h"
#endif /* SOC_MPI_SUPPORTED && SOC_ECDSA_USES_MPI */

#include "psa_crypto_driver_esp_ecdsa.h"
#include "include/psa_crypto_driver_esp_ecdsa_utilities.h"
#include "psa_crypto_driver_esp_opaque_common.h"
#include "psa_crypto_driver_wrappers_no_static.h"
#include "sdkconfig.h"

/* The log tag is used only by the opaque key paths below. The transparent driver
 * keeps its own tag in psa_crypto_driver_esp_ecdsa_transparent.c. */
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static const char *TAG = "psa_crypto_driver_esp_ecdsa";
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

/* Linkage of the sign helpers shared with the transparent driver. Only that driver
 * needs them outside this file. In any other build they stay static, so that the
 * compiler can inline each one into its one caller and fold constant arguments,
 * such as the max_attempts of esp_ecdsa_hw_sign(). That keeps opaque-only builds
 * no larger than before. */
#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
#define ESP_ECDSA_SHARED_LINKAGE
#else
#define ESP_ECDSA_SHARED_LINKAGE static
#endif /* ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
#include "esp_tee_sec_storage.h"
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

/*
 * Per-source storage structs — internal to the driver.
 * These are what actually get persisted to NVS, not the user-facing esp_ecdsa_opaque_key_t.
 *
 * All storage structs share a common prefix: [version][key_source][curve]
 * so that the driver can identify the key source at operation time.
 */
typedef enum {
    ESP_ECDSA_KEY_STORAGE_VERSION_INVALID = 0,
    ESP_ECDSA_KEY_STORAGE_VERSION_V1 = 1,
    ESP_ECDSA_KEY_STORAGE_VERSION_MAX = 2,
} esp_ecdsa_key_storage_version_t;

/* TEE secure storage key IDs are NVS key names, max 16 bytes including null terminator */
#define ESP_ECDSA_TEE_KEY_ID_MAX_LEN    16
ESP_STATIC_ASSERT(ESP_ECDSA_TEE_KEY_ID_MAX_LEN < UINT8_MAX, "ESP_ECDSA_TEE_KEY_ID_MAX_LEN must be less than 255 as we use uint8_t for tee_key_id_len");

typedef enum {
    ESP_ECDSA_KEY_SOURCE_EFUSE = 0,
    ESP_ECDSA_KEY_SOURCE_TEE = 1,
    ESP_ECDSA_KEY_SOURCE_KEY_MGR = 2,
} esp_ecdsa_key_source_t;

/* Storage structs use uint8_t for curve instead of esp_ecdsa_curve_t (enum)
 * to ensure stable serialized size across compilers. */

typedef struct __attribute__((packed)) {
    uint8_t version;
    uint8_t key_source;             /* esp_ecdsa_key_source_t */
    uint8_t curve;                  /* esp_ecdsa_curve_t */
} esp_ecdsa_common_key_storage_metadata_t;

ESP_STATIC_ASSERT(sizeof(esp_ecdsa_common_key_storage_metadata_t) == 3 * sizeof(uint8_t), "esp_ecdsa_common_key_storage_metadata_t structure must be exactly 3 bytes");

typedef struct __attribute__((packed)) {
    esp_ecdsa_common_key_storage_metadata_t metadata;
    uint8_t efuse_block;
} esp_ecdsa_efuse_key_storage_t;

typedef struct __attribute__((packed)) {
    esp_ecdsa_common_key_storage_metadata_t metadata;
    uint8_t tee_key_id_len;
    /* followed by: char tee_key_id[tee_key_id_len] */
} esp_ecdsa_tee_key_storage_t;

#if SOC_KEY_MANAGER_SUPPORTED
typedef struct {
    esp_ecdsa_common_key_storage_metadata_t metadata;
    uint8_t reserved;               /* explicit padding for alignment */
    esp_key_mgr_key_recovery_info_t key_recovery_info;
} esp_ecdsa_km_key_storage_t;

ESP_STATIC_ASSERT(offsetof(esp_ecdsa_km_key_storage_t, key_recovery_info) % 4 == 0,
                  "key_recovery_info must be 4-byte aligned in esp_ecdsa_km_key_storage_t");
#endif /* SOC_KEY_MANAGER_SUPPORTED */

/**
 * Pointer-based storage for volatile keys.
 * Embeds the user-facing opaque key struct (which contains pointers).
 * The caller must keep all referenced data valid until psa_destroy_key().
 */
typedef struct {
    esp_ecdsa_common_key_storage_metadata_t metadata;
    esp_ecdsa_opaque_key_t opaque_key;
} esp_ecdsa_volatile_key_storage_t;

/**
 * @brief Read the key_source tag from any storage struct.
 *
 * All storage structs share the layout: [version(1)][key_source(1)][...]
 */
static inline esp_ecdsa_key_source_t storage_get_key_source(const uint8_t *key_buffer)
{
    return (esp_ecdsa_key_source_t)key_buffer[1];
}

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
/**
 * @brief Calculate the storage buffer size required for a given user-facing opaque key.
 *
 * Inspects the user struct to determine the key source and returns the
 * corresponding storage struct size. Used by both the PSA size function
 * and for key_buffer_size validation at operation time.
 */
static size_t esp_ecdsa_get_storage_size(const esp_ecdsa_opaque_key_t *key, bool persistent)
{
    if (!persistent) {
        (void)key;
        return sizeof(esp_ecdsa_volatile_key_storage_t);
    }

    /* persistent: per-source inline storage */
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (key->tee_key_id) {
        size_t len = strnlen(key->tee_key_id, ESP_ECDSA_TEE_KEY_ID_MAX_LEN);
        return sizeof(esp_ecdsa_tee_key_storage_t) + len + 1;
    }
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

#if SOC_KEY_MANAGER_SUPPORTED
    if (key->key_recovery_info) {
        return sizeof(esp_ecdsa_km_key_storage_t);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    (void)key;
    return sizeof(esp_ecdsa_efuse_key_storage_t);
}

/**
 * @brief Calculate the expected storage size from an already-serialized storage buffer.
 *
 * Reads the key_source tag to determine the storage struct type and returns
 * the expected minimum buffer size. Used for key_buffer_size validation at
 * operation time (sign, export).
 */
static psa_status_t esp_ecdsa_get_expected_storage_size(const uint8_t *key_buffer, bool persistent, size_t *expected_storage_size)
{
    if (key_buffer[0] <= ESP_ECDSA_KEY_STORAGE_VERSION_INVALID || key_buffer[0] >= ESP_ECDSA_KEY_STORAGE_VERSION_MAX) {
        return PSA_ERROR_DATA_INVALID;
    }

    *expected_storage_size = 0;

    if (!persistent) {
        *expected_storage_size = sizeof(esp_ecdsa_volatile_key_storage_t);
        return PSA_SUCCESS;
    }

    /* persistent: dispatch by key_source */
    esp_ecdsa_key_source_t key_source = storage_get_key_source(key_buffer);

    switch (key_source) {
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
        case ESP_ECDSA_KEY_SOURCE_TEE:
            const esp_ecdsa_tee_key_storage_t *tee_st = (const esp_ecdsa_tee_key_storage_t *) key_buffer;
            if (tee_st->tee_key_id_len < 2 || tee_st->tee_key_id_len > ESP_ECDSA_TEE_KEY_ID_MAX_LEN) {
                return PSA_ERROR_DATA_INVALID;
            }
            *expected_storage_size = sizeof(esp_ecdsa_tee_key_storage_t) + tee_st->tee_key_id_len;
            return PSA_SUCCESS;
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
#if SOC_KEY_MANAGER_SUPPORTED
        case ESP_ECDSA_KEY_SOURCE_KEY_MGR:
            *expected_storage_size = sizeof(esp_ecdsa_km_key_storage_t);
            return PSA_SUCCESS;
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        case ESP_ECDSA_KEY_SOURCE_EFUSE:
            *expected_storage_size = sizeof(esp_ecdsa_efuse_key_storage_t);
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_DATA_INVALID;
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

/* Key lengths for different ECDSA curves */
#define ECDSA_KEY_LEN_P192          24
#define ECDSA_KEY_LEN_P256          32
#define ECDSA_KEY_LEN_P384          48

#define ECDSA_UNCOMPRESSED_POINT_FORMAT 0x04

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
#include "esp_timer.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"

#if CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH
/*
 * This is the maximum time (in us) required for performing 1 ECDSA signature
 * in this configuration along some additional margin considerations
 */
#define ECDSA_MAX_SIG_TIME 24000
#else /* CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */
#define ECDSA_MAX_SIG_TIME  17500
#endif /* !CONFIG_ESP_CRYPTO_DPA_PROTECTION_LEVEL_HIGH */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM
#define DUMMY_OP_COUNT ECDSA_SIGN_MAX_DUMMY_OP_COUNT
#else /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define DUMMY_OP_COUNT 0
#endif /* !CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_MASKING_CM */
#define ECDSA_CM_FIXED_SIG_TIME ECDSA_MAX_SIG_TIME * (DUMMY_OP_COUNT + 1)

#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM */

// Helper function to get curve from mbedtls group ID
esp_ecdsa_curve_t esp_ecdsa_bits_to_curve(size_t key_len)
{
    switch (key_len) {
        case ECDSA_KEY_LEN_P192:
            return ESP_ECDSA_CURVE_SECP192R1;
        case ECDSA_KEY_LEN_P256:
            return ESP_ECDSA_CURVE_SECP256R1;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ECDSA_KEY_LEN_P384:
            return ESP_ECDSA_CURVE_SECP384R1;
#endif
        default:
            return ESP_ECDSA_CURVE_MAX;
    }
}

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED) \
    || defined(ESP_ECDSA_VERIFY_DRIVER_ENABLED) || (CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN && SOC_ECDSA_SUPPORT_EXPORT_PUBKEY)
/**
 * @brief Map the driver's esp_ecdsa_curve_t enum to the HAL's ecdsa_curve_t enum.
 */
ecdsa_curve_t esp_ecdsa_curve_to_hal_curve(esp_ecdsa_curve_t curve)
{
    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1: return ECDSA_CURVE_SECP192R1;
        case ESP_ECDSA_CURVE_SECP256R1: return ECDSA_CURVE_SECP256R1;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1: return ECDSA_CURVE_SECP384R1;
#endif
        default: return (ecdsa_curve_t)-1;
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED || ESP_ECDSA_VERIFY_DRIVER_ENABLED || (CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN && SOC_ECDSA_SUPPORT_EXPORT_PUBKEY) */

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
static esp_tee_sec_storage_type_t esp_ecdsa_curve_to_tee_sec_storage_type(esp_ecdsa_curve_t curve)
{
    switch (curve) {
        case ESP_ECDSA_CURVE_SECP256R1:
            return ESP_SEC_STG_KEY_ECDSA_SECP256R1;
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
        case ESP_ECDSA_CURVE_SECP384R1:
            return ESP_SEC_STG_KEY_ECDSA_SECP384R1;
#endif
        default:
            return (esp_tee_sec_storage_type_t)-1;
        }
}
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

void esp_ecdsa_change_endianness(const uint8_t *old_point, uint8_t *new_point, uint8_t len)
{
    /* When the size is less than MAX_ECDSA_COMPONENT_LEN, it should be padded with 0 bytes*/
    memset(new_point, 0x0, len);

    for(int i = 0; i < len; i++) {
        new_point[i] = old_point[len - i - 1];
    }
}

psa_status_t esp_ecdsa_validate_sha_alg(psa_algorithm_t alg, const esp_ecdsa_curve_t curve)
{
    if ((PSA_ALG_SIGN_GET_HASH(alg) != PSA_ALG_SHA_256 && (curve == ESP_ECDSA_CURVE_SECP192R1 || curve == ESP_ECDSA_CURVE_SECP256R1))
#if SOC_ECDSA_SUPPORT_CURVE_P384
        || (PSA_ALG_SIGN_GET_HASH(alg) != PSA_ALG_SHA_384 && curve == ESP_ECDSA_CURVE_SECP384R1)
#endif
    ) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    return PSA_SUCCESS;
}

#if SOC_ECDSA_SUPPORTED

void esp_ecdsa_acquire_hardware(void)
{
    esp_crypto_ecdsa_lock_acquire();

    esp_crypto_ecdsa_enable_periph_clk(true);

    esp_crypto_ecc_enable_periph_clk(true);

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
    /*  Key Manager holds the key usage selector register (efuse vs own key).
        Thus, we need to enable the Key Manager peripheral clock to ensure
        that the key usage selector register is properly set.
        Taken after the ECDSA lock (which already holds SHA/AES and MPI) so
        the order matches HMAC/DS: sha_aes < mpi < key_manager.
     */
    esp_crypto_key_manager_lock_acquire();
    /* Clock only: a full KM reset would drop the XTS-AES flash encryption
       key-usage selector, and spi_flash DMA does not take the KM lock. */
    esp_crypto_key_mgr_enable_periph_clk_no_reset(true);
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
    /* We need to reset the MPI peripheral because ECDSA peripheral
     * of some targets use the MPI peripheral as well.
     */
        esp_crypto_mpi_enable_periph_clk(true);
    }
#endif /* SOC_ECDSA_USES_MPI */
}

void esp_ecdsa_release_hardware(void)
{
    esp_crypto_ecdsa_enable_periph_clk(false);

    esp_crypto_ecc_enable_periph_clk(false);

#if SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY
    esp_crypto_key_mgr_enable_periph_clk_no_reset(false);
    esp_crypto_key_manager_lock_release();
#endif /* SOC_KEY_MANAGER_ECDSA_KEY_DEPLOY */

#if SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_enable_periph_clk(false);
    }
#endif /* SOC_ECDSA_USES_MPI */

    esp_crypto_ecdsa_lock_release();
}

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
/* Pad one signature operation to a fixed time on the chip revisions that need it */
static inline void esp_ecdsa_pad_sign_time(uint64_t start_time)
{
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        uint64_t sig_time = esp_timer_get_time() - start_time;
        if (sig_time < ECDSA_CM_FIXED_SIG_TIME) {
            esp_rom_delay_us(ECDSA_CM_FIXED_SIG_TIME - sig_time);
        }
    }
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM */

/* The constant-time padding and the software deterministic loop below are guarded by
 * options that only the opaque path can enable today: the padding depends on
 * ESP32-H2, and the only target with a software key has the hardware deterministic
 * loop. Review both for the transparent path when a new target changes that. */
ESP_ECDSA_SHARED_LINKAGE psa_status_t esp_ecdsa_hw_sign(ecdsa_hal_config_t *conf, const uint8_t *sha,
                                                        uint8_t *r, uint8_t *s, uint16_t len,
                                                        unsigned int max_attempts, uint8_t *signature)
{
#if defined(ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED)
    /* Exported to the transparent driver, so do not trust len from the other file:
     * r, s, zeroes and signature are sized from MAX_ECDSA_COMPONENT_LEN. In other
     * builds the only caller is in this file and has already validated len. */
    if (len == 0 || len > MAX_ECDSA_COMPONENT_LEN) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }
#endif /* ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */

    const uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};
    bool process_again = false;
    unsigned int attempts = 0;

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
    uint16_t deterministic_loop_number __attribute__((unused)) = 1;
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

    esp_ecdsa_acquire_hardware();

    do {
#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (conf->sign_type == ECDSA_K_TYPE_DETERMINISITIC) {
            conf->loop_number = deterministic_loop_number++;
        }
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
        uint64_t sig_time = esp_timer_get_time();
#endif
        ecdsa_hal_gen_signature(conf, sha, r, s, len);
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN_CONSTANT_TIME_CM
        esp_ecdsa_pad_sign_time(sig_time);
#endif
        // Retry if the operation failed or produced a zero r or s
        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(r, zeroes, len)
                        || !memcmp(s, zeroes, len);

#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (conf->sign_type == ECDSA_K_TYPE_DETERMINISITIC) {
            process_again |= !ecdsa_hal_det_signature_k_check();
        }
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */
    } while (process_again && (max_attempts == 0 || ++attempts < max_attempts));

    esp_ecdsa_release_hardware();

    if (process_again) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    // Convert r and s from little-endian to big-endian and copy to output
    esp_ecdsa_change_endianness(r, signature, len);
    esp_ecdsa_change_endianness(s, signature + len, len);

    return PSA_SUCCESS;
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || ESP_ECDSA_TRANSPARENT_SIGN_DRIVER_ENABLED */

#endif /* SOC_ECDSA_SUPPORTED */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
#if SOC_ECDSA_SUPPORT_CURVE_P384
/**
 * @brief Check if the extracted efuse blocks are valid
 *
 * @param high_blk High efuse block number
 * @param low_blk Low efuse block number
 * @return true if both blocks are valid, false otherwise
 */
static bool is_efuse_blk_valid(int high_blk, int low_blk)
{
    return ((high_blk == 0 || (high_blk >= EFUSE_BLK_KEY0 && high_blk < EFUSE_BLK_KEY_MAX)) &&
            (low_blk >= EFUSE_BLK_KEY0 && low_blk < EFUSE_BLK_KEY_MAX));
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

/**
 * @brief Validate if the efuse block(s) have the appropriate ECDSA key purpose for the given curve
 *
 * This function validates that the provided efuse block(s) have been programmed with the appropriate
 * ECDSA key purpose for the specified curve type. It handles both curve-specific key purposes
 * (when SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES is defined) and generic ECDSA key purpose.
 *
 * For SECP384R1 curve, it checks for both high and low key blocks when supported.
 * For SECP192R1 and SECP256R1 curves, it validates the single block.
 *
 * @param[in] curve The ECDSA curve to validate the key purpose for
 * @param[in] efuse_blk The efuse block(s) to validate (can be combined for 384-bit keys)
 *
 * @return
 *      - 0 on success (block(s) have correct purpose)
 *      - PSA_ERROR_INVALID_ARGUMENT if input parameters are invalid
 *      - PSA_ERROR_NOT_PERMITTED if block(s) don't have appropriate key purpose
 */
static psa_status_t esp_ecdsa_validate_efuse_block(esp_ecdsa_curve_t curve, int efuse_blk)
{
    int low_blk = efuse_blk;
    esp_efuse_purpose_t expected_key_purpose_low;
#if SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P192) || EFUSE_LL_HAS_ECDSA_KEY_P192)
#if SOC_ECDSA_SUPPORT_CURVE_P384
    int high_blk;
    HAL_ECDSA_EXTRACT_KEY_BLOCKS(efuse_blk, high_blk, low_blk);
    if (!is_efuse_blk_valid(high_blk, low_blk)) {
        ESP_LOGE(TAG, "Invalid efuse block selected");
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    esp_efuse_purpose_t expected_key_purpose_high;
#endif

    switch (curve) {
        case ESP_ECDSA_CURVE_SECP192R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P192;
            break;
        case ESP_ECDSA_CURVE_SECP256R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P256;
            break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
        case ESP_ECDSA_CURVE_SECP384R1:
            expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_L;
            expected_key_purpose_high = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY_P384_H;
            break;
#endif
        default:
            ESP_LOGE(TAG, "Unsupported ECDSA curve: %ld", curve);
            return PSA_ERROR_INVALID_ARGUMENT;
    }
#else /* SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P192) || EFUSE_LL_HAS_ECDSA_KEY_P192) */
    expected_key_purpose_low = ESP_EFUSE_KEY_PURPOSE_ECDSA_KEY;
#endif  /* !SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES */

    if (expected_key_purpose_low != esp_efuse_get_key_purpose((esp_efuse_block_t)low_blk)) {
        ESP_LOGE(TAG, "Key burned in efuse has incorrect purpose");
        return PSA_ERROR_NOT_PERMITTED;
    }

#if SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P384) || EFUSE_LL_HAS_ECDSA_KEY_P384)
#if SOC_ECDSA_SUPPORT_CURVE_P384
    // Only check high block purpose for P384 curves that actually use it
    if (curve == ESP_ECDSA_CURVE_SECP384R1 &&
        expected_key_purpose_high != esp_efuse_get_key_purpose((esp_efuse_block_t)high_blk)) {
        ESP_LOGE(TAG, "Key burned in efuse has incorrect purpose for high block");
        return PSA_ERROR_NOT_PERMITTED;
    }
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
#endif /* SOC_ECDSA_SUPPORT_CURVE_SPECIFIC_KEY_PURPOSES && ((!defined(CONFIG_IDF_TARGET_ESP32P4) && SOC_EFUSE_ECDSA_KEY_P384) || EFUSE_LL_HAS_ECDSA_KEY_P384) */

    return PSA_SUCCESS;
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN || CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN

/**
 * @brief Validate the user-facing opaque key struct at import time.
 */
static psa_status_t validate_ecdsa_opaque_key_attributes(const psa_key_attributes_t *attributes, const esp_ecdsa_opaque_key_t *opaque_key)
{
    psa_key_type_t key_type = psa_get_key_type(attributes);
    if (!PSA_KEY_TYPE_IS_ECC_KEY_PAIR(key_type) ||
        PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) != PSA_ECC_FAMILY_SECP_R1) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    esp_ecdsa_curve_t expected_curve = esp_ecdsa_bits_to_curve(PSA_BITS_TO_BYTES(psa_get_key_bits(attributes)));

    if (expected_curve == ESP_ECDSA_CURVE_MAX || expected_curve != opaque_key->curve) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
    if (!(0
#if SOC_KEY_MANAGER_SUPPORTED
        || opaque_key->key_recovery_info
#endif /* SOC_KEY_MANAGER_SUPPORTED */
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
        || opaque_key->tee_key_id
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    )) {
        psa_status_t status = esp_ecdsa_validate_efuse_block(opaque_key->curve, opaque_key->efuse_block);
        if (status != PSA_SUCCESS) {
            return status;
        }
    }
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
    return PSA_SUCCESS;
}

/**
 * @brief Validate the stored curve against PSA key attributes at operation time.
 *
 * This is called when reading from a per-source storage struct (after import/NVS load)
 * to detect NVS corruption or wrong-blob scenarios.
 */
static psa_status_t validate_storage_curve(const psa_key_attributes_t *attributes, esp_ecdsa_curve_t stored_curve)
{
    psa_key_type_t key_type = psa_get_key_type(attributes);
    if (!PSA_KEY_TYPE_IS_ECC(key_type) ||
        PSA_KEY_TYPE_ECC_GET_FAMILY(key_type) != PSA_ECC_FAMILY_SECP_R1) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
    esp_ecdsa_curve_t expected_curve = esp_ecdsa_bits_to_curve(PSA_BITS_TO_BYTES(psa_get_key_bits(attributes)));
    if (expected_curve == ESP_ECDSA_CURVE_MAX || expected_curve != stored_curve) {
        ESP_LOGE(TAG, "Invalid curve expected");
        return PSA_ERROR_INVALID_ARGUMENT;
    }
    return PSA_SUCCESS;
}

/**
 * @brief Extract curve from a storage struct in key_buffer.
 *
 * All storage structs share the layout: [version(1)] [key_source(1)] [curve(1)].
 * This helper reads the curve from the common prefix.
 */
static esp_ecdsa_curve_t storage_get_curve(const uint8_t *key_buffer)
{
    return (esp_ecdsa_curve_t)key_buffer[2];
}

/**
 * @brief Read the eFuse block id from an eFuse-source storage struct.
 *
 * A volatile key keeps the user struct inline, a persistent key keeps the
 * compact eFuse layout. This helper hides that difference.
 */
__attribute__((unused))
static uint8_t storage_get_efuse_block(const uint8_t *key_buffer, bool is_persistent)
{
    if (!is_persistent) {
        return ((const esp_ecdsa_volatile_key_storage_t *)key_buffer)->opaque_key.efuse_block;
    }
    return ((const esp_ecdsa_efuse_key_storage_t *)key_buffer)->efuse_block;
}

#if SOC_KEY_MANAGER_SUPPORTED
/**
 * @brief Read the Key Manager recovery info from a Key-Manager-source storage struct.
 */
__attribute__((unused))
static const esp_key_mgr_key_recovery_info_t *storage_get_km_recovery_info(const uint8_t *key_buffer,
                                                                          bool is_persistent)
{
    if (!is_persistent) {
        return ((const esp_ecdsa_volatile_key_storage_t *)key_buffer)->opaque_key.key_recovery_info;
    }
    return &((const esp_ecdsa_km_key_storage_t *)key_buffer)->key_recovery_info;
}
#endif /* SOC_KEY_MANAGER_SUPPORTED */

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
/**
 * @brief Read the TEE secure storage key id from a TEE-source storage struct.
 */
static const char *storage_get_tee_key_id(const uint8_t *key_buffer, bool is_persistent)
{
    if (!is_persistent) {
        return ((const esp_ecdsa_volatile_key_storage_t *)key_buffer)->opaque_key.tee_key_id;
    }
    return (const char *)(key_buffer + sizeof(esp_ecdsa_tee_key_storage_t));
}
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */

/**
 * @brief Validate a serialized opaque key buffer and read back its common fields.
 *
 * The sign and export-public-key entry points share this preamble. It checks the
 * buffer size against the layout that the key_source tag names, and it validates
 * the stored curve against the PSA key attributes.
 *
 * @param[in]  attributes      PSA key attributes of the opaque key
 * @param[in]  key_buffer      Serialized storage struct held in the PSA key slot
 * @param[in]  key_buffer_size Size of key_buffer in bytes
 * @param[out] is_persistent   true if the key slot is persistent
 * @param[out] curve           Curve read from the storage struct
 *
 * @return PSA_SUCCESS on success, a PSA error code otherwise
 */
static psa_status_t esp_ecdsa_open_opaque_key(const psa_key_attributes_t *attributes,
                                              const uint8_t *key_buffer, size_t key_buffer_size,
                                              bool *is_persistent, esp_ecdsa_curve_t *curve)
{
    *is_persistent = esp_opaque_key_is_persistent(attributes);

    if (key_buffer_size < sizeof(esp_ecdsa_efuse_key_storage_t)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t expected_storage_size = 0;
    psa_status_t status = esp_ecdsa_get_expected_storage_size(key_buffer, *is_persistent,
                                                             &expected_storage_size);
    if (status != PSA_SUCCESS) {
        return status;
    }

    if (key_buffer_size < expected_storage_size) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    *curve = storage_get_curve(key_buffer);
    return validate_storage_curve(attributes, *curve);
}

psa_status_t esp_ecdsa_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    if (!attributes || !data || data_length < sizeof(esp_ecdsa_opaque_key_t) || !key_buffer || !key_buffer_length || !bits) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    const esp_ecdsa_opaque_key_t *opaque_key = (const esp_ecdsa_opaque_key_t *) data;
    psa_status_t status = validate_ecdsa_opaque_key_attributes(attributes, opaque_key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    /* Determine key source from user struct */
    esp_ecdsa_key_source_t key_source = ESP_ECDSA_KEY_SOURCE_EFUSE;
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (opaque_key->tee_key_id) {
        key_source = ESP_ECDSA_KEY_SOURCE_TEE;
    } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
#if SOC_KEY_MANAGER_SUPPORTED
    if (opaque_key->key_recovery_info) {
        key_source = ESP_ECDSA_KEY_SOURCE_KEY_MGR;
    } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    {
        key_source = ESP_ECDSA_KEY_SOURCE_EFUSE;
    }

    if (!is_persistent) {
        /* Volatile: store pointers -- caller keeps data alive */
        if (key_buffer_size < sizeof(esp_ecdsa_volatile_key_storage_t)) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        esp_ecdsa_volatile_key_storage_t *storage = (esp_ecdsa_volatile_key_storage_t *)key_buffer;
        storage->metadata.version = ESP_ECDSA_KEY_STORAGE_VERSION_V1;
        storage->metadata.key_source = key_source;
        storage->metadata.curve = (uint8_t)opaque_key->curve;
        memcpy(&storage->opaque_key, opaque_key, sizeof(esp_ecdsa_opaque_key_t));
        *key_buffer_length = sizeof(esp_ecdsa_volatile_key_storage_t);
    } else {
        /* Persistent: per-source inline storage */
#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
        if (key_source == ESP_ECDSA_KEY_SOURCE_TEE) {
            size_t str_len = strnlen(opaque_key->tee_key_id, ESP_ECDSA_TEE_KEY_ID_MAX_LEN);
            if (str_len == 0 || str_len >= ESP_ECDSA_TEE_KEY_ID_MAX_LEN) {
                return PSA_ERROR_INVALID_ARGUMENT;
            }
            uint8_t id_len = (uint8_t)(str_len + 1); /* include null terminator */
            size_t required_len = sizeof(esp_ecdsa_tee_key_storage_t) + id_len;
            if (key_buffer_size < required_len) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            esp_ecdsa_tee_key_storage_t *storage = (esp_ecdsa_tee_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_ECDSA_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_ECDSA_KEY_SOURCE_TEE;
            storage->metadata.curve = (uint8_t)opaque_key->curve;
            storage->tee_key_id_len = id_len;
            memcpy((uint8_t *)storage + sizeof(esp_ecdsa_tee_key_storage_t), opaque_key->tee_key_id, str_len);
            /* Explicitly null-terminate */
            *((uint8_t *)storage + sizeof(esp_ecdsa_tee_key_storage_t) + str_len) = '\0';
            *key_buffer_length = required_len;
        } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
#if SOC_KEY_MANAGER_SUPPORTED
        if (key_source == ESP_ECDSA_KEY_SOURCE_KEY_MGR) {
            if (key_buffer_size < sizeof(esp_ecdsa_km_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }
            esp_ecdsa_km_key_storage_t *storage = (esp_ecdsa_km_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_ECDSA_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_ECDSA_KEY_SOURCE_KEY_MGR;
            storage->metadata.curve = (uint8_t)opaque_key->curve;
            storage->reserved = 0;
            memcpy(&storage->key_recovery_info, opaque_key->key_recovery_info,
                   sizeof(esp_key_mgr_key_recovery_info_t));
            *key_buffer_length = sizeof(esp_ecdsa_km_key_storage_t);
        } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        {
            if (key_buffer_size < sizeof(esp_ecdsa_efuse_key_storage_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            esp_ecdsa_efuse_key_storage_t *storage = (esp_ecdsa_efuse_key_storage_t *)key_buffer;
            storage->metadata.version = ESP_ECDSA_KEY_STORAGE_VERSION_V1;
            storage->metadata.key_source = ESP_ECDSA_KEY_SOURCE_EFUSE;
            storage->metadata.curve = (uint8_t)opaque_key->curve;
            storage->efuse_block = opaque_key->efuse_block;
            *key_buffer_length = sizeof(esp_ecdsa_efuse_key_storage_t);
        }
    }

    *bits = psa_get_key_bits(attributes);
    return PSA_SUCCESS;
}

ESP_ECDSA_SHARED_LINKAGE psa_status_t esp_ecdsa_check_sign_request(psa_algorithm_t alg, esp_ecdsa_curve_t curve,
                                                                   size_t hash_length)
{
    psa_status_t status = esp_ecdsa_validate_sha_alg(alg, curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* An eFuse can permanently disable a curve on a given chip. Reject a
     * disabled curve on its own, not as part of the hash length check. */
#if SOC_ECDSA_SUPPORTED
    if ((curve == ESP_ECDSA_CURVE_SECP192R1 && !esp_efuse_is_ecdsa_p192_curve_supported())
        || (curve == ESP_ECDSA_CURVE_SECP256R1 && !esp_efuse_is_ecdsa_p256_curve_supported())) {
        return PSA_ERROR_NOT_SUPPORTED;
    }
#endif /* SOC_ECDSA_SUPPORTED */

    /* esp_ecdsa_validate_sha_alg pinned the hash algorithm to the curve, so the
     * expected digest length follows from the curve. PSA does not force
     * hash_length to match the algorithm, so the length is checked here. */
    if (hash_length != esp_ecdsa_expected_hash_len(curve)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_sign_hash_start(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length)
{
    if (!operation || !attributes || !key_buffer || !hash) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    // Check if ECDSA algorithm
    if (!PSA_ALG_IS_ECDSA(alg)) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    bool is_persistent = false;
    esp_ecdsa_curve_t curve = ESP_ECDSA_CURVE_MAX;
    psa_status_t status = esp_ecdsa_open_opaque_key(attributes, key_buffer, key_buffer_size,
                                                   &is_persistent, &curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    status = esp_ecdsa_check_sign_request(alg, curve, hash_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

    size_t component_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));

    memset(operation, 0, sizeof(esp_ecdsa_opaque_sign_hash_operation_t));
    operation->is_persistent = is_persistent;

    operation->key_len = component_len;

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    /* Determine hash endianness based on key source */
    esp_ecdsa_key_source_t key_source = storage_get_key_source(key_buffer);
    if (key_source == ESP_ECDSA_KEY_SOURCE_TEE) {
        // The ESP-TEE API expects the hash in big endian order
        memcpy(operation->sha, hash, component_len);
    } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
        esp_ecdsa_change_endianness(hash, operation->sha, component_len);
    }

    operation->key_buffer = key_buffer;
    operation->alg = alg;
    operation->sha_len = hash_length;

    return PSA_SUCCESS;
}

#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
/* Sign with an opaque key held in eFuse or in the Key Manager */
static psa_status_t esp_ecdsa_opaque_sign_with_peripheral(esp_ecdsa_opaque_sign_hash_operation_t *operation,
                                                          esp_ecdsa_key_source_t key_source __attribute__((unused)),
                                                          uint8_t *signature)
{
    // Check if the ECDSA peripheral is supported on this chip revision
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGE(TAG, "ECDSA peripheral not supported on this chip revision");
        return PSA_ERROR_NOT_SUPPORTED;
    }

    ecdsa_curve_t hal_curve = esp_ecdsa_curve_to_hal_curve(storage_get_curve(operation->key_buffer));
    if (hal_curve == (ecdsa_curve_t)-1) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    ecdsa_sign_type_t k_type = ECDSA_K_TYPE_TRNG;

    if (PSA_ALG_ECDSA_IS_DETERMINISTIC(operation->alg)) {
#if CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
        if (!ecdsa_ll_is_deterministic_mode_supported()) {
            return PSA_ERROR_NOT_SUPPORTED;
        }
        k_type = ECDSA_K_TYPE_DETERMINISITIC;
#else
        /* Without HW/SW deterministic-ECDSA support, do not silently downgrade
         * a deterministic-alg request to randomized. */
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* CONFIG_MBEDTLS_ECDSA_DETERMINISTIC && SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
    }

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .curve = hal_curve,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .sign_type = k_type,
    };

#if SOC_KEY_MANAGER_SUPPORTED
    const esp_key_mgr_key_recovery_info_t *key_recovery_info = NULL;
    if (key_source == ESP_ECDSA_KEY_SOURCE_KEY_MGR) {
        key_recovery_info = storage_get_km_recovery_info(operation->key_buffer, operation->is_persistent);
        esp_err_t err = esp_key_mgr_activate_key((esp_key_mgr_key_recovery_info_t *)key_recovery_info);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
            return PSA_ERROR_INVALID_HANDLE;
        }
        conf.use_km_key = 1;
    } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
    {
        conf.efuse_key_blk = storage_get_efuse_block(operation->key_buffer, operation->is_persistent);
    }

    /* No bound on the attempts: the opaque path keeps retrying until the peripheral succeeds */
    psa_status_t status = esp_ecdsa_hw_sign(&conf, operation->sha, operation->r, operation->s,
                                            operation->key_len, 0, signature);

#if SOC_KEY_MANAGER_SUPPORTED
    if (key_recovery_info) {
        esp_key_mgr_deactivate_key(key_recovery_info->key_type);
    }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

    return status;
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */

psa_status_t esp_ecdsa_opaque_sign_hash_complete(
    esp_ecdsa_opaque_sign_hash_operation_t *operation,
    uint8_t *signature, size_t signature_size,
    size_t *signature_length)
{
    if (!operation || !signature || !signature_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    size_t component_len = operation->key_len;

    if (signature_size < 2 * component_len) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    esp_ecdsa_key_source_t key_source __attribute__((unused)) = storage_get_key_source(operation->key_buffer);

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (key_source == ESP_ECDSA_KEY_SOURCE_TEE) {
        /* TEE key path */
        if (PSA_ALG_ECDSA_IS_DETERMINISTIC(operation->alg)) {
            ESP_LOGW(TAG, "Deterministic ECDSA unsupported for TEE keys; using randomized nonce");
        }

        const char *tee_key_id = storage_get_tee_key_id(operation->key_buffer, operation->is_persistent);
        esp_ecdsa_curve_t stored_curve = storage_get_curve(operation->key_buffer);

        esp_tee_sec_storage_type_t tee_sec_storage_type = esp_ecdsa_curve_to_tee_sec_storage_type(stored_curve);
        if (tee_sec_storage_type == (esp_tee_sec_storage_type_t) -1) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        esp_tee_sec_storage_key_cfg_t cfg = {
            .id = tee_key_id,
            .type = tee_sec_storage_type
        };

        esp_tee_sec_storage_ecdsa_sign_t sign = {};
        esp_err_t err = esp_tee_sec_storage_ecdsa_sign(&cfg, operation->sha, operation->sha_len, &sign);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get signature: 0x%08lx", err);
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        memcpy(signature, sign.signature, 2 * component_len);

    } else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
#if CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN
        psa_status_t status = esp_ecdsa_opaque_sign_with_peripheral(operation, key_source, signature);
        if (status != PSA_SUCCESS) {
            return status;
        }
#else
        // This is an invalid operation as the hardware ECDSA signing is not supported on this chip
        // and still the key is opaque.
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
    }

    *signature_length = 2 * component_len;

    return PSA_SUCCESS;
}

psa_status_t esp_ecdsa_opaque_sign_hash_abort(esp_ecdsa_opaque_sign_hash_operation_t *operation)
{
    if (operation) {
        mbedtls_platform_zeroize(operation, sizeof(esp_ecdsa_opaque_sign_hash_operation_t));
    }
    return PSA_SUCCESS;
}


psa_status_t esp_ecdsa_opaque_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *hash,
    size_t hash_length,
    uint8_t *signature,
    size_t signature_size,
    size_t *signature_length)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    esp_ecdsa_opaque_sign_hash_operation_t operation;

    status = esp_ecdsa_opaque_sign_hash_start(&operation, attributes, key_buffer, key_buffer_size, alg, hash, hash_length);
    if (status != PSA_SUCCESS) {
        esp_ecdsa_opaque_sign_hash_abort(&operation);
        return status;
    }

    status = esp_ecdsa_opaque_sign_hash_complete(&operation, signature, signature_size, signature_length);

    esp_ecdsa_opaque_sign_hash_abort(&operation);
    return status;
}

psa_status_t esp_ecdsa_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length)
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    if (!attributes || !key_buffer || !data || !data_length) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    bool is_persistent = false;
    esp_ecdsa_curve_t curve = ESP_ECDSA_CURVE_MAX;
    status = esp_ecdsa_open_opaque_key(attributes, key_buffer, key_buffer_size,
                                      &is_persistent, &curve);
    if (status != PSA_SUCCESS) {
        return status;
    }

    esp_ecdsa_key_source_t key_source __attribute__((unused)) = storage_get_key_source(key_buffer);

    // Get curve parameters from attributes
    size_t key_len = PSA_BITS_TO_BYTES(psa_get_key_bits(attributes));

    // As the ESP ECDSA driver only supports uncompressed point format,
    // the public key is exported in uncompressed point format (ECDSA_UNCOMPRESSED_POINT_FORMAT || x || y)
    // Need 1 byte for format + key_len bytes for x + key_len bytes for y
    size_t required_size = 1 + (2 * key_len);
    if (data_size < required_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

#if CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN
    if (key_source == ESP_ECDSA_KEY_SOURCE_TEE) {
        /* TEE key path */
        const char *tee_key_id = storage_get_tee_key_id(key_buffer, is_persistent);

        esp_tee_sec_storage_type_t tee_sec_storage_type = esp_ecdsa_curve_to_tee_sec_storage_type(curve);
        if (tee_sec_storage_type == (esp_tee_sec_storage_type_t) -1) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        esp_tee_sec_storage_key_cfg_t cfg = {
            .id = tee_key_id,
            .type = tee_sec_storage_type,
        };

        esp_tee_sec_storage_ecdsa_pubkey_t pubkey;
        if (esp_tee_sec_storage_ecdsa_get_pubkey(&cfg, &pubkey) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to get public key from secure storage");
            return PSA_ERROR_GENERIC_ERROR;
        }

        data[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;
        memcpy(data + 1, pubkey.pub_x, key_len);
        memcpy(data + 1 + key_len, pubkey.pub_y, key_len);
    }
    else
#endif /* CONFIG_MBEDTLS_TEE_SEC_STG_ECDSA_SIGN */
    {
#if SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
        // Check if the ECDSA peripheral is supported on this chip revision
        if (!ecdsa_ll_is_supported()) {
            ESP_LOGE(TAG, "ECDSA peripheral not supported on this chip revision");
            return PSA_ERROR_NOT_SUPPORTED;
        }

        uint8_t zeroes[MAX_ECDSA_COMPONENT_LEN] = {0};
        uint8_t qx[MAX_ECDSA_COMPONENT_LEN];
        uint8_t qy[MAX_ECDSA_COMPONENT_LEN];

        ecdsa_curve_t hal_curve = esp_ecdsa_curve_to_hal_curve(curve);
        if (hal_curve == (ecdsa_curve_t)-1) {
            return PSA_ERROR_INVALID_ARGUMENT;
        }

        ecdsa_hal_config_t conf = {
            .mode = ECDSA_MODE_EXPORT_PUBKEY,
            .curve = hal_curve,
        };

#if SOC_KEY_MANAGER_SUPPORTED
        const esp_key_mgr_key_recovery_info_t *key_recovery_info = NULL;
        if (key_source == ESP_ECDSA_KEY_SOURCE_KEY_MGR) {
            key_recovery_info = storage_get_km_recovery_info(key_buffer, is_persistent);
            esp_err_t err = esp_key_mgr_activate_key((esp_key_mgr_key_recovery_info_t *)key_recovery_info);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to activate key: 0x%x", err);
                return PSA_ERROR_INVALID_HANDLE;
            }
            conf.use_km_key = 1;
        } else
#endif /* SOC_KEY_MANAGER_SUPPORTED */
        {
            conf.efuse_key_blk = storage_get_efuse_block(key_buffer, is_persistent);
        }

        esp_ecdsa_acquire_hardware();
        bool process_again = false;

        do {
            ecdsa_hal_export_pubkey(&conf, qx, qy, key_len);

            process_again = !ecdsa_hal_get_operation_result()
                            || !memcmp(qx, zeroes, key_len)
                            || !memcmp(qy, zeroes, key_len);

        } while (process_again);

        esp_ecdsa_release_hardware();

#if SOC_KEY_MANAGER_SUPPORTED
        if (key_recovery_info) {
            esp_key_mgr_deactivate_key(key_recovery_info->key_type);
        }
#endif /* SOC_KEY_MANAGER_SUPPORTED */

        // Format: uncompressed point (ECDSA_UNCOMPRESSED_POINT_FORMAT byte followed by x and y coordinates)
        data[0] = ECDSA_UNCOMPRESSED_POINT_FORMAT;

        // Convert qx from little-endian to big-endian and copy to output
        esp_ecdsa_change_endianness(qx, data + 1, key_len);

        // Convert qy from little-endian to big-endian and copy to output
        esp_ecdsa_change_endianness(qy, data + 1 + key_len, key_len);
#else
        // This is an invalid operation as the export public key is not supported on this chip
        // and still the key is opaque
        return PSA_ERROR_NOT_SUPPORTED;
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
    }

    *data_length = required_size;

    return PSA_SUCCESS;
}

size_t esp_ecdsa_opaque_size_function(
    const psa_key_attributes_t *attributes,
    psa_key_type_t key_type,
    const uint8_t *data,
    size_t data_length)
{
    (void)key_type;
    bool is_persistent = esp_opaque_key_is_persistent(attributes);

    if (!data || data_length < sizeof(esp_ecdsa_opaque_key_t)) {
        /* Return minimum storage size so PSA core allocates a buffer.
         * The import function will validate data_length and return
         * PSA_ERROR_INVALID_ARGUMENT if the input is too short. */
        return is_persistent ? sizeof(esp_ecdsa_efuse_key_storage_t)
                             : sizeof(esp_ecdsa_volatile_key_storage_t);
    }

    return esp_ecdsa_get_storage_size((const esp_ecdsa_opaque_key_t *)data, is_persistent);
}
#endif /* CONFIG_MBEDTLS_HARDWARE_ECDSA_SIGN */
