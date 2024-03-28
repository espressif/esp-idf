/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <inttypes.h>
#include <stdio.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/dac_oneshot.h"
#include "driver/dac_cosine.h"
#include "driver/dac_continuous.h"
#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#if CONFIG_IDF_TARGET_ESP32
#include "esp_private/i2s_platform.h"
// Following headers are used to test the conversion frequency
#include "soc/i2s_periph.h"
#include "hal/gpio_hal.h"
#include "driver/pulse_cnt.h"
#include "soc/pcnt_periph.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp_private/spi_common_internal.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
#define ADC_TEST_CHANNEL_NUM   ADC_CHANNEL_8   // GPIO25, same as DAC channel 0
#define ADC_TEST_WIDTH         ADC_BITWIDTH_12
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_CHANNEL_NUM   ADC_CHANNEL_6   // GPIO17, same as DAC channel 0
#define ADC_TEST_WIDTH         ADC_BITWIDTH_13
#endif

#define ADC_TEST_ATTEN         ADC_ATTEN_DB_12

TEST_CASE("DAC_API_basic_logic_test", "[dac]")
{
    /* Constant API test */
    dac_oneshot_handle_t oneshot_chan0_handle;
    TEST_ESP_OK(dac_oneshot_new_channel(&(dac_oneshot_config_t) {
        .chan_id = DAC_CHAN_0
    }, &oneshot_chan0_handle));
    TEST_ESP_OK(dac_oneshot_output_voltage(oneshot_chan0_handle, 128));
    TEST_ESP_OK(dac_oneshot_del_channel(oneshot_chan0_handle));
    dac_oneshot_handle_t oneshot_chan1_handle;
    TEST_ESP_OK(dac_oneshot_new_channel(&(dac_oneshot_config_t) {
        .chan_id = DAC_CHAN_1
    }, &oneshot_chan1_handle));
    TEST_ESP_OK(dac_oneshot_output_voltage(oneshot_chan1_handle, 100));
    TEST_ESP_OK(dac_oneshot_del_channel(oneshot_chan1_handle));

    /* Cosine wave API test */
    dac_cosine_handle_t cos_chan0_handle;
    dac_cosine_handle_t cos_chan1_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_0,
        .freq_hz = 1000, // It will be covered by 8000 in the latter configuration
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .flags.force_set_freq = false,
    };
    dac_cosine_config_t cos1_cfg = {
        .chan_id = DAC_CHAN_1,
        .freq_hz = 8000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_180,
        .atten = DAC_COSINE_ATTEN_DB_6,
        .flags.force_set_freq = false,
    };
    TEST_ESP_OK(dac_cosine_new_channel(&cos0_cfg, &cos_chan0_handle));
    /* Try to update the frequency without force set */
    TEST_ASSERT(dac_cosine_new_channel(&cos1_cfg, &cos_chan1_handle) == ESP_ERR_INVALID_STATE);
    /* Force update the frequnecy */
    cos1_cfg.flags.force_set_freq = true;
    TEST_ESP_OK(dac_cosine_new_channel(&cos1_cfg, &cos_chan1_handle));
    TEST_ASSERT(dac_cosine_stop(cos_chan0_handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_cosine_start(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_start(cos_chan1_handle));
    TEST_ASSERT(dac_cosine_del_channel(cos_chan0_handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_cosine_stop(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_stop(cos_chan1_handle));
    TEST_ESP_OK(dac_cosine_del_channel(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_del_channel(cos_chan1_handle));

    /* DMA API test */
    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = 48000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* DMA peripheral availability test */
#if CONFIG_IDF_TARGET_ESP32
    TEST_ESP_OK(i2s_platform_acquire_occupation(0, "dac_test"));
#elif CONFIG_IDF_TARGET_ESP32S2
    TEST_ASSERT(spicommon_periph_claim(SPI3_HOST, "dac_test"));
#endif
    TEST_ASSERT(dac_continuous_new_channels(&cont_cfg, &cont_handle) == ESP_ERR_NOT_FOUND);
#if CONFIG_IDF_TARGET_ESP32
    TEST_ESP_OK(i2s_platform_release_occupation(0));
#elif CONFIG_IDF_TARGET_ESP32S2
    TEST_ASSERT(spicommon_periph_free(SPI3_HOST));
#endif

    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ASSERT(dac_continuous_disable(cont_handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_continuous_enable(cont_handle));
    TEST_ASSERT(dac_continuous_del_channels(cont_handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
}

TEST_CASE("DAC_memory_leak_test", "[dac]")
{
    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = 48000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    size_t len = 1024;
    uint8_t buf[len];
    for (int i = 0; i < len; i++) {
        buf[i] = i % 256;
    }

    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ESP_OK(dac_continuous_enable(cont_handle));
    TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, buf, len, NULL));
    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));

    uint32_t initial_size = esp_get_free_heap_size();
    printf("Initial free heap size: %"PRIu32"\n", initial_size);
    for (int i = 0; i < 20; i++) {
        printf("# %d: ---------------------------------\n", i + 1);
        TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
        TEST_ESP_OK(dac_continuous_enable(cont_handle));
        TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, buf, len, NULL));
        TEST_ESP_OK(dac_continuous_disable(cont_handle));
        TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
        printf("current heap size: %"PRIu32"\n", esp_get_free_heap_size());
        TEST_ASSERT(initial_size == esp_get_free_heap_size());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());
}

TEST_CASE("DAC_set_voltage_test", "[dac]")
{
    dac_oneshot_handle_t oneshot_chan0_handle;
    dac_oneshot_config_t onshot_cfg = {
        .chan_id = DAC_CHAN_0,
    };
    TEST_ESP_OK(dac_oneshot_new_channel(&onshot_cfg, &oneshot_chan0_handle));

    /* Prepare ADC2 */
    adc_oneshot_unit_handle_t adc2_handle;
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = false,
    };
    TEST_ESP_OK(adc_oneshot_new_unit(&unit_cfg, &adc2_handle));
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_TEST_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    TEST_ESP_OK(adc_oneshot_config_channel(adc2_handle, ADC_TEST_CHANNEL_NUM, &chan_cfg));

    int curr_adc = 0;
    int last_adc = 0;
    for (uint8_t i = 0; i <= 200; i += 20) {
        TEST_ESP_OK(dac_oneshot_output_voltage(oneshot_chan0_handle, i));
        vTaskDelay(pdMS_TO_TICKS(20));
        TEST_ESP_OK(adc_oneshot_read(adc2_handle, ADC_TEST_CHANNEL_NUM, &curr_adc));
        printf("DAC: %d - ADC: %d\n", i, curr_adc);
        if (last_adc != 0) {
            TEST_ASSERT_GREATER_THAN(last_adc, curr_adc);
        }
        last_adc = curr_adc;
    }
    TEST_ESP_OK(dac_oneshot_del_channel(oneshot_chan0_handle));
    TEST_ESP_OK(adc_oneshot_del_unit(adc2_handle));
}

TEST_CASE("DAC_dma_write_test", "[dac]")
{
    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 1024,
        .freq_hz = 48000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    size_t len = 520; // To test if the driver can work correctly with uncommon length
    uint8_t buf[len];
    for (int i = 0; i < len; i++) {
        buf[i] = i % 104;
    }

    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ESP_OK(dac_continuous_enable(cont_handle));
    for (int i = 0; i < 4; i++) {
        TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, buf, len, NULL));
        vTaskDelay(pdMS_TO_TICKS(200));
        for (int j = 0; j < 10; j++) {
            TEST_ESP_OK(dac_continuous_write(cont_handle, buf, len, NULL, 1000));
        }
    }
    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
}

/* Test the conversion frequency by counting the pulse of WS signal
 * The frequency test is currently only supported on ESP32
 * because there is no such signal to monitor on ESP32-S2 */
#if CONFIG_IDF_TARGET_ESP32
TEST_CASE("DAC_dma_convert_frequency_test", "[dac]")
{
    /* Prepare configuration for the PCNT unit */
    pcnt_unit_handle_t pcnt_unit = NULL;
    pcnt_channel_handle_t pcnt_chan = NULL;

    pcnt_unit_config_t unit_config = {
        .high_limit = (int16_t)0x7fff,
        .low_limit = (int16_t)0x8000,
    };
    pcnt_chan_config_t chan_config = {
        .edge_gpio_num = GPIO_NUM_4,
        .level_gpio_num = -1,
    };
    TEST_ESP_OK(pcnt_new_unit(&unit_config, &pcnt_unit));
    TEST_ESP_OK(pcnt_unit_set_glitch_filter(pcnt_unit, NULL));
    TEST_ESP_OK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));
    TEST_ESP_OK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    TEST_ESP_OK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));
    TEST_ESP_OK(pcnt_unit_enable(pcnt_unit));

    // Connect the clock signal to pcnt input signal
    gpio_hal_iomux_func_sel(GPIO_PIN_MUX_REG[GPIO_NUM_4], PIN_FUNC_GPIO);
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT_OUTPUT);
    // The DAC conversion frequency is equal to I2S bclk.
    esp_rom_gpio_connect_out_signal(GPIO_NUM_4, i2s_periph_signal[0].m_tx_ws_sig, 0, 0);
    esp_rom_gpio_connect_in_signal(GPIO_NUM_4, pcnt_periph_signals.groups[0].units[0].channels[0].pulse_sig, 0);

    size_t len = 800;
    uint8_t data[len];
    for (int i = 0; i < len; i++) {
        data[i] = i % 256;
    }

    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = 20000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ESP_OK(dac_continuous_enable(cont_handle));
    TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, data, len, NULL));

    int expt_pulse = 2000;
    int real_pulse;
    /* Count pulse by PCNT */
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
    /* Delete DAC continuous handle */
    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));

    printf("[PLL | 20000 Hz] %d pulses, expected %d, err %d\n", real_pulse, expt_pulse, real_pulse - expt_pulse);
    TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);

    cont_cfg.clk_src = DAC_DIGI_CLK_SRC_APLL;
    /* Initialize DAC to test APLL clock */
    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ESP_OK(dac_continuous_enable(cont_handle));
    /* Start transmitting data on line */
    TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, data, len, NULL));

    /* Count pulse by PCNT */
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
    /* Delete DAC handle */
    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));

    printf("[APLL | 20000 Hz] %d pulses, expected %d, err %d\n", real_pulse, expt_pulse, real_pulse - expt_pulse);
    TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);
    /* Free PCNT */
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}
#endif

TEST_CASE("DAC_cosine_wave_test", "[dac]")
{
    dac_cosine_handle_t cos_chan0_handle;
    dac_cosine_handle_t cos_chan1_handle;
    dac_cosine_config_t cos0_cfg = {
        .chan_id = DAC_CHAN_0,
        .freq_hz = 1000, // It will be covered by 8000 in the latter configuration
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .flags.force_set_freq = false,
    };
    dac_cosine_config_t cos1_cfg = {
        .chan_id = DAC_CHAN_1,
        .freq_hz = 1000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_180,
        .atten = DAC_COSINE_ATTEN_DB_6,
        .flags.force_set_freq = false,
    };
    TEST_ESP_OK(dac_cosine_new_channel(&cos0_cfg, &cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_new_channel(&cos1_cfg, &cos_chan1_handle));
    TEST_ESP_OK(dac_cosine_start(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_start(cos_chan1_handle));

    // TODO: find some more meaningful way to test cosine wave
    vTaskDelay(pdMS_TO_TICKS(500));

    TEST_ESP_OK(dac_cosine_stop(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_stop(cos_chan1_handle));
    TEST_ESP_OK(dac_cosine_del_channel(cos_chan0_handle));
    TEST_ESP_OK(dac_cosine_del_channel(cos_chan1_handle));
}

static void dac_cyclically_write_task(void *arg)
{
    dac_continuous_handle_t dac_handle = (dac_continuous_handle_t)arg;
    size_t len = 1000;
    uint8_t buf[len];
    uint8_t max_val = 50;
    while (1) {
        max_val += 50;
        for (int i = 0; i < len; i++) {
            buf[i] = i % max_val;
        }
        printf("Write cyclically\n");
        TEST_ESP_OK(dac_continuous_write_cyclically(dac_handle, buf, len, NULL));
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    vTaskDelete(NULL);
}

static void dac_continuously_write_task(void *arg)
{
    dac_continuous_handle_t dac_handle = (dac_continuous_handle_t)arg;
    size_t len = 2048;
    uint8_t buf[len];
    for (int i = 0; i < len; i++) {
        buf[i] = i % 256;
    }
    while (1) {
        printf("Write continuously\n");
        TEST_ESP_OK(dac_continuous_write(dac_handle, buf, len, NULL, 100));
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    vTaskDelete(NULL);
}

TEST_CASE("DAC_continuous_mode_concurrency_test", "[dac]")
{
    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 1024,
        .freq_hz = 48000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };

    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    TEST_ESP_OK(dac_continuous_enable(cont_handle));

    TaskHandle_t cyc_task;
    TaskHandle_t con_task;
    xTaskCreate(dac_cyclically_write_task, "dac_cyclically_write_task", 4096, cont_handle, 5, &cyc_task);
    xTaskCreate(dac_continuously_write_task, "dac_continuously_write_task", 4096, cont_handle, 5, &con_task);

    vTaskDelay(pdMS_TO_TICKS(5000));

    vTaskDelete(cyc_task);
    vTaskDelete(con_task);

    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
}
