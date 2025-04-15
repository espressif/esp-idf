/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "string.h"
#include <inttypes.h>
#include "unity.h"
#include "utils/common.h"
#include "mbedtls/pkcs5.h"
#include "crypto/sha1.h"
#include "test_wpa_supplicant_common.h"

#define PMK_LEN 32
#define NUM_ITERATIONS 15
#define MIN_PASSPHARSE_LEN 8

void fastpbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                          const uint8_t *salt, size_t nsalt,
                          uint32_t iterations,
                          uint8_t *out, size_t nout);

int64_t esp_timer_get_time(void);

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

    /* Compare fast PBKDF output with mbedtls pbkdf2 function's output */
    pbkdf2_sha1("espressif2", (uint8_t *)"espressif2", strlen("espressif2"), 4096, PMK, PMK_LEN);
    mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA1, (const unsigned char *) "espressif2",
                                  strlen("espressif2"), (const unsigned char *)"espressif2",
                                  strlen("espressif2"), 4096, PMK_LEN, expected_pmk);
    TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);

    int64_t total_time_pbkdf2 = 0;  // Variable to store total time for pbkdf2_sha1
    int64_t total_time_mbedtls = 0;
    int64_t total_time_fast_pbkdf2 = 0;
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
        int64_t start_time = esp_timer_get_time();
        pbkdf2_sha1((char *)passphrase, ssid, ssid_len, 4096, PMK, PMK_LEN);
        int64_t end_time = esp_timer_get_time();
        total_time_pbkdf2 += (end_time - start_time);
        start_time = esp_timer_get_time();
        mbedtls_pkcs5_pbkdf2_hmac_ext(MBEDTLS_MD_SHA1, (const unsigned char *) passphrase,
                                      strlen((char *)passphrase), (const unsigned char *)ssid,
                                      ssid_len, 4096, PMK_LEN, expected_pmk);
        end_time = esp_timer_get_time();
        total_time_mbedtls += (end_time - start_time);
        /* Dump values if fails */
        if (memcmp(PMK, expected_pmk, PMK_LEN) != 0) {
            ESP_LOG_BUFFER_HEXDUMP("passphrase", passphrase, passphrase_len, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("ssid", ssid, ssid_len, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("PMK", PMK, PMK_LEN, ESP_LOG_INFO);
            ESP_LOG_BUFFER_HEXDUMP("expected_pmk", expected_pmk, PMK_LEN, ESP_LOG_INFO);
        }
        TEST_ASSERT(memcmp(PMK, expected_pmk, PMK_LEN) == 0);

        start_time = esp_timer_get_time();
        fastpbkdf2_hmac_sha1((const u8 *)passphrase, os_strlen((char *)passphrase), ssid, ssid_len, 4096, PMK, PMK_LEN);
        end_time = esp_timer_get_time();
        total_time_fast_pbkdf2 += (end_time - start_time);
    }

    // Calculate average time for pbkdf2_sha1
    int64_t avg_time_pbkdf2 = total_time_pbkdf2 / NUM_ITERATIONS;
    // Calculate average time for mbedtls_pkcs5_pbkdf2_hmac_ext
    int64_t avg_time_mbedtls = total_time_mbedtls / NUM_ITERATIONS;
    int64_t avg_time_fast = total_time_fast_pbkdf2 / NUM_ITERATIONS;

    // Log average times
    ESP_LOGI("Timing", "Average time for pbkdf2_sha1: %lld microseconds", avg_time_pbkdf2);
    ESP_LOGI("Timing", "Average time for fast_pbkdf2_sha1: %lld microseconds", avg_time_fast);
    ESP_LOGI("Timing", "Average time for mbedtls_pkcs5_pbkdf2_hmac_ext: %lld microseconds", avg_time_mbedtls);
}
#endif
