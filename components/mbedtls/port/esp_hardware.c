/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp_random.h"
#include <entropy_poll.h>
#include "psa/crypto.h"

#if ESP_TEE_BUILD
#include "mbedtls/private/ctr_drbg.h"
#include "mbedtls/platform_util.h"
#include "esp_cpu.h"
#include "esp_fault.h"
#include "hal/efuse_hal.h"
#include "hal/rng_ll.h"

#define CTR_DRBG_RESEED_INTERVAL 1024

static mbedtls_ctr_drbg_context s_ctr_drbg;

static int esp_tee_entropy_func(void *data, unsigned char *output, unsigned int len)
{
    (void)data;
    /* Explicitly enable the RNG */
    rng_ll_enable();
    esp_fill_random(output, len);
    return 0;
}

static void esp_tee_ctr_drbg_init(void)
{
    static bool s_ctr_drbg_initialized = false;

    if (!s_ctr_drbg_initialized) {
        /* Personalization data for CTR-DRBG seeding */
        struct {
            uint8_t mac[6];
            uint32_t random;
            uint32_t cycle_cnt;
        } data = {};

        rng_ll_enable();
        data.random = esp_random();
        efuse_hal_get_mac(data.mac);
        data.cycle_cnt = esp_cpu_get_cycle_count();

        mbedtls_ctr_drbg_init(&s_ctr_drbg);
        mbedtls_ctr_drbg_set_reseed_interval(&s_ctr_drbg, CTR_DRBG_RESEED_INTERVAL);

        int ret = mbedtls_ctr_drbg_seed(&s_ctr_drbg, esp_tee_entropy_func, NULL,
                                       (const unsigned char *)&data, sizeof(data));
        mbedtls_platform_zeroize(&data, sizeof(data));
        if (ret != 0) {
            abort();
        }
        ESP_FAULT_ASSERT(ret == 0);

        s_ctr_drbg_initialized = true;
    }

    ESP_FAULT_ASSERT(s_ctr_drbg_initialized);
}
#endif // ESP_TEE_BUILD

int mbedtls_hardware_poll( void *data,
                           unsigned char *output, size_t len, size_t *olen )
{
    esp_fill_random(output, len);
    *olen = len;
    return 0;
}

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    if (context == NULL || output == NULL || output_length == NULL) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

#if ESP_TEE_BUILD
    esp_tee_ctr_drbg_init();
    int ret = mbedtls_ctr_drbg_random(&s_ctr_drbg, output, output_size);
    if (ret != 0) {
        return PSA_ERROR_HARDWARE_FAILURE;
    }
    ESP_FAULT_ASSERT(ret == 0);
#else
    esp_fill_random(output, output_size);
#endif

    *output_length = output_size;
    return PSA_SUCCESS;
}
#endif // MBEDTLS_PSA_DRIVER_GET_ENTROPY
