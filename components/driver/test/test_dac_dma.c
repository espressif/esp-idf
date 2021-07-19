/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 Tests for the dac device driver on ESP32-S2 only
*/
#include "sdkconfig.h"
#if CONFIG_IDF_TARGET_ESP32S2

#include "esp_system.h"
#include "esp_intr_alloc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "unity.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "test_utils.h"
#include "soc/soc.h"
#include "soc/spi_reg.h"
#include "soc/adc_periph.h"
#include "soc/dac_periph.h"
#include "soc/spi_periph.h"
#include "test/test_common_adc.h"
#include "driver/dac.h"
#include "soc/system_reg.h"
#include "esp32s2/rom/lldesc.h"
#include "test/test_adc_dac_dma.h"
#include "soc/apb_ctrl_reg.h"
#include "math.h"

static const char *TAG = "test_adc";

#define PLATFORM_SELECT            (1)  //0: pxp; 1: chip
#if (PLATFORM_SELECT == 0)              //PXP platform
#include "soc/syscon_reg.h"
#define SET_BREAK_POINT(flag) REG_WRITE(SYSCON_DATE_REG, flag)
//PXP clk is slower.
#define SYS_DELAY_TIME_MOM    (1/40)
#define RTC_SLOW_CLK_FLAG     1     // Slow clock is 32KHz.
static void test_pxp_deinit_io(void)
{
    for (int i = 0; i < 22; i++) {
        rtc_gpio_init(i);
    }
}
#else
//PXP clk is slower.
#define SET_BREAK_POINT(flag)
#define SYS_DELAY_TIME_MOM    (1)
#define RTC_SLOW_CLK_FLAG     0     // Slow clock is 32KHz.
#endif

#define SAR_SIMPLE_NUM  512  // Set out number of enabled unit.

static const char *TAG = "test_adc";

/*******************************************/
/**           DAC-DMA INIT CODE            */
/*******************************************/
#define PI 3.14159265
static uint8_t _buf[512];
void dac_dma_test_create_buffer(dac_digi_convert_mode_t mode)
{
    if (mode == DAC_CONV_ALTER) {
        for(int i=0; i < 256; i++) {
            if (i % 2 != 0) {
                _buf[i] = i % 256;
            } else {
                _buf[i] = 255*(sin(i * PI / 255) + 1)/2;
            }

        }
        for (int i=256;i < 512; i++) {
            if (i % 2 != 0) {
                _buf[i] = 255 - i % 256;
            } else {
                _buf[i] = 255*(sin((i-256) * PI / 255 - PI)+1)/2;
            }

        }
    } else {
        for(int i=0; i < 256; i++) {
            _buf[i] = i % 256;
        }
        for (int i=256;i < 512; i++) {
            _buf[i] = 255 - i % 256;
        }
    }
}

/**
 * Testcase: Check the interrupt types of DAC-DMA.
 */
void test_dac_dma(dac_digi_convert_mode_t mode)
{
    ESP_LOGI(TAG, "  >> %s - dac mode %d<<  ", __func__, mode);

    const dac_digi_init_config_t cfg = {
        .mode = mode,
        .dac_chan_msk = BIT(DAC_CHANNEL_1) | BIT(DAC_CHANNEL_2),
        .interval = 100,
        .dac_dma_cnt = 4,
        .dac_dma_length = 128,
        .dac_dma_link_type = DAC_DMA_LINK_RECURSIVE,
    };
    dac_digi_initialize(&cfg);

    dac_dma_test_create_buffer(mode);
    dac_digi_start();
    uint32_t length = sizeof(_buf);
    dac_digi_write_bytes(length, (uint8_t*)_buf, portMAX_DELAY);
    // /* Check interrupt type */

    ESP_LOGI(TAG, "DAC-DMA intr test over");
    dac_digi_stop();
    dac_digi_deinitialize();
}

TEST_CASE("DAC with DMA test", "[dac]")
{
    test_dac_dma(DAC_CONV_NORMAL);
    test_dac_dma(DAC_CONV_ALTER);
}

/** ADC-DMA ISR handler. */
static IRAM_ATTR void dac_dma_isr(void * arg)
{
    uint32_t int_st = REG_READ(SPI_DMA_INT_ST_REG(3));
    int task_awoken = pdFALSE;
    dac_dma_event_t adc_evt;
    adc_evt.int_msk = int_st;
    REG_WRITE(SPI_DMA_INT_CLR_REG(3), int_st);
    xQueueSendFromISR(que_dac, &adc_evt, &task_awoken);
    ESP_EARLY_LOGV(TAG, "int msk%x, raw%x", int_st, REG_READ(SPI_DMA_INT_RAW_REG(3)));

    if (task_awoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

/**
 * Testcase: Check the interrupt types of DAC-DMA.
 */
void test_dac_dig_dma_intr_check_legacy(dac_digi_convert_mode_t mode)
{
    ESP_LOGI(TAG, "  >> %s - dac mode %d<<  ", __func__, mode);

    dac_dma_event_t evt;

    dac_digi_init();
    const dac_digi_config_t cfg = {
        .mode = mode,
        .interval = 100,
        .dig_clk.use_apll = false,  // APB clk
        .dig_clk.div_num = 79,
        .dig_clk.div_b = 1,
        .dig_clk.div_a = 0,
    };
    dac_digi_controller_config(&cfg);
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_enable(DAC_CHANNEL_2);

    /* DAC-DMA linker init */
    if (que_dac == NULL) {
        que_dac = xQueueCreate(5, sizeof(dac_dma_event_t));
    } else {
        xQueueReset(que_dac);
    }
    uint32_t int_mask = SPI_OUT_DONE_INT_ENA | SPI_OUT_EOF_INT_ENA | SPI_OUT_TOTAL_EOF_INT_ENA;
    uint32_t dma_addr = dac_dma_linker_init(mode, false);
    adc_dac_dma_isr_register(dac_dma_isr, NULL, int_mask);
    adc_dac_dma_linker_start(DMA_ONLY_DAC_OUTLINK, (void *)dma_addr, int_mask);

    /* ADC-DMA start output */
    dac_digi_start();

    /* Check interrupt type */
    while (int_mask) {
        TEST_ASSERT_EQUAL( xQueueReceive(que_dac, &evt, 2000 / portTICK_PERIOD_MS), pdTRUE );
        ESP_LOGI(TAG, "DAC-DMA intr type 0x%x", evt.int_msk);
        if (evt.int_msk & int_mask) {
            int_mask &= (~evt.int_msk);
        }
    }

    ESP_LOGI(TAG, "DAC-DMA intr test over");
    adc_dac_dma_linker_deinit();
    adc_dac_dma_isr_deregister(dac_dma_isr, NULL);
    TEST_ESP_OK( dac_digi_deinit() );
}

TEST_CASE("DAC-DMA interrupt test(legacy api)", "[dac]")
{
    test_dac_dig_dma_intr_check_legacy(DAC_CONV_NORMAL);
    test_dac_dig_dma_intr_check_legacy(DAC_CONV_ALTER);
}

#endif // DAC_DMA_LEGACY_IMPL

#endif // CONFIG_IDF_TARGET_ESP32S2
