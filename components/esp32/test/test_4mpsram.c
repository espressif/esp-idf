#include "esp_heap_caps.h"
#include "unity.h"
#include "esp_log.h"
#include "driver/spi_common_internal.h"
#include "sdkconfig.h"

static const char TAG[] = "test_psram";

#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
static void test_psram_content(void)
{
    const int test_size = 2048;
    uint32_t *test_area = heap_caps_malloc(test_size, MALLOC_CAP_SPIRAM);

    size_t p;
    size_t s=test_size;
    int errct=0;
    int initial_err=-1;
    for (p=0; p<(s/sizeof(int)); p+=4) {
        test_area[p]=p^0xAAAAAAAA;
    }
    for (p=0; p<(s/sizeof(int)); p+=4) {
        if (test_area[p]!=(p^0xAAAAAAAA)) {
            errct++;
            if (errct==1) initial_err=p*4;
        }
    }
    if (errct) {
        ESP_LOGE(TAG, "SPI SRAM memory test fail. %d/%d writes failed, first @ %p\n", errct, s/32, initial_err+test_area);
        TEST_FAIL();
    } else {
        ESP_LOGI(TAG, "SPI SRAM memory test OK");
    }

    free(test_area);
}
#endif

// NOTE: this unit test rely on the config that PSRAM of 8MB is used only when CONFIG_SPIRAM_BNKSWITCH_ENABLE is set
TEST_CASE("can use spi when not being used by psram", "[psram_4m]")
{
    spi_host_device_t host;
#if !CONFIG_ESP32_SPIRAM_SUPPORT || !CONFIG_SPIRAM_SPEED_80M || CONFIG_SPIRAM_BANKSWITCH_ENABLE
    //currently all 8M psram don't need more SPI peripherals
    host = -1;
#elif CONFIG_SPIRAM_OCCUPY_HSPI_HOST
    host = HSPI_HOST;
#elif CONFIG_SPIRAM_OCCUPY_VSPI_HOST
    host = VSPI_HOST;
#endif

    bool claim_hspi = spicommon_periph_claim(HSPI_HOST, "ut-hspi");
    if (claim_hspi) ESP_LOGI(TAG, "HSPI claimed.");

    bool claim_vspi = spicommon_periph_claim(VSPI_HOST, "ut-vspi");
    if (claim_vspi) ESP_LOGI(TAG, "VSPI claimed.");

    if (host == HSPI_HOST) {
        TEST_ASSERT(claim_hspi==false);
        TEST_ASSERT(claim_vspi==true);
    } else if (host == VSPI_HOST) {
        TEST_ASSERT(claim_vspi==false);
        TEST_ASSERT(claim_hspi==true);
    } else {
        TEST_ASSERT(claim_hspi==true);
        TEST_ASSERT(claim_vspi==true);
    }

#ifdef CONFIG_ESP32_SPIRAM_SUPPORT
    test_psram_content();
#endif
}
