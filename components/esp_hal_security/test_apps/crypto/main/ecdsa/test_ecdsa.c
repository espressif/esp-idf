/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "esp_crypto_lock.h"
#include "esp_efuse_chip.h"
#include "esp_crypto_periph_clk.h"
#include "esp_private/esp_crypto_lock_internal.h"
#include "esp_random.h"
#include "esp_err.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "hal/ecc_ll.h"
#include "hal/ecdsa_hal.h"
#include "hal/efuse_ll.h"
#include "hal/ecdsa_ll.h"
#include "hal/ecdsa_types.h"
#ifdef SOC_MPI_SUPPORTED
#include "hal/mpi_ll.h"
#endif
#include "soc/soc_caps.h"

#include "memory_checks.h"
#include "unity_fixture.h"

#include "ecdsa_params.h"
#include "hal_crypto_common.h"
#include "test_ecdsa.h"

ESP_LOG_ATTR_TAG(TAG, "crypto_test");

static void ecdsa_enable_and_reset(void)
{
    esp_crypto_ecdsa_enable_periph_clk(true);
    esp_crypto_ecc_enable_periph_clk(true);
#ifdef SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_enable_periph_clk(true);
    }
#endif
}

static void ecdsa_disable(void)
{
#ifdef SOC_ECDSA_USES_MPI
    if (ecdsa_ll_is_mpi_required()) {
        esp_crypto_mpi_enable_periph_clk(false);
    }
#endif
    esp_crypto_ecc_enable_periph_clk(false);
    esp_crypto_ecdsa_enable_periph_clk(false);
}

static void ecc_be_to_le(const uint8_t* be_point, uint8_t *le_point, uint8_t len)
{
    memset(le_point, 0x0, 32);
    for (int i = 0; i < len; i++) {
        le_point[i] = be_point[len - i - 1];
    }
}

int test_ecdsa_verify(ecdsa_curve_t curve, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y)
{
    uint16_t len = 0;
    uint8_t sha_le[48];

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_VERIFY,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
    };

    switch (curve) {
    case ECDSA_CURVE_SECP192R1:
        conf.curve = ECDSA_CURVE_SECP192R1;
        len = 24;
        break;
    case ECDSA_CURVE_SECP256R1:
        conf.curve = ECDSA_CURVE_SECP256R1;
        len = 32;
        break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ECDSA_CURVE_SECP384R1:
        conf.curve = ECDSA_CURVE_SECP384R1;
        len = 48;
        break;
#endif
    default:
        break;
    }

    /* Set HASH */
    ecc_be_to_le(sha, sha_le, len);

    ecdsa_enable_and_reset();
    int ret = ecdsa_hal_verify_signature(&conf, sha_le, r_le, s_le, pub_x, pub_y, len);
    ecdsa_disable();
    return ret;
}

static void test_ecdsa_corrupt_data(ecdsa_curve_t curve, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, uint8_t *pub_x, uint8_t *pub_y)
{
    int len = 0;

    switch (curve) {
    case ECDSA_CURVE_SECP192R1: len = 24; break;
    case ECDSA_CURVE_SECP256R1: len = 32; break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ECDSA_CURVE_SECP384R1: len = 48; break;
#endif
    default: break;
    }

    // Randomly select a bit and corrupt its corresponding value
    uint16_t r_bit = esp_random() % (len * 8);

    printf("Corrupting SHA bit %d...\n", r_bit);
    sha[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
    sha[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting R bit %d...\n", r_bit);
    r_le[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
    r_le[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting S bit %d...\n", r_bit);
    s_le[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
    s_le[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting pub_x bit %d...\n", r_bit);
    pub_x[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
    pub_x[r_bit / 8] ^= 1 << (r_bit % 8);

    printf("Corrupting pub_y bit %d...\n", r_bit);
    pub_y[r_bit / 8] ^= 1 << (r_bit % 8);
    TEST_ASSERT_EQUAL(-1, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
    pub_y[r_bit / 8] ^= 1 << (r_bit % 8);
}

void test_ecdsa_sign(ecdsa_curve_t curve, uint8_t* sha, uint8_t* r_le, uint8_t* s_le, bool use_km_key, ecdsa_sign_type_t k_type, const uint8_t *sw_key)
{
    uint8_t sha_le[48] = {0};
    uint8_t zeroes[48] = {0};
    uint16_t len = 0;

#if !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
    uint16_t det_loop_number = 1;
#endif /* !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .use_km_key = use_km_key,
        .sign_type = k_type,
    };

#if SOC_ECDSA_SUPPORT_SOFTWARE_KEY
    if (sw_key) {
        conf.use_sw_key = true;
        conf.sw_key = sw_key;
    }
#endif /* SOC_ECDSA_SUPPORT_SOFTWARE_KEY */

    switch (curve) {
    case ECDSA_CURVE_SECP192R1:
        conf.curve = ECDSA_CURVE_SECP192R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_1;
        }
        len = 24;
        break;
    case ECDSA_CURVE_SECP256R1:
        conf.curve = ECDSA_CURVE_SECP256R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_2;
        }
        len = 32;
        break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ECDSA_CURVE_SECP384R1:
        conf.curve = ECDSA_CURVE_SECP384R1;
        if (use_km_key == 0) {
            // Store two key blocks in a single 8-bit integer
            // Most significant 4 bits for first block where, lower 4 bits for second block
            conf.efuse_key_blk = HAL_ECDSA_COMBINE_KEY_BLOCKS(EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_3, EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_4);
        }
        len = 48;
        break;
#endif
    default:
        break;
    }

    /* Set HASH */
    ecc_be_to_le(sha, sha_le, len);

    ecdsa_enable_and_reset();

    bool process_again = false;

    do {
#if !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (ecdsa_ll_is_deterministic_mode_supported() && k_type == ECDSA_K_TYPE_DETERMINISITIC) {
            conf.loop_number = det_loop_number++;
        }
#endif /* !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */

        ecdsa_hal_gen_signature(&conf, sha_le, r_le, s_le, len);

        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(r_le, zeroes, len)
                        || !memcmp(s_le, zeroes, len);

#if SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP
        if (ecdsa_ll_is_deterministic_mode_supported() && k_type == ECDSA_K_TYPE_DETERMINISITIC) {
            process_again |= !ecdsa_hal_det_signature_k_check();
        }
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE && !SOC_ECDSA_SUPPORT_HW_DETERMINISTIC_LOOP */
    } while (process_again);

    ecdsa_disable();
}

void test_ecdsa_sign_and_verify(ecdsa_curve_t curve, uint8_t* sha, uint8_t* pub_x, uint8_t* pub_y, bool use_km_key, ecdsa_sign_type_t k_type, const uint8_t *sw_key)
{
    uint8_t r_le[48] = {0};
    uint8_t s_le[48] = {0};
    test_ecdsa_sign(curve, sha, r_le, s_le, use_km_key, k_type, sw_key);
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(curve, sha, r_le, s_le, pub_x, pub_y));
}

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
void test_ecdsa_export_pubkey_inner(ecdsa_curve_t curve, uint8_t *exported_pub_x, uint8_t *exported_pub_y, bool use_km_key, const uint8_t *sw_key, uint16_t *len)
{
    uint8_t zeroes[48] = {0};
    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_EXPORT_PUBKEY,
        .use_km_key = use_km_key,
    };

#if SOC_ECDSA_SUPPORT_SOFTWARE_KEY
    if (sw_key) {
        conf.use_sw_key = true;
        conf.sw_key = sw_key;
    }
#endif /* SOC_ECDSA_SUPPORT_SOFTWARE_KEY */

    switch (curve) {
    case ECDSA_CURVE_SECP192R1:
        conf.curve = ECDSA_CURVE_SECP192R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_1;
        }
        *len = 24;
        break;
    case ECDSA_CURVE_SECP256R1:
        conf.curve = ECDSA_CURVE_SECP256R1;
        if (use_km_key == 0) {
            conf.efuse_key_blk = EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_2;
        }
        *len = 32;
        break;
#if SOC_ECDSA_SUPPORT_CURVE_P384
    case ECDSA_CURVE_SECP384R1:
        conf.curve = ECDSA_CURVE_SECP384R1;
        if (use_km_key == 0) {
            // Store two key blocks in a single 8-bit integer
            // Most significant 4 bits for first block where, lower 4 bits for second block
            conf.efuse_key_blk = HAL_ECDSA_COMBINE_KEY_BLOCKS(EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_3, EFUSE_BLK_KEY0 + ECDSA_KEY_BLOCK_4);
        }
        *len = 48;
        break;
#endif
    default:
        break;
    }

    ecdsa_enable_and_reset();

    bool process_again = false;

    do {
        ecdsa_hal_export_pubkey(&conf, exported_pub_x, exported_pub_y, *len);
        process_again = !ecdsa_hal_get_operation_result()
                        || !memcmp(exported_pub_x, zeroes, *len)
                        || !memcmp(exported_pub_y, zeroes, *len);
    } while (process_again);

    ecdsa_disable();
}
void test_ecdsa_export_pubkey(ecdsa_curve_t curve, uint8_t *ecdsa_pub_x, uint8_t *ecdsa_pub_y, bool use_km_key, const uint8_t *sw_key)
{
    uint8_t pub_x[48] = {0};
    uint8_t pub_y[48] = {0};
    uint16_t len;
    test_ecdsa_export_pubkey_inner(curve, pub_x, pub_y, use_km_key, sw_key, &len);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecdsa_pub_x, pub_x, len);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(ecdsa_pub_y, pub_y, len);
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

TEST_GROUP(ecdsa);

TEST_SETUP(ecdsa)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(ecdsa)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(ecdsa, ecdsa_SECP192R1_signature_verification)
{
    if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        TEST_ASSERT_EQUAL(0, test_ecdsa_verify(ECDSA_CURVE_SECP192R1, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y));
    }
}

TEST(ecdsa, ecdsa_SECP192R1_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, ECDSA_K_TYPE_TRNG, NULL);
    }
}

TEST(ecdsa, ecdsa_SECP192R1_corrupt_signature)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_corrupt_data(ECDSA_CURVE_SECP192R1, sha, ecdsa192_r, ecdsa192_s, ecdsa192_pub_x, ecdsa192_pub_y);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_signature_verification)
{
    TEST_ASSERT_EQUAL(0, test_ecdsa_verify(ECDSA_CURVE_SECP256R1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y));
}

TEST(ecdsa, ecdsa_SECP256R1_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, ECDSA_K_TYPE_TRNG, NULL);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_corrupt_signature)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_corrupt_data(ECDSA_CURVE_SECP256R1, sha, ecdsa256_r, ecdsa256_s, ecdsa256_pub_x, ecdsa256_pub_y);
    }
}

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
TEST(ecdsa, ecdsa_SECP192R1_det_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP192R1, sha, ecdsa192_pub_x, ecdsa192_pub_y, false, ECDSA_K_TYPE_DETERMINISITIC, NULL);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_det_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP256R1, sha, ecdsa256_pub_x, ecdsa256_pub_y, false, ECDSA_K_TYPE_DETERMINISITIC, NULL);
    }
}
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
TEST(ecdsa, ecdsa_SECP192R1_export_pubkey)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_export_pubkey(ECDSA_CURVE_SECP192R1, ecdsa192_pub_x, ecdsa192_pub_y, 0, NULL);
    }
}

TEST(ecdsa, ecdsa_SECP256R1_export_pubkey)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_export_pubkey(ECDSA_CURVE_SECP256R1, ecdsa256_pub_x, ecdsa256_pub_y, 0, NULL);
    }
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST(ecdsa, ecdsa_SECP384R1_signature_verification)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        TEST_ASSERT_EQUAL(0, test_ecdsa_verify(ECDSA_CURVE_SECP384R1, sha, ecdsa384_r, ecdsa384_s, ecdsa384_pub_x, ecdsa384_pub_y));
    }
}

TEST(ecdsa, ecdsa_SECP384R1_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP384R1, sha, ecdsa384_pub_x, ecdsa384_pub_y, false, ECDSA_K_TYPE_TRNG, NULL);
    }
}

TEST(ecdsa, ecdsa_SECP384R1_corrupt_signature)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_corrupt_data(ECDSA_CURVE_SECP384R1, sha, ecdsa384_r, ecdsa384_s, ecdsa384_pub_x, ecdsa384_pub_y);
    }
}

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
TEST(ecdsa, ecdsa_SECP384R1_det_sign_and_verify)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else if (!ecdsa_ll_is_deterministic_mode_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA deterministic mode is not supported.");
    } else {
        test_ecdsa_sign_and_verify(ECDSA_CURVE_SECP384R1, sha, ecdsa384_pub_x, ecdsa384_pub_y, false, ECDSA_K_TYPE_DETERMINISITIC, NULL);
    }
}
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */

#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
TEST(ecdsa, ecdsa_SECP384R1_export_pubkey)
{
    if (!ecdsa_ll_is_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA is not supported.");
    } else {
        test_ecdsa_export_pubkey(ECDSA_CURVE_SECP384R1, ecdsa384_pub_x, ecdsa384_pub_y, 0, NULL);
    }
}
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

#if SOC_ECDSA_SUPPORT_SOFTWARE_KEY
/* Software-key test cases do not need any key burnt in eFuse, so they are kept
 * in a separate test group that runs without CONFIG_CRYPTO_TEST_APP_ENABLE_ECDSA_TESTS */

static void test_ecdsa_sw_key_on_curve(ecdsa_curve_t curve, uint8_t *pub_x, uint8_t *pub_y, const uint8_t *priv)
{
    test_ecdsa_sign_and_verify(curve, sha, pub_x, pub_y, false, ECDSA_K_TYPE_TRNG, priv);
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    test_ecdsa_sign_and_verify(curve, sha, pub_x, pub_y, false, ECDSA_K_TYPE_DETERMINISITIC, priv);
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    test_ecdsa_export_pubkey(curve, pub_x, pub_y, 0, priv);
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
}

TEST(ecdsa, sw_key_SECP192R1_operations)
{
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA software key is disabled by eFuse.");
    } else if (!esp_efuse_is_ecdsa_p192_curve_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA 192-curve operations are disabled.");
    } else {
        test_ecdsa_sw_key_on_curve(ECDSA_CURVE_SECP192R1, ecdsa192_pub_x, ecdsa192_pub_y, ecdsa192_priv);
    }
}

TEST(ecdsa, sw_key_SECP256R1_operations)
{
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA software key is disabled by eFuse.");
    } else {
        test_ecdsa_sw_key_on_curve(ECDSA_CURVE_SECP256R1, ecdsa256_pub_x, ecdsa256_pub_y, ecdsa256_priv);
    }
}

#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
TEST(ecdsa, sw_key_SECP384R1_operations)
{
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA software key is disabled by eFuse.");
    } else {
        test_ecdsa_sw_key_on_curve(ECDSA_CURVE_SECP384R1, ecdsa384_pub_x, ecdsa384_pub_y, ecdsa384_priv);
    }
}
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */

/* Read the key registers back as the little-endian byte string that
 * ecdsa_ll_write_key() writes */
static void ecdsa_read_key_regs(uint8_t *key_regs)
{
    for (uint16_t i = 0; i < ECDSA_LL_KEY_REG_BYTES; i += 4) {
        uint32_t word = REG_READ(ECDSA_KEY_0_REG + i);
        memcpy(key_regs + i, &word, sizeof(word));
    }
}

TEST(ecdsa, sw_key_registers_cleared_after_use)
{
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA software key is disabled by eFuse.");
        return;
    }

    uint8_t sha_le[32] = {0};
    uint8_t r_le[32] = {0};
    uint8_t s_le[32] = {0};
    uint8_t key_regs[ECDSA_LL_KEY_REG_BYTES] = {0};
    uint8_t expected_key[ECDSA_LL_KEY_REG_BYTES] = {0};
    memcpy(expected_key, ecdsa256_priv, sizeof(ecdsa256_priv));

    ecdsa_hal_config_t conf = {
        .mode = ECDSA_MODE_SIGN_GEN,
        .curve = ECDSA_CURVE_SECP256R1,
        .sha_mode = ECDSA_Z_USER_PROVIDED,
        .sign_type = ECDSA_K_TYPE_TRNG,
        .use_sw_key = true,
        .sw_key = ecdsa256_priv,
    };
    ecc_be_to_le(sha, sha_le, sizeof(sha_le));

    /* Positive control: while the peripheral is still acquired the key
     * registers hold the key. Without this, the check after release below
     * would also pass if the registers were not readable at all. */
    ecdsa_enable_and_reset();
    ecdsa_hal_gen_signature(&conf, sha_le, r_le, s_le, sizeof(r_le));
    ecdsa_read_key_regs(key_regs);
    ecdsa_disable();
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_key, key_regs, sizeof(key_regs));

    /* Releasing the peripheral must have scrubbed the key registers.
     * Re-enable only the bus clock (without the reset that a regular acquire
     * performs) so that any leftover key material would still be visible.
     * The ECC clock is enabled to hold the shared crypto clock on. */
    esp_crypto_ecc_enable_periph_clk(true);
    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(true);
    }
    ecdsa_read_key_regs(key_regs);
    ECDSA_RCC_ATOMIC() {
        ecdsa_ll_enable_bus_clock(false);
    }
    esp_crypto_ecc_enable_periph_clk(false);
    TEST_ASSERT_EACH_EQUAL_HEX8(0, key_regs, sizeof(key_regs));
}

#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
/* RFC 6979 Appendix A.2.5: NIST P-256 (secp256r1), hash SHA-256, message "sample".
 * The values are stored big-endian, exactly as printed in the RFC. The peripheral
 * works little-endian, so the key and the expected r and s are byte-reversed before use. */
static const uint8_t rfc6979_p256_priv_be[32] = {
    0xC9, 0xAF, 0xA9, 0xD8, 0x45, 0xBA, 0x75, 0x16,
    0x6B, 0x5C, 0x21, 0x57, 0x67, 0xB1, 0xD6, 0x93,
    0x4E, 0x50, 0xC3, 0xDB, 0x36, 0xE8, 0x9B, 0x12,
    0x7B, 0x8A, 0x62, 0x2B, 0x12, 0x0F, 0x67, 0x21,
};
static const uint8_t rfc6979_p256_sample_hash_be[32] = {  /* SHA-256("sample") */
    0xAF, 0x2B, 0xDB, 0xE1, 0xAA, 0x9B, 0x6E, 0xC1,
    0xE2, 0xAD, 0xE1, 0xD6, 0x94, 0xF4, 0x1F, 0xC7,
    0x1A, 0x83, 0x1D, 0x02, 0x68, 0xE9, 0x89, 0x15,
    0x62, 0x11, 0x3D, 0x8A, 0x62, 0xAD, 0xD1, 0xBF,
};
static const uint8_t rfc6979_p256_sample_r_be[32] = {
    0xEF, 0xD4, 0x8B, 0x2A, 0xAC, 0xB6, 0xA8, 0xFD,
    0x11, 0x40, 0xDD, 0x9C, 0xD4, 0x5E, 0x81, 0xD6,
    0x9D, 0x2C, 0x87, 0x7B, 0x56, 0xAA, 0xF9, 0x91,
    0xC3, 0x4D, 0x0E, 0xA8, 0x4E, 0xAF, 0x37, 0x16,
};
static const uint8_t rfc6979_p256_sample_s_be[32] = {
    0xF7, 0xCB, 0x1C, 0x94, 0x2D, 0x65, 0x7C, 0x41,
    0xD4, 0x36, 0xC7, 0xA1, 0xB6, 0xE2, 0x9F, 0x65,
    0xF3, 0xE9, 0x00, 0xDB, 0xB9, 0xAF, 0xF4, 0x06,
    0x4D, 0xC4, 0xAB, 0x2F, 0x84, 0x3A, 0xCD, 0xA8,
};

/* Known-answer test for the deterministic-K derivation of the ECDSA peripheral.
 * A deterministic signature over the fixed key and message above must equal the
 * RFC 6979 vector. */
TEST(ecdsa, sw_key_SECP256R1_deterministic_rfc6979_kat)
{
    if (!esp_efuse_is_ecdsa_software_key_supported()) {
        ESP_LOGI(TAG, "Skipping test because ECDSA software key is disabled by eFuse.");
        return;
    }

    /* test_ecdsa_sign expects the private key little-endian and the hash big-endian,
     * and returns r and s little-endian. */
    uint8_t priv_le[32] = {0};
    ecc_be_to_le(rfc6979_p256_priv_be, priv_le, 32);

    uint8_t r_le[32] = {0};
    uint8_t s_le[32] = {0};
    test_ecdsa_sign(ECDSA_CURVE_SECP256R1, (uint8_t *)rfc6979_p256_sample_hash_be,
                    r_le, s_le, false, ECDSA_K_TYPE_DETERMINISITIC, priv_le);

    uint8_t expected_r_le[32] = {0};
    uint8_t expected_s_le[32] = {0};
    ecc_be_to_le(rfc6979_p256_sample_r_be, expected_r_le, 32);
    ecc_be_to_le(rfc6979_p256_sample_s_be, expected_s_le, 32);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_r_le, r_le, 32);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_s_le, s_le, 32);
}
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#endif /* SOC_ECDSA_SUPPORT_SOFTWARE_KEY */

TEST_GROUP_RUNNER(ecdsa)
{
    /* The following test cases need keys provisioned in eFuse,
     * see CONFIG_CRYPTO_TEST_APP_ENABLE_ECDSA_TESTS */
#if CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS && CONFIG_CRYPTO_TEST_APP_ENABLE_ECDSA_TESTS
    /* SECP192R1 test cases */
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_corrupt_signature)
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_det_sign_and_verify)
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    RUN_TEST_CASE(ecdsa, ecdsa_SECP192R1_export_pubkey)
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

    /* SECP256R1 test cases */
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_corrupt_signature)
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_det_sign_and_verify)
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    RUN_TEST_CASE(ecdsa, ecdsa_SECP256R1_export_pubkey)
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */

    /* SECP384R1 test cases */
#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
    RUN_TEST_CASE(ecdsa, ecdsa_SECP384R1_signature_verification)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP384R1_sign_and_verify)
    RUN_TEST_CASE(ecdsa, ecdsa_SECP384R1_corrupt_signature)
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    RUN_TEST_CASE(ecdsa, ecdsa_SECP384R1_det_sign_and_verify)
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#ifdef SOC_ECDSA_SUPPORT_EXPORT_PUBKEY
    RUN_TEST_CASE(ecdsa, ecdsa_SECP384R1_export_pubkey)
#endif /* SOC_ECDSA_SUPPORT_EXPORT_PUBKEY */
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
#endif /* CONFIG_CRYPTO_TEST_APP_ENABLE_FPGA_TESTS && CONFIG_CRYPTO_TEST_APP_ENABLE_ECDSA_TESTS */

#if SOC_ECDSA_SUPPORT_SOFTWARE_KEY
    /* Software-key test cases need no key provisioned in eFuse */
    RUN_TEST_CASE(ecdsa, sw_key_SECP192R1_operations)
    RUN_TEST_CASE(ecdsa, sw_key_SECP256R1_operations)
#ifdef SOC_ECDSA_SUPPORT_CURVE_P384
    RUN_TEST_CASE(ecdsa, sw_key_SECP384R1_operations)
#endif /* SOC_ECDSA_SUPPORT_CURVE_P384 */
    RUN_TEST_CASE(ecdsa, sw_key_registers_cleared_after_use)
#ifdef SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE
    RUN_TEST_CASE(ecdsa, sw_key_SECP256R1_deterministic_rfc6979_kat)
#endif /* SOC_ECDSA_SUPPORT_DETERMINISTIC_MODE */
#endif /* SOC_ECDSA_SUPPORT_SOFTWARE_KEY */
}
