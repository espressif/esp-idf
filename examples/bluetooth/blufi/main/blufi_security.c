/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include "esp_random.h"
#if CONFIG_BT_CONTROLLER_ENABLED || !CONFIG_BT_NIMBLE_ENABLED
#include "esp_bt.h"
#endif

#include "esp_blufi_api.h"
#include "blufi_example.h"

#include "psa/crypto.h"
#include "esp_crc.h"
#include "soc/soc_caps.h"

/*
   The SEC_TYPE_xxx is for self-defined packet data type in the procedure of "BLUFI negotiate key"
   If user use other negotiation procedure to exchange(or generate) key, should redefine the type by yourself.
 */
#define SEC_TYPE_DH_PARAM_LEN   0x00
#define SEC_TYPE_DH_PARAM_DATA  0x01
#define SEC_TYPE_DH_P           0x02
#define SEC_TYPE_DH_G           0x03
#define SEC_TYPE_DH_PUBLIC      0x04

#define BLUFI_IV_PREFIX_LEN     9
#define BLUFI_IV_SIZE           16
#define BLUFI_HASH_SIZE         32   /* SHA-256 output size */
#define BLUFI_KEY_SIZE          32
#define BLUFI_ENC_DOMAIN_STR "blufi_enc"
#define BLUFI_DEC_DOMAIN_STR "blufi_dec"

#if !SOC_MPI_SUPPORTED
#define BLUFI_DH_PREGEN_STACK_SIZE   4096
#define BLUFI_DH_PREGEN_PRIO        (tskIDLE_PRIORITY + 1)
#define BLUFI_DH_PREGEN_WAIT_MS      30000
#define BLUFI_DH_HEAVY_CRYPTO_WDT_MS 30000
#endif

struct blufi_security {
#define DH_PARAM_LEN_MAX        1024
#define DH_SELF_PUB_KEY_LEN     384  /* Support 3072-bit DH key (3072/8 = 384 bytes) */
    uint8_t  self_public_key[DH_SELF_PUB_KEY_LEN];
#define SHARE_KEY_LEN           384  /* Support 3072-bit DH key (3072/8 = 384 bytes) */
    uint8_t  share_key[SHARE_KEY_LEN];
    size_t   share_len;
#define PSK_LEN                 32
    uint8_t  psk[PSK_LEN];
    uint8_t  *dh_param;
    int      dh_param_len;
    psa_key_id_t aes_key;
    psa_cipher_operation_t enc_operation;  /* Persistent cipher operation for encryption */
    psa_cipher_operation_t dec_operation;  /* Persistent cipher operation for decryption */
};
static struct blufi_security *blufi_sec;

extern void btc_blufi_report_error(esp_blufi_error_state_t state);

/**
 * @brief Clean up DH parameter memory
 */
static void blufi_cleanup_dh_param(void)
{
    if (blufi_sec && blufi_sec->dh_param) {
        free(blufi_sec->dh_param);
        blufi_sec->dh_param = NULL;
    }
}

/**
 * @brief Clean up AES key and cipher operations
 *
 * @param abort_enc Whether to abort encryption operation
 * @param abort_dec Whether to abort decryption operation
 */
static void blufi_cleanup_aes_session(bool abort_enc, bool abort_dec)
{
    if (!blufi_sec) {
        return;
    }

    if (abort_enc) {
        psa_cipher_abort(&blufi_sec->enc_operation);
    }
    if (abort_dec) {
        psa_cipher_abort(&blufi_sec->dec_operation);
    }
    if (blufi_sec->aes_key != 0) {
        psa_destroy_key(blufi_sec->aes_key);
        blufi_sec->aes_key = 0;
    }
}

/**
 * @brief Clean up entire DH negotiation session (AES key, cipher ops, and DH params)
 *
 * @param abort_enc Whether to abort encryption operation
 * @param abort_dec Whether to abort decryption operation
 */
static void blufi_cleanup_negotiation(bool abort_enc, bool abort_dec)
{
    blufi_cleanup_aes_session(abort_enc, abort_dec);
    blufi_cleanup_dh_param();
}

#if !SOC_MPI_SUPPORTED
/*
 * DH keypair pre-generation: overlap the expensive G^X mod P computation
 * with BLE advertising so only key agreement remains on the critical path
 * when the phone sends its DH parameters.
 */
static psa_key_id_t s_pregen_private_key;
static uint8_t s_pregen_public_key[DH_SELF_PUB_KEY_LEN];
static size_t s_pregen_public_key_len;
static SemaphoreHandle_t s_pregen_done;
static bool s_pregen_ok;
static TaskHandle_t s_pregen_task_hdl;
static void (*s_pregen_complete_cb)(void);

static void blufi_wdt_set_timeout(uint32_t timeout_ms)
{
    esp_task_wdt_config_t cfg = {
        .timeout_ms = timeout_ms,
        .idle_core_mask = (1 << portNUM_PROCESSORS) - 1,
        .trigger_panic = true,
    };
    esp_task_wdt_reconfigure(&cfg);
}

static void blufi_dh_pregen_task(void *arg)
{
    (void)arg;

    blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);

    psa_key_attributes_t attr = psa_key_attributes_init();
    psa_set_key_type(&attr, PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919));
    psa_set_key_bits(&attr, 3072);
    psa_set_key_algorithm(&attr, PSA_ALG_FFDH);
    psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_DERIVE);

    psa_status_t status = psa_generate_key(&attr, &s_pregen_private_key);
    if (status == PSA_SUCCESS) {
        status = psa_export_public_key(s_pregen_private_key,
                                       s_pregen_public_key, DH_SELF_PUB_KEY_LEN,
                                       &s_pregen_public_key_len);
    }
    psa_reset_key_attributes(&attr);

    s_pregen_ok = (status == PSA_SUCCESS);
    if (!s_pregen_ok && s_pregen_private_key != 0) {
        psa_destroy_key(s_pregen_private_key);
        s_pregen_private_key = 0;
    }

    BLUFI_INFO("DH keypair pre-generation %s", s_pregen_ok ? "done" : "FAILED");

    blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);

    xSemaphoreGive(s_pregen_done);

    if (s_pregen_complete_cb) {
        void (*cb)(void) = s_pregen_complete_cb;
        s_pregen_complete_cb = NULL;
        cb();
    }

    vTaskSuspend(NULL);
}

static void blufi_dh_pregen_start_impl(void (*done_cb)(void))
{
    if (s_pregen_done != NULL) {
        return;
    }

    s_pregen_private_key = 0;
    s_pregen_public_key_len = 0;
    s_pregen_ok = false;
    s_pregen_task_hdl = NULL;
    s_pregen_complete_cb = done_cb;
    s_pregen_done = xSemaphoreCreateBinary();
    if (s_pregen_done == NULL) {
        BLUFI_ERROR("Failed to create pre-gen semaphore");
        return;
    }

    if (xTaskCreate(blufi_dh_pregen_task, "blufi_pregen",
                    BLUFI_DH_PREGEN_STACK_SIZE, NULL,
                    BLUFI_DH_PREGEN_PRIO, &s_pregen_task_hdl) != pdPASS) {
        BLUFI_ERROR("Failed to create pre-gen task");
        vSemaphoreDelete(s_pregen_done);
        s_pregen_done = NULL;
    }
}

void blufi_dh_pregen_start(void)
{
    blufi_dh_pregen_start_impl(NULL);
}

void blufi_dh_pregen_start_with_cb(void (*done_cb)(void))
{
    blufi_dh_pregen_start_impl(done_cb);
}

void blufi_dh_pregen_wait(void)
{
    if (s_pregen_done == NULL) {
        return;
    }
    xSemaphoreTake(s_pregen_done, portMAX_DELAY);
    xSemaphoreGive(s_pregen_done);
}

static void blufi_dh_pregen_cleanup(void)
{
    if (s_pregen_done != NULL) {
        xSemaphoreTake(s_pregen_done, portMAX_DELAY);
        vSemaphoreDelete(s_pregen_done);
        s_pregen_done = NULL;
    }
    if (s_pregen_task_hdl != NULL) {
        vTaskDelete(s_pregen_task_hdl);
        s_pregen_task_hdl = NULL;
    }
    if (s_pregen_private_key != 0) {
        psa_destroy_key(s_pregen_private_key);
        s_pregen_private_key = 0;
    }
    s_pregen_ok = false;
}
#endif /* !SOC_MPI_SUPPORTED */

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free)
{
    if (data == NULL || len < 3) {
        BLUFI_ERROR("BLUFI Invalid data format");
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    /* Validate output parameters */
    if (output_data == NULL || output_len == NULL || need_free == NULL) {
        BLUFI_ERROR("BLUFI Invalid output parameters");
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        return;
    }

    uint8_t type = data[0];

    if (blufi_sec == NULL) {
        BLUFI_ERROR("BLUFI Security is not initialized");
        btc_blufi_report_error(ESP_BLUFI_INIT_SECURITY_ERROR);
        return;
    }

    switch (type) {
    case SEC_TYPE_DH_PARAM_LEN:
        blufi_sec->dh_param_len = ((data[1]<<8)|data[2]);
        // Security fix: Limit DH param length to prevent DoS via large memory allocation
        if (blufi_sec->dh_param_len == 0 || blufi_sec->dh_param_len > DH_PARAM_LEN_MAX) {
            BLUFI_ERROR("%s, invalid dh param len %d\n", __func__, blufi_sec->dh_param_len);
            blufi_sec->dh_param_len = 0;
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Allow renegotiation - clean up previous state */
        blufi_cleanup_dh_param();
        blufi_cleanup_aes_session(true, true);

        blufi_sec->dh_param = (uint8_t *)malloc(blufi_sec->dh_param_len);
        if (blufi_sec->dh_param == NULL) {
            blufi_sec->dh_param_len = 0;  /* Reset length to avoid using unallocated memory */
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            BLUFI_ERROR("%s, malloc failed\n", __func__);
            return;
        }
        break;
    case SEC_TYPE_DH_PARAM_DATA:{
        if (blufi_sec->dh_param == NULL) {
            BLUFI_ERROR("%s, blufi_sec->dh_param == NULL\n", __func__);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        if (len < (blufi_sec->dh_param_len + 1)) {
            BLUFI_ERROR("%s, invalid dh param len\n", __func__);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        uint8_t *param = blufi_sec->dh_param;
        memcpy(blufi_sec->dh_param, &data[1], blufi_sec->dh_param_len);

        /* Parse P length */
        if (blufi_sec->dh_param_len < 2) {
            BLUFI_ERROR("DH param too short for P length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t p_len = (param[0] << 8) | param[1];
        size_t p_offset = 2 + p_len;
        if (p_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("P length %d exceeds dh_param bounds", p_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        param += p_offset;

        /* Parse G length */
        if ((param - blufi_sec->dh_param) + 2 > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("DH param too short for G length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t g_len = (param[0] << 8) | param[1];
        size_t g_offset = (param - blufi_sec->dh_param) + 2 + g_len;
        if (g_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("G length %d exceeds dh_param bounds", g_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        param += 2 + g_len;

        /* Parse public key length */
        if ((param - blufi_sec->dh_param) + 2 > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("DH param too short for public key length");
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }
        size_t pub_len = (param[0] << 8) | param[1];
        param += 2;

        size_t pub_offset = (param - blufi_sec->dh_param) + pub_len;
        if (pub_offset > (size_t)blufi_sec->dh_param_len) {
            BLUFI_ERROR("Public key length %d exceeds dh_param bounds", pub_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Determine key bits based on public key length (RFC7919 key sizes)
         * Note: Only 3072 bit keys are supported due to buffer size constraints
         * (DH_SELF_PUB_KEY_LEN = 384 bytes = 3072 bits) */
        size_t key_bits = pub_len * 8;
        if (key_bits != 3072) {
            BLUFI_ERROR("Unsupported DH key length: %d bytes (only 3072 bit keys supported)", pub_len);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Validate public key length fits in output buffer */
        if (pub_len > DH_SELF_PUB_KEY_LEN) {
            BLUFI_ERROR("Public key length %d exceeds output buffer size %d", pub_len, DH_SELF_PUB_KEY_LEN);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        psa_algorithm_t alg = PSA_ALG_FFDH;
        psa_key_id_t private_key = 0;
        size_t public_key_len = 0;
        psa_status_t status;

#if !SOC_MPI_SUPPORTED
        /* On SoCs without hardware MPI, 3072-bit modular exponentiation takes
         * ~12s in software.  Use the pre-generated keypair, free all possible
         * heap before key agreement, and widen the task WDT window. */
        bool used_pregen = false;

        if (s_pregen_done != NULL &&
            xSemaphoreTake(s_pregen_done, pdMS_TO_TICKS(BLUFI_DH_PREGEN_WAIT_MS)) == pdTRUE) {
            xSemaphoreGive(s_pregen_done);
            if (s_pregen_ok && s_pregen_private_key != 0) {
                private_key = s_pregen_private_key;
                s_pregen_private_key = 0;
                memcpy(blufi_sec->self_public_key, s_pregen_public_key, s_pregen_public_key_len);
                public_key_len = s_pregen_public_key_len;
                used_pregen = true;
                BLUFI_INFO("Using pre-generated DH keypair");
            }
        }

        if (!used_pregen) {
            BLUFI_INFO("Pre-gen unavailable, generating DH keypair inline");
            blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);

            psa_key_attributes_t keygen_attr = psa_key_attributes_init();
            psa_set_key_type(&keygen_attr, PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919));
            psa_set_key_bits(&keygen_attr, key_bits);
            psa_set_key_algorithm(&keygen_attr, alg);
            psa_set_key_usage_flags(&keygen_attr, PSA_KEY_USAGE_DERIVE);

            status = psa_generate_key(&keygen_attr, &private_key);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_generate_key failed %d\n", __func__, status);
                blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
            psa_reset_key_attributes(&keygen_attr);

            status = psa_export_public_key(private_key, blufi_sec->self_public_key,
                                           DH_SELF_PUB_KEY_LEN, &public_key_len);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_export_public_key failed %d\n", __func__, status);
                blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);
                psa_destroy_key(private_key);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
        }

        uint8_t peer_pub[DH_SELF_PUB_KEY_LEN];
        memcpy(peer_pub, param, pub_len);
        blufi_cleanup_dh_param();

        if (s_pregen_task_hdl != NULL) {
            vTaskDelete(s_pregen_task_hdl);
            s_pregen_task_hdl = NULL;
        }

        blufi_wdt_set_timeout(BLUFI_DH_HEAVY_CRYPTO_WDT_MS);
        status = psa_raw_key_agreement(alg, private_key, peer_pub, pub_len,
                                       blufi_sec->share_key, SHARE_KEY_LEN,
                                       &blufi_sec->share_len);
        psa_destroy_key(private_key);
        blufi_wdt_set_timeout(CONFIG_ESP_TASK_WDT_TIMEOUT_S * 1000);
#else /* SOC_MPI_SUPPORTED */
        {
            psa_key_attributes_t keygen_attr = psa_key_attributes_init();
            psa_set_key_type(&keygen_attr, PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919));
            psa_set_key_bits(&keygen_attr, key_bits);
            psa_set_key_algorithm(&keygen_attr, alg);
            psa_set_key_usage_flags(&keygen_attr, PSA_KEY_USAGE_DERIVE);

            status = psa_generate_key(&keygen_attr, &private_key);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_generate_key failed %d\n", __func__, status);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
            psa_reset_key_attributes(&keygen_attr);

            status = psa_export_public_key(private_key, blufi_sec->self_public_key,
                                           DH_SELF_PUB_KEY_LEN, &public_key_len);
            if (status != PSA_SUCCESS) {
                BLUFI_ERROR("%s psa_export_public_key failed %d\n", __func__, status);
                psa_destroy_key(private_key);
                btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
                blufi_cleanup_dh_param();
                return;
            }
        }

        status = psa_raw_key_agreement(alg, private_key, param, pub_len,
                                       blufi_sec->share_key, SHARE_KEY_LEN,
                                       &blufi_sec->share_len);
        psa_destroy_key(private_key);
        blufi_cleanup_dh_param();
#endif /* !SOC_MPI_SUPPORTED */

        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_raw_key_agreement failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        /* Validate share key length fits in buffer */
        if (blufi_sec->share_len > SHARE_KEY_LEN) {
            BLUFI_ERROR("Share key length %d exceeds buffer size %d", blufi_sec->share_len, SHARE_KEY_LEN);
            blufi_cleanup_dh_param();
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        size_t hash_length = 0;
        status = psa_hash_compute(PSA_ALG_SHA_256, blufi_sec->share_key, blufi_sec->share_len, blufi_sec->psk, PSK_LEN, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_hash_compute failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_CALC_SHA_256_ERROR);
            blufi_cleanup_dh_param();
            return;
        }

        /* Destroy previous AES key and cipher operations if they exist */
        blufi_cleanup_aes_session(true, true);

        /* Import AES key for CTR mode */
        psa_key_attributes_t attributes = psa_key_attributes_init();
        psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attributes, PSK_LEN * 8);
        psa_set_key_algorithm(&attributes, PSA_ALG_CTR);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        status = psa_import_key(&attributes, blufi_sec->psk, PSK_LEN, &blufi_sec->aes_key);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_import_key failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_dh_param();
            return;
        }
        psa_reset_key_attributes(&attributes);

        /* Derive domain-separated IVs for encryption and decryption */
        uint8_t enc_material[BLUFI_IV_PREFIX_LEN + SHARE_KEY_LEN];
        memcpy(enc_material, BLUFI_ENC_DOMAIN_STR, BLUFI_IV_PREFIX_LEN);
        memcpy(enc_material + BLUFI_IV_PREFIX_LEN, blufi_sec->share_key, blufi_sec->share_len);

        uint8_t enc_hash[BLUFI_HASH_SIZE];
        status = psa_hash_compute(PSA_ALG_SHA_256, enc_material, BLUFI_IV_PREFIX_LEN + blufi_sec->share_len,
                                 enc_hash, BLUFI_HASH_SIZE, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption IV derivation failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }
        mbedtls_platform_zeroize(enc_material, sizeof(enc_material));

        uint8_t dec_material[BLUFI_IV_PREFIX_LEN + SHARE_KEY_LEN];
        memcpy(dec_material, BLUFI_DEC_DOMAIN_STR, BLUFI_IV_PREFIX_LEN);
        memcpy(dec_material + BLUFI_IV_PREFIX_LEN, blufi_sec->share_key, blufi_sec->share_len);

        uint8_t dec_hash[BLUFI_HASH_SIZE];
        status = psa_hash_compute(PSA_ALG_SHA_256, dec_material, BLUFI_IV_PREFIX_LEN + blufi_sec->share_len,
                                 dec_hash, BLUFI_HASH_SIZE, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption IV derivation failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }
        mbedtls_platform_zeroize(dec_material, sizeof(dec_material));

        /* Use first 16 bytes (128 bits) as IV for CTR mode */
        uint8_t iv_enc[BLUFI_IV_SIZE], iv_dec[BLUFI_IV_SIZE];
        memcpy(iv_enc, enc_hash, BLUFI_IV_SIZE);
        memcpy(iv_dec, dec_hash, BLUFI_IV_SIZE);
        mbedtls_platform_zeroize(enc_hash, sizeof(enc_hash));
        mbedtls_platform_zeroize(dec_hash, sizeof(dec_hash));

        /* Setup persistent cipher operations */
        blufi_sec->enc_operation = psa_cipher_operation_init();
        status = psa_cipher_encrypt_setup(&blufi_sec->enc_operation, blufi_sec->aes_key, PSA_ALG_CTR);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(false, false);
            return;
        }

        status = psa_cipher_set_iv(&blufi_sec->enc_operation, iv_enc, BLUFI_IV_SIZE);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s encryption IV setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, false);
            return;
        }
        mbedtls_platform_zeroize(iv_enc, sizeof(iv_enc));

        blufi_sec->dec_operation = psa_cipher_operation_init();
        /* Note: CTR mode uses encrypt_setup for both encryption and decryption */
        status = psa_cipher_encrypt_setup(&blufi_sec->dec_operation, blufi_sec->aes_key, PSA_ALG_CTR);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, false);
            return;
        }

        status = psa_cipher_set_iv(&blufi_sec->dec_operation, iv_dec, BLUFI_IV_SIZE);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s decryption IV setup failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            blufi_cleanup_negotiation(true, true);
            return;
        }
        mbedtls_platform_zeroize(iv_dec, sizeof(iv_dec));

        /* alloc output data */
        *output_data = &blufi_sec->self_public_key[0];
        *output_len = public_key_len;
        *need_free = false;
    }
        break;
    case SEC_TYPE_DH_P:
        break;
    case SEC_TYPE_DH_G:
        break;
    case SEC_TYPE_DH_PUBLIC:
        break;
    default:
        /* Reject unknown packet types */
        BLUFI_ERROR("Unknown packet type: 0x%02x", type);
        btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
        break;
    }
}

int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    (void)iv8;  /* iv8 parameter is no longer used with persistent CTR operation */

    if (!blufi_sec || !crypt_data || crypt_len < 0) {
        return -1;
    }

    if (crypt_len == 0) {
        return 0;
    }

    if (blufi_sec->aes_key == 0) {
        BLUFI_ERROR("AES key is not initialized");
        return -1;
    }

    /* Use persistent cipher operation - CTR counter is automatically managed */
    size_t output_len = 0;
    size_t output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CTR, crypt_len);
    uint8_t *output_buf = (uint8_t *)malloc(output_buf_size);
    if (output_buf == NULL) {
        BLUFI_ERROR("Failed to allocate output buffer");
        return -1;
    }

    psa_status_t status = psa_cipher_update(&blufi_sec->enc_operation, crypt_data, crypt_len,
                                           output_buf, output_buf_size, &output_len);
    if (status != PSA_SUCCESS) {
        BLUFI_ERROR("Encryption failed with status=%d", status);
        free(output_buf);
        return -1;
    }

    if (output_len > (size_t)crypt_len) {
        BLUFI_ERROR("Output length %d exceeds input buffer size %d", output_len, crypt_len);
        free(output_buf);
        return -1;
    }

    memcpy(crypt_data, output_buf, output_len);
    free(output_buf);

    return output_len;
}

int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    (void)iv8;  /* iv8 parameter is no longer used with persistent CTR operation */

    if (!blufi_sec || !crypt_data || crypt_len < 0) {
        return -1;
    }

    if (crypt_len == 0) {
        return 0;
    }

    if (blufi_sec->aes_key == 0) {
        BLUFI_ERROR("AES key is not initialized");
        return -1;
    }

    /* Use persistent cipher operation - CTR counter is automatically managed */
    size_t output_len = 0;
    size_t output_buf_size = PSA_CIPHER_DECRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CTR, crypt_len);
    uint8_t *output_buf = (uint8_t *)malloc(output_buf_size);
    if (output_buf == NULL) {
        BLUFI_ERROR("Failed to allocate output buffer");
        return -1;
    }

    psa_status_t status = psa_cipher_update(&blufi_sec->dec_operation, crypt_data, crypt_len,
                                           output_buf, output_buf_size, &output_len);
    if (status != PSA_SUCCESS) {
        BLUFI_ERROR("Decryption failed with status=%d", status);
        free(output_buf);
        return -1;
    }

    if (output_len > (size_t)crypt_len) {
        BLUFI_ERROR("Output length %d exceeds input buffer size %d", output_len, crypt_len);
        free(output_buf);
        return -1;
    }

    memcpy(crypt_data, output_buf, output_len);
    free(output_buf);

    return output_len;
}

uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len)
{
    /* This iv8 ignore, not used */
    return esp_crc16_be(0, data, len);
}

esp_err_t blufi_security_init(void)
{
    blufi_sec = (struct blufi_security *)malloc(sizeof(struct blufi_security));
    if (blufi_sec == NULL) {
        return ESP_FAIL;
    }

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));
    blufi_sec->enc_operation = psa_cipher_operation_init();
    blufi_sec->dec_operation = psa_cipher_operation_init();

#if !SOC_MPI_SUPPORTED
    blufi_dh_pregen_start();
#endif

    return 0;
}

void blufi_security_deinit(void)
{
    if (blufi_sec == NULL) {
        return;
    }

#if !SOC_MPI_SUPPORTED
    blufi_dh_pregen_cleanup();
#endif

    /* Clean up all resources */
    blufi_cleanup_negotiation(true, true);

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));

    free(blufi_sec);
    blufi_sec = NULL;
}
