/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_random.h"
#if CONFIG_BT_CONTROLLER_ENABLED || !CONFIG_BT_NIMBLE_ENABLED
#include "esp_bt.h"
#endif

#include "esp_blufi_api.h"
#include "blufi_example.h"

#include "psa/crypto.h"
#include "esp_crc.h"

/*
   The SEC_TYPE_xxx is for self-defined packet data type in the procedure of "BLUFI negotiate key"
   If user use other negotiation procedure to exchange(or generate) key, should redefine the type by yourself.
 */
#define SEC_TYPE_DH_PARAM_LEN   0x00
#define SEC_TYPE_DH_PARAM_DATA  0x01
#define SEC_TYPE_DH_P           0x02
#define SEC_TYPE_DH_G           0x03
#define SEC_TYPE_DH_PUBLIC      0x04


struct blufi_security {
#define DH_PARAM_LEN_MAX        1024
#define DH_SELF_PUB_KEY_LEN     256
    uint8_t  self_public_key[DH_SELF_PUB_KEY_LEN];
#define SHARE_KEY_LEN           256
    uint8_t  share_key[SHARE_KEY_LEN];
    size_t   share_len;
#define PSK_LEN                 16
    uint8_t  psk[PSK_LEN];
    uint8_t  *dh_param;
    int      dh_param_len;
    uint8_t  iv[16];
    psa_key_id_t aes_key;
};
static struct blufi_security *blufi_sec;

extern void btc_blufi_report_error(esp_blufi_error_state_t state);

void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free)
{
    if (data == NULL || len < 3) {
        BLUFI_ERROR("BLUFI Invalid data format");
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
        if (blufi_sec->dh_param) {
            free(blufi_sec->dh_param);
            blufi_sec->dh_param = NULL;
        }
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
        size_t p_len = (param[0] << 8) | param[1];
        param += 2 + p_len;

        size_t g_len = (param[0] << 8) | param[1];
        param += 2 + g_len;

        size_t pub_len = (param[0] << 8) | param[1];
        param += 2;
        ESP_LOGD("blfi", "P len %d, G len %d, pub len %d", p_len, g_len, pub_len);

        psa_key_type_t key_type = PSA_KEY_TYPE_DH_KEY_PAIR(PSA_DH_FAMILY_RFC7919);
        size_t key_bits = 3072;
        ESP_LOGI("blfi", "DH param len %d, bits %d", blufi_sec->dh_param_len, key_bits);
        psa_algorithm_t alg = PSA_ALG_FFDH;
        psa_key_attributes_t attributes = psa_key_attributes_init();
        psa_set_key_type(&attributes, key_type);
        psa_set_key_bits(&attributes, key_bits);
        psa_set_key_algorithm(&attributes, alg);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
        psa_key_id_t private_key = 0;
        psa_status_t status = psa_generate_key(&attributes, &private_key);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_generate_key failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            return;
        }
        psa_reset_key_attributes(&attributes);
        size_t public_key_len = 0;
        status = psa_export_public_key(private_key, blufi_sec->self_public_key, DH_SELF_PUB_KEY_LEN, &public_key_len);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_export_public_key failed %d\n", __func__, status);
            psa_destroy_key(private_key);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            return;
        }

        status = psa_raw_key_agreement(alg, private_key, param, pub_len, blufi_sec->share_key, SHARE_KEY_LEN, &blufi_sec->share_len);
        psa_destroy_key(private_key);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_raw_key_agreement failed %d\n", __func__, status);
            free(blufi_sec->dh_param);
            blufi_sec->dh_param = NULL;
            btc_blufi_report_error(ESP_BLUFI_DH_PARAM_ERROR);
            return;
        }

        size_t hash_length = 0;
        status = psa_hash_compute(PSA_ALG_MD5, blufi_sec->share_key, blufi_sec->share_len, blufi_sec->psk, PSK_LEN, &hash_length);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_hash_compute failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_CALC_MD5_ERROR);
            return;
        }

        // mbedtls_aes_setkey_enc(&blufi_sec->aes, blufi_sec->psk, PSK_LEN * 8);
        attributes = psa_key_attributes_init();
        psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&attributes, PSK_LEN * 8);
        psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
        status = psa_import_key(&attributes, blufi_sec->psk, PSK_LEN, &blufi_sec->aes_key);
        if (status != PSA_SUCCESS) {
            BLUFI_ERROR("%s psa_import_key failed %d\n", __func__, status);
            btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
            return;
        }

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
    }
}

int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    uint8_t iv0[16];

    if (!blufi_sec) {
        return -1;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_status_t status = psa_cipher_encrypt_setup(&operation, blufi_sec->aes_key, PSA_ALG_CFB);
    if (status != PSA_SUCCESS) {
        return -1;
    }

    memcpy(iv0, blufi_sec->iv, sizeof(blufi_sec->iv));
    iv0[0] = iv8;

    status = psa_cipher_set_iv(&operation, iv0, sizeof(iv0));
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    size_t encrypt_out_len = 0;
    status = psa_cipher_update(&operation, crypt_data, crypt_len, crypt_data, crypt_len, &encrypt_out_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    status = psa_cipher_finish(&operation, crypt_data, crypt_len, &encrypt_out_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    return encrypt_out_len;
}

int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len)
{
    uint8_t iv0[16];

    if (!blufi_sec) {
        return -1;
    }

    psa_cipher_operation_t operation = PSA_CIPHER_OPERATION_INIT;
    psa_status_t status = psa_cipher_decrypt_setup(&operation, blufi_sec->aes_key, PSA_ALG_CFB);
    if (status != PSA_SUCCESS) {
        return -1;
    }

    memcpy(iv0, blufi_sec->iv, sizeof(blufi_sec->iv));
    iv0[0] = iv8;

    status = psa_cipher_set_iv(&operation, iv0, sizeof(iv0));
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    size_t encrypt_out_len = 0;
    status = psa_cipher_update(&operation, crypt_data, crypt_len, crypt_data, crypt_len, &encrypt_out_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    status = psa_cipher_finish(&operation, crypt_data, crypt_len, &encrypt_out_len);
    if (status != PSA_SUCCESS) {
        psa_cipher_abort(&operation);
        return -1;
    }

    return crypt_len;
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

    memset(blufi_sec->iv, 0x0, sizeof(blufi_sec->iv));
    return 0;
}

void blufi_security_deinit(void)
{
    if (blufi_sec == NULL) {
        return;
    }
    if (blufi_sec->dh_param){
        free(blufi_sec->dh_param);
        blufi_sec->dh_param = NULL;
    }
    psa_destroy_key(blufi_sec->aes_key);

    memset(blufi_sec, 0x0, sizeof(struct blufi_security));

    free(blufi_sec);
    blufi_sec = NULL;
}
