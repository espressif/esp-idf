/*
 Tests for the Wi-Fi
*/
#include "string.h"
#include "esp_system.h"
#include "unity.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "soc/soc_caps.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/wifi.h"

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32H2)
//IDF-5046

#include "esp_phy_init.h"

//Function just extern, need not test
#if SOC_BT_SUPPORTED
extern void bt_bb_init_cmplx(void);
#endif
extern void IRAM_ATTR spi_flash_disable_interrupts_caches_and_other_cpu(void);
extern void IRAM_ATTR spi_flash_enable_interrupts_caches_and_other_cpu(void);

//Functions in librtc.a called by WIFI or Blutooth directly in ISR
#if SOC_BT_SUPPORTED
extern void bt_track_pll_cap(void);
#endif


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
#if CONFIG_ESP_WIFI_ENABLED
    esp_phy_enable(PHY_MODEM_WIFI);
#endif
#if CONFIG_BT_ENABLED
    esp_phy_enable(PHY_MODEM_BT);
#endif
#if CONFIG_IEEE802154_ENABLED
    esp_phy_enable(PHY_MODEM_IEEE802154);
#endif
    //must run here, not blocking in above code
    TEST_ASSERT(1);
    nvs_flash_deinit();
}

static IRAM_ATTR void test_phy_rtc_cache_task(void *arg)
{
    //power up wifi and bt mac bb power domain
    esp_wifi_power_domain_on();
#if CONFIG_IDF_TARGET_ESP32C6
    modem_clock_module_enable(PERIPH_PHY_MODULE);
#endif // CONFIG_IDF_TARGET_ESP32C6
    test_phy_rtc_init();

#if CONFIG_IDF_TARGET_ESP32
    extern void force_wifi_mode(int);
    extern void unforce_wifi_mode(void);

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
#endif //CONFIG_IDF_TARGET_ESP32

#if SOC_BT_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32
    /* Only esp32 will call bt_track_pll_cap() in the interrupt
        handler, other chips will call this function in the task
     */
    ESP_LOGI(TAG, "Test bt_track_pll_cap()...");
    spi_flash_disable_interrupts_caches_and_other_cpu();
    bt_track_pll_cap();
    spi_flash_enable_interrupts_caches_and_other_cpu();

    extern void bt_bb_init_cmplx_reg(void);
    ESP_LOGI(TAG, "Test bt_bb_init_cmplx_reg()...");
    spi_flash_disable_interrupts_caches_and_other_cpu();
    bt_bb_init_cmplx_reg();
    spi_flash_enable_interrupts_caches_and_other_cpu();
#endif //CONFIG_IDF_TARGET_ESP32

#endif //SOC_BT_SUPPORTED

#if CONFIG_IDF_TARGET_ESP32C6
    modem_clock_module_disable(PERIPH_PHY_MODULE);
#endif // CONFIG_IDF_TARGET_ESP32C6
    //power down wifi and bt mac bb power domain
    esp_wifi_power_domain_off();

    TEST_ASSERT( xSemaphoreGive(semphr_done) );

    vTaskDelete(NULL);
}

TEST_CASE("Test PHY/RTC functions called when cache is disabled", "[phy_rtc][cache_disabled]")
{
    semphr_done = xSemaphoreCreateCounting(1, 0);

    xTaskCreatePinnedToCore(test_phy_rtc_cache_task, "phy_rtc_test_task", 3072,
                            NULL, configMAX_PRIORITIES-1, NULL, 0);

    TEST_ASSERT( xSemaphoreTake(semphr_done, portMAX_DELAY) );

    vSemaphoreDelete(semphr_done);
}
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C6)
