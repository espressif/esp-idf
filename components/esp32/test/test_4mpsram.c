#include "esp_heap_caps.h"
#include "unity.h"
#include "esp_log.h"
#include "driver/spi_common_internal.h"
#include "sdkconfig.h"

static const char TAG[] = "test_psram";

#ifdef CONFIG_SPIRAM
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

bool psram_is_32mbit_ver0(void);

static void test_spi_bus_occupy(spi_host_device_t expected_occupied_host)
{
    bool claim_hspi = spicommon_periph_claim(HSPI_HOST, "ut-hspi");
    if (claim_hspi) ESP_LOGI(TAG, "HSPI claimed.");

    bool claim_vspi = spicommon_periph_claim(VSPI_HOST, "ut-vspi");
    if (claim_vspi) ESP_LOGI(TAG, "VSPI claimed.");

    if (expected_occupied_host == HSPI_HOST) {
        TEST_ASSERT_FALSE(claim_hspi);
        TEST_ASSERT(claim_vspi);
    } else if (expected_occupied_host == VSPI_HOST) {
        TEST_ASSERT_FALSE(claim_vspi);
        TEST_ASSERT(claim_hspi);
    } else {
        TEST_ASSERT(claim_hspi);
        TEST_ASSERT(claim_vspi);
    }

#ifdef CONFIG_SPIRAM
    test_psram_content();
#endif
}

#if CONFIG_SPIRAM_OCCUPY_HSPI_HOST || CONFIG_SPIRAM_OCCUPY_VSPI_HOST
TEST_CASE("some spi bus occpied by psram", "[psram_4m][test_env=UT_T1_PSRAMV0]")
{
// NOTE: this unit test rely on the config that PSRAM of 8MB is used only when CONFIG_SPIRAM_BNKSWITCH_ENABLE is set
//currently all 8M psram don't need more SPI peripherals
#if !CONFIG_SPIRAM || !CONFIG_SPIRAM_SPEED_80M || CONFIG_SPIRAM_BANKSWITCH_ENABLE
#error unexpected test config, only psram 32MBit ver 0 at 80MHz will trigger the workaround
#endif

    spi_host_device_t host;
    if (!psram_is_32mbit_ver0()) {
        TEST_FAIL_MESSAGE("unexpected psram version");
    }

#if CONFIG_SPIRAM_OCCUPY_HSPI_HOST
    host = HSPI_HOST;
#elif CONFIG_SPIRAM_OCCUPY_VSPI_HOST
    host = VSPI_HOST;
#endif
    test_spi_bus_occupy(host);
}
#else
TEST_CASE("can use spi when not being used by psram", "[psram_4m]")
{
#if CONFIG_SPIRAM && CONFIG_SPIRAM_SPEED_80M
#error unexpected test config, some runners using the UT_T1_PSRAMV0 but still perform this test.\
they will not pass this test at 80MHz.
#endif
    test_spi_bus_occupy(-1);
}
#endif
