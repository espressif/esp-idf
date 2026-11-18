/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "unity.h"
#include "esp_private/sleep_sys_periph.h"
#include "esp_private/sleep_retention.h"
#include "esp_sleep.h"
#include "esp_private/sleep_cpu.h"

#if CONFIG_SPIRAM && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#if CONFIG_IDF_TARGET_ESP32C5
#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"
#endif
#endif

ESP_LOG_ATTR_TAG(TAG, "retention");

#if CONFIG_SPIRAM && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && CONFIG_PM_ESP_SLEEP_POWER_DOWN_CPU
TEST_CASE("retention: PSRAM writeback stays intact after aborted light sleep", "[retention]")
{
#if CONFIG_IDF_TARGET_ESP32C5
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102)) {
        TEST_IGNORE_MESSAGE("ESP32-C5 below v1.2 cannot power down the TOP domain");
    }
#endif
    const size_t buf_size = 256;
    const size_t word_count = buf_size / sizeof(uint32_t);
    uint32_t *buf = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT | MALLOC_CAP_CACHE_ALIGNED);
    TEST_ASSERT_NOT_NULL(buf);
    /* Flush allocator metadata while MR4 is still valid so only buf is dirty later. */
    TEST_ESP_OK(esp_cache_msync((void *)((uintptr_t)buf & ~0xfffU), 0x2000, ESP_CACHE_MSYNC_FLAG_DIR_C2M));

    esp_sleep_context_t sleep_ctx = {};
    esp_sleep_set_sleep_context(&sleep_ctx);
    TEST_ESP_OK(sleep_cpu_configure(true));

    /* First light sleep since boot aborts before entering PSRAM halfsleep. */
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1));
    esp_err_t err = esp_light_sleep_start();
    TEST_ASSERT_EQUAL(ESP_ERR_SLEEP_TOO_SHORT_SLEEP_DURATION, err);
    TEST_ASSERT_EQUAL(ESP_ERR_SLEEP_TOO_SHORT_SLEEP_DURATION, sleep_ctx.sleep_request_result);
    TEST_ASSERT_EQUAL(BIT(ESP_SLEEP_WAKEUP_UNDEFINED), esp_sleep_get_wakeup_causes());

    /* Dirty PSRAM lines in data cache so sleep entry writes them back to PSRAM. */
    volatile uint32_t *vbuf = (volatile uint32_t *)buf;
    for (size_t i = 0; i < word_count; i++) {
        vbuf[i] = 0xA5A50000U | (uint32_t)i;
    }

    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(50 * 1000));
    err = esp_light_sleep_start();
    esp_sleep_set_sleep_context(NULL);
    TEST_ESP_OK(sleep_cpu_configure(false));

    TEST_ESP_OK(err);
    TEST_ASSERT_EQUAL(ESP_OK, sleep_ctx.sleep_request_result);
    TEST_ASSERT_EQUAL_HEX32(PMU_SLEEP_PD_CPU, sleep_ctx.sleep_flags & PMU_SLEEP_PD_CPU);
    TEST_ASSERT_EQUAL_HEX32(PMU_SLEEP_PD_TOP, sleep_ctx.sleep_flags & PMU_SLEEP_PD_TOP);
    TEST_ASSERT_NOT_EQUAL(0, esp_sleep_get_wakeup_causes() & BIT(ESP_SLEEP_WAKEUP_TIMER));

    /* On chips with a retained L2 cache (e.g. ESP32-P4), sleep entry only flushes L1 to L2;
     * C2M | INVALIDATE flushes any remaining dirty L2 lines to PSRAM (no-op on clean lines
     * already written back during sleep entry) and forces subsequent reads from PSRAM. */
    TEST_ESP_OK(esp_cache_msync(buf, buf_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE));
    for (size_t i = 0; i < word_count; i++) {
        TEST_ASSERT_EQUAL_HEX32(0xA5A50000U | (uint32_t)i, vbuf[i]);
    }
    heap_caps_free(buf);
}
#endif

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
TEST_CASE("retention: can go to retention", "[retention]")
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
    sleep_cpu_configure(true);

    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);
    vTaskDelay(1000/portTICK_PERIOD_MS);

    ESP_LOGI(TAG, "Going to sleep...");
    esp_light_sleep_start();

    ESP_LOGI(TAG, "After wakeup");
    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);

    sleep_cpu_configure(false);
}
#endif
