/*
 Tests for the Wi-Fi
*/
#include "string.h"
#include "esp_system.h"
#include "unity.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "esp_phy_init.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>


//Function just extern, need not test
extern void bt_bb_init_cmplx(void);
extern void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu();
extern void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu();

//Functions in librtc.a called by WIFI or Blutooth directly in ISR
extern void bt_bb_init_cmplx_reg(void);
extern void force_wifi_mode(int);
extern void unforce_wifi_mode(void);
extern void bt_track_pll_cap(void);

static const char* TAG = "test_phy_rtc";

static SemaphoreHandle_t semphr_done;

//Functions in libphy.a called by WIFI or Blutooth directly in ISR
static void test_phy_rtc_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGI(TAG, "no free pages or nvs version mismatch, erase..");
        TEST_ESP_OK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    TEST_ESP_OK(ret);

    esp_phy_load_cal_and_init(PHY_BT_MODULE);
    esp_phy_load_cal_and_init(PHY_WIFI_MODULE);

    //must run here, not blocking in above code
    TEST_ASSERT(1);
}

static IRAM_ATTR void test_phy_rtc_cache_task(void *arg)
{
    test_phy_rtc_init();

    ESP_LOGI(TAG, "Test bt_bb_init_cmplx_reg()...");
    spi_flash_disable_interrupts_caches_and_other_cpu();
    bt_bb_init_cmplx_reg();
    spi_flash_enable_interrupts_caches_and_other_cpu();

    for (int i = 0; i < 2; i++) {
        ESP_LOGI(TAG, "Test force_wifi_mode(%d)...", i);
        spi_flash_disable_interrupts_caches_and_other_cpu();
        force_wifi_mode(i);
        spi_flash_enable_interrupts_caches_and_other_cpu();

        ESP_LOGI(TAG, "Test unforce_wifi_mode()...");
        spi_flash_disable_interrupts_caches_and_other_cpu();
        unforce_wifi_mode();
        spi_flash_enable_interrupts_caches_and_other_cpu();
    }

    ESP_LOGI(TAG, "Test bt_track_pll_cap()...");
    spi_flash_disable_interrupts_caches_and_other_cpu();
    bt_track_pll_cap();
    spi_flash_enable_interrupts_caches_and_other_cpu();

    TEST_ASSERT( xSemaphoreGive(semphr_done) );

    vTaskDelete(NULL);
}

TEST_CASE("Test PHY/RTC functions called when cache is disabled", "[phy_rtc][cache_disabled]")
{
    semphr_done = xSemaphoreCreateCounting(1, 0);

    xTaskCreatePinnedToCore(test_phy_rtc_cache_task, "phy_rtc_test_task", 2048,
                            NULL, configMAX_PRIORITIES-1, NULL, 0);

    TEST_ASSERT( xSemaphoreTake(semphr_done, portMAX_DELAY) );

    vSemaphoreDelete(semphr_done);
}
