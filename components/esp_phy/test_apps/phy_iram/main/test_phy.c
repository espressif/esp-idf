/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "esp_system.h"
#include "unity.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "esp_phy_init.h"

extern void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu_no_os(void);
extern void IRAM_ATTR spi_flash_enable_interrupts_caches_no_os(void);

static const char* TAG = "test_phy_iram";

static SemaphoreHandle_t semphr_done;

//Functions in libphy.a called by WIFI or Bluetooth directly in ISR
static void test_phy_rtc_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "no free pages or nvs version mismatch, erase..");
        TEST_ESP_OK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    TEST_ESP_OK(ret);

#if CONFIG_IEEE802154_ENABLED
    esp_phy_enable(PHY_MODEM_IEEE802154);
#endif
    //must run here, not blocking in above code
    TEST_ASSERT(1);
    nvs_flash_deinit();
}

static IRAM_ATTR void test_phy_rtc_cache_task(void *arg)
{
    test_phy_rtc_init();

#if CONFIG_IDF_TARGET_ESP32H2
    /* Check if esp_phy_enable is fully placed in IRAM,
        requires CONFIG_ESP_PHY_ENABLE_VERSION_PRINT to be disabled.
        Do not suspend OS scheduler because of _lock_acquire.
     */
    spi_flash_disable_interrupts_caches_and_other_cpu_no_os();
    esp_phy_disable(PHY_MODEM_IEEE802154);
    esp_phy_enable(PHY_MODEM_IEEE802154);
    spi_flash_enable_interrupts_caches_no_os();
#endif

    TEST_ASSERT( xSemaphoreGive(semphr_done) );

    vTaskDelete(NULL);
}

TEST_CASE("Test PHY enable/disable functions with cache disabled", "[phy_iram][cache_disabled]")
{
    semphr_done = xSemaphoreCreateCounting(1, 0);

    xTaskCreatePinnedToCore(test_phy_rtc_cache_task, "phy_rtc_test_task", 3072,
                            NULL, configMAX_PRIORITIES-1, NULL, 0);

    TEST_ASSERT( xSemaphoreTake(semphr_done, portMAX_DELAY) );

    vSemaphoreDelete(semphr_done);
}
