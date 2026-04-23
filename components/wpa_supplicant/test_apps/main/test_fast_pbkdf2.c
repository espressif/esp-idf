/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "string.h"
#include <inttypes.h>
#include "unity.h"
#include "utils/common.h"
#include "psa/crypto.h"
#include "crypto/sha1.h"
#include "test_wpa_supplicant_common.h"

#define PMK_LEN 32
#define NUM_ITERATIONS 2
#define MIN_PASSPHARSE_LEN 8

#ifdef CONFIG_FAST_PBKDF2
void fastpbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                          const uint8_t *salt, size_t nsalt,
                          uint32_t iterations,
                          uint8_t *out, size_t nout);
#endif

#ifdef CONFIG_FAST_PSK
int esp_fast_psk(const char *password, size_t password_len, const uint8_t *ssid,
                 size_t ssid_len, size_t iterations, uint8_t *output, size_t output_len);
#endif

int64_t esp_timer_get_time(void);

static int psa_pbkdf2_sha1(const unsigned char *password, size_t plen,
                           const unsigned char *salt, size_t slen,
                           unsigned int iterations,
                           uint32_t key_length, unsigned char *output)
{
    psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_status_t status;

    status = psa_key_derivation_setup(&op, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1));
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    status = psa_key_derivation_input_integer(&op, PSA_KEY_DERIVATION_INPUT_COST, iterations);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_SALT, salt, slen);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_PASSWORD, password, plen);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    status = psa_key_derivation_output_bytes(&op, output, key_length);

cleanup:
    psa_key_derivation_abort(&op);
    return status == PSA_SUCCESS ? 0 : -1;
}

#if defined(CONFIG_MBEDTLS_SHA1_C) || defined(CONFIG_MBEDTLS_HARDWARE_SHA)
TEST_CASE("Test pbkdf2", "[crypto-pbkdf2]")
{
    set_leak_threshold(130);

    uint8_t PMK[PMK_LEN];
    uint8_t ssid_len;
    uint8_t passphrase_len;
    uint8_t ssid[MAX_SSID_LEN];
    uint8_t passphrase[MAX_PASSPHRASE_LEN];
    uint8_t expected_pmk1[PMK_LEN] = {
        0xe7, 0x90, 0xd0, 0x65, 0x67, 0xf0, 0xbf, 0xca, 0xca, 0x10, 0x88, 0x0b, 0x85, 0xb2, 0x33, 0xe5,
        0xe1, 0xd5, 0xe5, 0xb8, 0xd0, 0xfd, 0x94, 0x60, 0x56, 0x95, 0x5e, 0x41, 0x5a, 0x7f, 0xfa, 0xfa
    };

    uint8_t expected_pmk[PMK_LEN];

    /* Compare Fast PBKDF output with expected output*/
    pbkdf2_sha1("espressif", (uint8_t *)"espressif", strlen("espressif"), 4096, PMK, PMK_LEN);
    TEST_ASSERT(memcmp(PMK, expected_pmk1, PMK_LEN) == 0);

    /* Compare fast PBKDF output with PSA pbkdf2 function's output */
    pbkdf2_sha1("espressif2", (uint8_t *)"espressif2", strlen("espressif2"), 4096, PMK, PMK_LEN);
    psa_pbkdf2_sha1((const unsigned char *) "espressif2", strlen("espressif2"),
                    (const unsigned char *)"espressif2", strlen("espressif2"),
                    4096, PMK_LEN, expected_pmk);
    TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);

    int64_t total_time_pbkdf2 = 0;
    int64_t total_time_mbedtls = 0;
#ifdef CONFIG_FAST_PBKDF2
    int64_t total_time_fast_pbkdf2 = 0;
#endif
#ifdef CONFIG_FAST_PSK
    int64_t total_time_fast_psk = 0;
#endif
    int i;
    for (i = 0; i < NUM_ITERATIONS; i++) {
        /* Calculate PMK using random ssid and passphrase and compare */
        os_memset(ssid, 0, MAX_SSID_LEN);
        os_memset(passphrase, 0, MAX_PASSPHRASE_LEN);
        ssid_len = os_random();
        ssid_len %= MAX_SSID_LEN;

        os_get_random(ssid, ssid_len);

        passphrase_len = os_random();
        passphrase_len %= MAX_PASSPHRASE_LEN;
        if (passphrase_len < MIN_PASSPHARSE_LEN) {
            passphrase_len += MIN_PASSPHARSE_LEN;
        }

        os_get_random(passphrase, passphrase_len);

        /* Reference: PSA PBKDF2 */
        int64_t start_time = esp_timer_get_time();
        psa_pbkdf2_sha1((const unsigned char *) passphrase, strlen((char *)passphrase),
                        (const unsigned char *)ssid, ssid_len,
                        4096, PMK_LEN, expected_pmk);
        int64_t end_time = esp_timer_get_time();
        total_time_mbedtls += (end_time - start_time);

        /* pbkdf2_sha1 (dispatches to fastpsk or fastpbkdf2 per chip) */
        start_time = esp_timer_get_time();
        pbkdf2_sha1((char *)passphrase, ssid, ssid_len, 4096, PMK, PMK_LEN);
        end_time = esp_timer_get_time();
        total_time_pbkdf2 += (end_time - start_time);

        if (memcmp(PMK, expected_pmk, PMK_LEN) != 0) {
            ESP_LOG_BUFFER_HEXDUMP("passphrase", passphrase, passphrase_len, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("ssid", ssid, ssid_len, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("PMK", PMK, PMK_LEN, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("expected_pmk", expected_pmk, PMK_LEN, ESP_LOG_INFO);
        }
        TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);

#ifdef CONFIG_FAST_PSK
        /* esp_fast_psk: HW SHA accelerated (fastpsk.c) */
        start_time = esp_timer_get_time();
        esp_fast_psk((char *)passphrase, os_strlen((char *)passphrase),
                     ssid, ssid_len, 4096, PMK, PMK_LEN);
        end_time = esp_timer_get_time();
        total_time_fast_psk += (end_time - start_time);
        TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);
#endif

#ifdef CONFIG_FAST_PBKDF2
        /* fastpbkdf2: SW SHA optimized (fastpbkdf2.c) */
        start_time = esp_timer_get_time();
        fastpbkdf2_hmac_sha1((const u8 *)passphrase, os_strlen((char *)passphrase),
                             ssid, ssid_len, 4096, PMK, PMK_LEN);
        end_time = esp_timer_get_time();
        total_time_fast_pbkdf2 += (end_time - start_time);
        TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);
#endif
    }

    ESP_LOGI("Timing", "Average time for pbkdf2_sha1:       %lld us", total_time_pbkdf2 / NUM_ITERATIONS);
#ifdef CONFIG_FAST_PSK
    ESP_LOGI("Timing", "Average time for esp_fast_psk (HW):  %lld us", total_time_fast_psk / NUM_ITERATIONS);
#endif
#ifdef CONFIG_FAST_PBKDF2
    ESP_LOGI("Timing", "Average time for fastpbkdf2 (SW):    %lld us", total_time_fast_pbkdf2 / NUM_ITERATIONS);
#endif
    ESP_LOGI("Timing", "Average time for psa_pbkdf2:          %lld us", total_time_mbedtls / NUM_ITERATIONS);
}
#endif
