/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "unity.h"
#include "esp_random.h"

#if SOC_LIGHT_SLEEP_SUPPORTED
#include "esp_sleep.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_pmu.h"
#endif

/* Note: these are just sanity tests, not the same as
   entropy tests
*/

TEST_CASE("call esp_random()", "[random]")
{
    const size_t NUM_RANDOM = 128; /* in most cases this is massive overkill */

    uint32_t zeroes = UINT32_MAX;
    uint32_t ones = 0;
    for (int i = 0; i < NUM_RANDOM - 1; i++) {
        uint32_t r = esp_random();
        ones |= r;
        zeroes &= ~r;
    }

    /* assuming a 'white' random distribution, we can expect
       usually at least one time each bit will be zero and at
       least one time each will be one. Statistically this
       can still fail, just *very* unlikely to. */
    TEST_ASSERT_EQUAL_HEX32(0, zeroes);
    TEST_ASSERT_EQUAL_HEX32(UINT32_MAX, ones);
}

TEST_CASE("call esp_fill_random()", "[random]")
{
    const size_t NUM_BUF = 200;
    const size_t BUF_SZ = 16;
    uint8_t buf[NUM_BUF][BUF_SZ];
    uint8_t zero_buf[BUF_SZ];
    uint8_t one_buf[BUF_SZ];

    bzero(buf, sizeof(buf));
    bzero(one_buf, sizeof(zero_buf));
    memset(zero_buf, 0xFF, sizeof(one_buf));

    for (int i = 0; i < NUM_BUF; i++) {
        esp_fill_random(buf[i], BUF_SZ);
    }
    /* No two 128-bit buffers should be the same
       (again, statistically this could happen but it's very unlikely) */
    for (int i = 0; i < NUM_BUF; i++) {
        for (int j = 0; j < NUM_BUF; j++) {
            if (i != j) {
                TEST_ASSERT_NOT_EQUAL(0, memcmp(buf[i], buf[j], BUF_SZ));
            }
        }
    }

    /* Do the same all bits are zero and one at least once test across the buffers */
    for (int i = 0; i < NUM_BUF; i++) {
        for (int x = 0; x < BUF_SZ; x++) {
            zero_buf[x] &= ~buf[i][x];
            one_buf[x] |= buf[i][x];
        }
    }
    for (int x = 0; x < BUF_SZ; x++) {
        TEST_ASSERT_EQUAL_HEX8(0, zero_buf[x]);
        TEST_ASSERT_EQUAL_HEX8(0xFF, one_buf[x]);
    }
}

TEST_CASE("verify esp_random() bit balance 0/1 ratio", "[random]")
{
#if SOC_LIGHT_SLEEP_SUPPORTED
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);
    printf("go to light sleep for 1 seconds\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    TEST_ESP_OK(esp_light_sleep_start());
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(PMU_SLEEP_PD_TOP, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    esp_sleep_set_sleep_context(NULL);
    printf("Waked up! Let's see if esp_random can still work correctly...\r\n");
#if CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
#endif

    const size_t NUM_RANDOM = 50000; /* Need enough samples for statistical significance */
    const float EXPECTED_RATIO = 0.5f; /* Expected ratio of 1s */
    const float TOLERANCE = 0.01f; /* Allow 1% deviation (49%-51%) */
    uint32_t bit_counts[32] = {0}; /* Count of 1s for each bit position */
    /* Collect random numbers and count 1s in each bit position */
    for (int i = 0; i < NUM_RANDOM; i++) {
        uint32_t r = esp_random();
        for (int bit = 0; bit < 32; bit++) {
            if (r & (1U << bit)) {
                bit_counts[bit]++;
            }
        }
    }
    /* Verify each bit position has approximately 50% ones */
    printf("\nBit balance statistics (total samples: %zu):\n", NUM_RANDOM);
    printf("Bit | 1s count | 0s count | 1s ratio | 0s ratio | Deviation\n");
    printf("----|----------|----------|----------|----------|----------\n");
    for (int bit = 0; bit < 32; bit++) {
        uint32_t ones_count = bit_counts[bit];
        uint32_t zeros_count = NUM_RANDOM - ones_count;
        float ratio_ones = (float)ones_count / NUM_RANDOM;
        float ratio_zeros = (float)zeros_count / NUM_RANDOM;
        float deviation = fabs(ratio_ones - EXPECTED_RATIO);

        printf("%3d | %8" PRIu32 " | %8" PRIu32 " | %8.4f | %8.4f | %9.4f\n",
               bit, ones_count, zeros_count, ratio_ones, ratio_zeros, deviation);

        /* Check if ratio is within tolerance */
        TEST_ASSERT_LESS_THAN_FLOAT(TOLERANCE, deviation);
    }
}
