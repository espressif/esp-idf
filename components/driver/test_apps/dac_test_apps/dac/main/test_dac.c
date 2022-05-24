/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/dac_driver.h"
#include "driver/adc.h"
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
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_8   // GPIO25, same as DAC channel 0
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_12
#elif CONFIG_IDF_TARGET_ESP32S2
#define ADC_TEST_CHANNEL_NUM   ADC2_CHANNEL_6   // GPIO17, same as DAC channel 0
#define ADC_TEST_WIDTH         ADC_WIDTH_BIT_13
#endif

#define ADC_TEST_ATTEN         ADC_ATTEN_DB_11

TEST_CASE("DAC_API_basic_logic_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_conti_config_t dma_cfg = {
        .freq_hz = 20000,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    dac_cosine_config_t cos_cfg = {
        .freq_hz = 1000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .scale = DAC_COSINE_NO_ATTEN,
    };
    /* Constant API test */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_set_voltage(handle, 100));
    TEST_ESP_OK(dac_channels_disable(handle));

    /* DMA API test */
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ASSERT(dac_channels_enable_continuous_mode(handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
    TEST_ASSERT(dac_channels_disable(handle) == ESP_ERR_INVALID_STATE);
    TEST_ASSERT(dac_channels_deinit_continuous_mode(handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));

    /* Cosine wave API test */
    TEST_ESP_OK(dac_channels_init_cosine_mode(handle, &cos_cfg));
    TEST_ASSERT(dac_del_channels(handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_channels_start_cosine_output(handle));
    TEST_ASSERT(dac_channels_disable(handle) == ESP_ERR_INVALID_STATE);
    TEST_ESP_OK(dac_channels_stop_cosine_output(handle));
    TEST_ESP_OK(dac_channels_deinit_cosine_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));

    /* DMA peripheral availability test */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
#if CONFIG_IDF_TARGET_ESP32
    TEST_ESP_OK(i2s_platform_acquire_occupation(0, "dac_test"));
#elif CONFIG_IDF_TARGET_ESP32S2
    TEST_ASSERT(spicommon_periph_claim(SPI3_HOST, "dac_test"));
#endif
    TEST_ASSERT(dac_channels_init_continuous_mode(handle, &dma_cfg) == ESP_ERR_NOT_FOUND);
#if CONFIG_IDF_TARGET_ESP32
    TEST_ESP_OK(i2s_platform_release_occupation(0));
#elif CONFIG_IDF_TARGET_ESP32S2
    TEST_ASSERT(spicommon_periph_free(SPI3_HOST));
#endif
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
}

TEST_CASE("DAC_memory_leak_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_conti_config_t dma_cfg = {
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .freq_hz = 20000,
    };
    /* Some resources will be lazy installed, ignore the first around */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));

    int initial_size = esp_get_free_heap_size();
    printf("Initial free heap size: %d\n", initial_size);
    for (int i = 0; i < 20; i++) {
        printf("# %d: ---------------------------------\n", i + 1);
        TEST_ESP_OK(dac_new_channels(&cfg, &handle));
        TEST_ESP_OK(dac_channels_enable(handle));
        TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
        TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
        TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
        TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
        TEST_ESP_OK(dac_channels_disable(handle));
        TEST_ESP_OK(dac_del_channels(handle));
        printf("current heap size: %d\n", esp_get_free_heap_size());
        TEST_ASSERT(initial_size == esp_get_free_heap_size());
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
    TEST_ASSERT(initial_size == esp_get_free_heap_size());
}

TEST_CASE("DAC_set_voltage_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));

    /* Prepare ADC2 */
    TEST_ESP_OK(adc2_config_channel_atten(ADC_TEST_CHANNEL_NUM, ADC_TEST_ATTEN));

    int curr_adc = 0;
    int last_adc = 0;
    for (uint8_t i = 0; i <= 200; i += 20) {
        TEST_ESP_OK(dac_channels_set_voltage(handle, i));
        vTaskDelay(pdMS_TO_TICKS(20));
        TEST_ESP_OK(adc2_get_raw(ADC_TEST_CHANNEL_NUM, ADC_TEST_WIDTH, &curr_adc));
        printf("DAC: %d - ADC: %d\n", i, curr_adc);
        if (last_adc != 0) {
            TEST_ASSERT_GREATER_THAN(last_adc, curr_adc);
        }
        last_adc = curr_adc;
    }
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
}

TEST_CASE("DAC_dma_write_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_conti_config_t dma_cfg = {
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .freq_hz = 20000,
    };
    uint8_t *data = (uint8_t *)calloc(1, 2000);
    TEST_ASSERT(data);
    for (int i = 0; i < 2000; i++) {
        data[i] = i % 256;
    }

    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));

    TEST_ESP_OK(dac_channels_write_continuously(handle, data, 2000, NULL, 1000));
    vTaskDelay(pdMS_TO_TICKS(200));
    TEST_ESP_OK(dac_channels_write_cyclically(handle, data, 2000, NULL, 1000));

    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
    free(data);
}

/* Test the conversion frequency by counting the pulse of WS signal
 * The frequency test is currently only supported on ESP32
 * because there is no such signal to monitor on ESP32-S2 */
#if CONFIG_IDF_TARGET_ESP32
TEST_CASE("DAC_dma_conver_frequency_test", "[dac]")
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

    uint8_t *data = (uint8_t *)calloc(1, 2000);
    TEST_ASSERT(data);

    /* Register DAC DMA using PLL */
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_conti_config_t dma_cfg = {
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .freq_hz = 20000,
    };

    /* Initialize DAC to test default PLL clock */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
    /* Start transmitting data on line */
    TEST_ESP_OK(dac_channels_write_cyclically(handle, data, 2000, NULL, 1000));

    int expt_pulse = 2000;
    int real_pulse;
    /* Count pulse by PCNT */
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
    /* Delete DAC handle */
    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));

    printf("[PLL | 20000 Hz] %d pulses, expected %d, err %d\n", real_pulse, expt_pulse, real_pulse - expt_pulse);
    TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);

    dma_cfg.clk_src = DAC_DIGI_CLK_SRC_APLL;
    /* Initialize DAC to test APLL clock */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
    /* Start transmitting data on line */
    TEST_ESP_OK(dac_channels_write_cyclically(handle, data, 2000, NULL, 1000));

    /* Count pulse by PCNT */
    TEST_ESP_OK(pcnt_unit_clear_count(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_start(pcnt_unit));
    vTaskDelay(pdMS_TO_TICKS(100));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_get_count(pcnt_unit, &real_pulse));
    /* Delete DAC handle */
    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));

    printf("[APLL | 20000 Hz] %d pulses, expected %d, err %d\n", real_pulse, expt_pulse, real_pulse - expt_pulse);
    TEST_ASSERT_INT_WITHIN(expt_pulse * 0.01, expt_pulse, real_pulse);
    free(data);
    /* Free PCNT */
    TEST_ESP_OK(pcnt_del_channel(pcnt_chan));
    TEST_ESP_OK(pcnt_unit_stop(pcnt_unit));
    TEST_ESP_OK(pcnt_unit_disable(pcnt_unit));
    TEST_ESP_OK(pcnt_del_unit(pcnt_unit));
}
#endif

TEST_CASE("DAC_cosine_wave_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_cosine_config_t cos_cfg = {
        .freq_hz = 1000,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .offset = 0,
        .phase = DAC_COSINE_PHASE_0,
        .scale = DAC_COSINE_NO_ATTEN,
    };
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_cosine_mode(handle, &cos_cfg));
    TEST_ESP_OK(dac_channels_start_cosine_output(handle));

    vTaskDelay(pdMS_TO_TICKS(200));

    TEST_ESP_OK(dac_channels_stop_cosine_output(handle));
    TEST_ESP_OK(dac_channels_deinit_cosine_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
}

static volatile bool task_run_flag;

static void dac_acyclicly_write_task(void *arg)
{
    dac_channels_handle_t dac_handle = (dac_channels_handle_t)arg;
    uint8_t buf[1000];
    for (int i = 0; i < 1000; i++) {
        buf[i] = i % 256;
    }
    while (task_run_flag) {
        if (dac_channels_write_continuously(dac_handle, buf, 100, NULL, 1000) == ESP_OK) {
            printf("DAC write data success\n");
        }
        vTaskDelay(20);
    }
    vTaskDelete(NULL);
}

TEST_CASE("DAC_DMA_thread_safe", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_CH0};
    dac_conti_config_t dma_cfg = {
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .freq_hz = 20000,
    };
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
    task_run_flag = true;
    xTaskCreate(dac_acyclicly_write_task, "dac_acyclicly_write_task", 4096, handle, 5, NULL);

    for (int i = 0; i < 5; i++) {
        TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
        printf("DAC stopped\n");
        vTaskDelay(pdMS_TO_TICKS(100));
        TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));
        printf("DAC started\n");
        vTaskDelay(pdMS_TO_TICKS(300));
    }
    task_run_flag = false;
    vTaskDelay(pdMS_TO_TICKS(100));

    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
}
