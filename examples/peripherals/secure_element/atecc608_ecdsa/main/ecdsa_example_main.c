/*
 * atecc608a_ecdsa example
 *
 * SPDX-FileCopyrightText: 2006-2016 ARM Limited, All Rights Reserved
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2015-2021 Espressif Systems (Shanghai) CO LTD
 */

/* This is mbedtls boilerplate for library configuration */
#include "mbedtls/mbedtls_config.h"

/* System Includes*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"
#include "esp_log.h"

/* Cryptoauthlib includes */
#include "cryptoauthlib.h"
#include "mbedtls/atca_mbedtls_wrap.h"

/* mbedTLS includes */
#include "mbedtls/platform.h"
#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/pk.h"

static const char *TAG = "atecc_example";
/* globals for mbedtls RNG */
static mbedtls_entropy_context entropy;
static mbedtls_ctr_drbg_context ctr_drbg;

static int configure_mbedtls_rng(void)
{
    int ret;
    const char * seed = "some random seed string";
    mbedtls_ctr_drbg_init(&ctr_drbg);

    ESP_LOGI(TAG, "Seeding the random number generator...");

    mbedtls_entropy_init(&entropy);
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
        (const unsigned char *)seed, strlen(seed));
    if (ret != 0) {
        ESP_LOGI(TAG, " failed  ! mbedtls_ctr_drbg_seed returned %d", ret);
    } else {
        ESP_LOGI(TAG, " ok");
    }
    return ret;
}

static void close_mbedtls_rng(void)
{
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

/* An example hash */
static unsigned char hash[32] = {
    0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
    0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const uint8_t public_key_x509_header[] = {
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04
};

static void print_public_key(uint8_t *pubkey)
{
    uint8_t buf[128];
    uint8_t * tmp;
    size_t buf_len = sizeof(buf);

    /* Calculate where the raw data will fit into the buffer */
    tmp = buf + sizeof(buf) - ATCA_PUB_KEY_SIZE - sizeof(public_key_x509_header);

    /* Copy the header */
    memcpy(tmp, public_key_x509_header, sizeof(public_key_x509_header));

    /* Copy the key bytes */
    memcpy(tmp + sizeof(public_key_x509_header), pubkey, ATCA_PUB_KEY_SIZE);

    /* Convert to base 64 */
    (void)atcab_base64encode(tmp, ATCA_PUB_KEY_SIZE + sizeof(public_key_x509_header), (char*)buf, &buf_len);

    /* Add a null terminator */
    buf[buf_len] = '\0';

    /* Print out the key */
    ESP_LOGI(TAG, "\r\n-----BEGIN PUBLIC KEY-----\r\n%s\r\n-----END PUBLIC KEY-----", buf);
}

static int atca_ecdsa_test(void)
{
    mbedtls_pk_context pkey;
    int ret;
    unsigned char buf[MBEDTLS_MPI_MAX_SIZE];
    size_t olen = 0;

    /* ECDSA Sign/Verify */

#ifdef MBEDTLS_ECDSA_SIGN_ALT
    /* Convert to an mbedtls key */
    ESP_LOGI(TAG,  " Using a hardware private key ..." );
    ret = atca_mbedtls_pk_init(&pkey, 0);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed !  atca_mbedtls_pk_init returned %02x", ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok");
#else
    ESP_LOGI(TAG,  " Generating a software private key ..." );
    mbedtls_pk_init(&pkey);
    ret = mbedtls_pk_setup(&pkey,
            mbedtls_pk_info_from_type( MBEDTLS_PK_ECDSA ));
    if (ret != 0) {
        ESP_LOGI(TAG,  " failed !  mbedtls_pk_setup returned -0x%04x", -ret );
        goto exit;
    }

    ret = mbedtls_ecp_gen_key( MBEDTLS_ECP_DP_SECP256R1,
                                   mbedtls_pk_ec( pkey ),
                                   mbedtls_ctr_drbg_random, &ctr_drbg );
    if (ret != 0) {
        ESP_LOGI(TAG,  " failed !  mbedtls_ecp_gen_key returned -0x%04x", -ret );
        goto exit;
    }
    ESP_LOGI(TAG, " ok");
#endif

    ESP_LOGI(TAG, " Generating ECDSA Signature...");
    ret = mbedtls_pk_sign(&pkey, MBEDTLS_MD_SHA256, hash, 0, buf, MBEDTLS_MPI_MAX_SIZE, &olen,
        mbedtls_ctr_drbg_random, &ctr_drbg);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed ! mbedtls_pk_sign returned -0x%04x", -ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok");

    ESP_LOGI(TAG, " Verifying ECDSA Signature...");
    ret = mbedtls_pk_verify(&pkey, MBEDTLS_MD_SHA256, hash, 0,
        buf, olen);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed ! mbedtls_pk_verify returned -0x%04x", -ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok");

exit:
    fflush(stdout);
    return ret;
}

void app_main(void)
{
    int ret = 0;
    bool lock;
    uint8_t buf[ATCA_ECC_CONFIG_SIZE];
    uint8_t pubkey[ATCA_PUB_KEY_SIZE];

    /* Initialize the mbedtls library */
    ret = configure_mbedtls_rng();
#ifdef CONFIG_ATECC608A_TNG
    ESP_LOGI(TAG, "  . Initialize the ATECC interface for Trust & GO ...");
    cfg_ateccx08a_i2c_default.atcai2c.address = 0x6A;
#elif CONFIG_ATECC608A_TFLEX /* CONFIG_ATECC608A_TNGO */
    ESP_LOGI(TAG, "  . Initialize the ATECC interface for TrustFlex ...");
    cfg_ateccx08a_i2c_default.atcai2c.address = 0x6C;
#elif CONFIG_ATECC608A_TCUSTOM /* CONFIG_ATECC608A_TFLEX */
    ESP_LOGI(TAG, "  . Initialize the ATECC interface for TrustCustom ...");
    /* Default slave address is same as that of TCUSTOM ATECC608A chips */
#endif /* CONFIG_ATECC608A_TCUSTOM */
    ret = atcab_init(&cfg_ateccx08a_i2c_default);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed ! atcab_init returned %02x", ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok");

    lock = 0;
    ESP_LOGI(TAG, " Check the data zone lock status...");
    ret = atcab_is_locked(LOCK_ZONE_DATA, &lock);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed\n  ! atcab_is_locked returned %02x", ret);
        goto exit;
    }

    if (lock) {
        ESP_LOGI(TAG, " ok: locked");
    } else {
        ESP_LOGE(TAG, "unlocked, please lock(configure) the ATECC608A chip with help of esp_cryptoauth_utility and try again");
        goto exit;
    }

    ESP_LOGI(TAG, " Get the device info (type)...");
    ret = atcab_info(buf);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed\n  ! atcab_info returned %02x", ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok: %02x %02x", buf[2], buf[3]);

    ESP_LOGI(TAG, " Get the public key...");
    ret = atcab_get_pubkey(0, pubkey);
    if (ret != 0) {
        ESP_LOGI(TAG, " failed\n  ! atcab_get_pubkey returned %02x", ret);
        goto exit;
    }
    ESP_LOGI(TAG, " ok");
    print_public_key(pubkey);

    /* Perform a Sign/Verify Test */
    ret = atca_ecdsa_test();
    if (ret != 0) {
        ESP_LOGE(TAG, " ECDSA sign/verify failed");
        goto exit;
    }

exit:
    fflush(stdout);
    close_mbedtls_rng();

}
