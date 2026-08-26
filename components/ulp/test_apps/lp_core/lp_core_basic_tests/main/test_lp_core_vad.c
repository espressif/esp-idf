/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <string.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/lp_i2s.h"
#include "driver/lp_i2s_std.h"
#include "driver/i2s_std.h"
#include "ulp_lp_core.h"
#include "ulp_lp_core_lp_vad_shared.h"
#include "lp_core_test_app_vad.h"

#if CONFIG_TEST_LP_CORE_VAD_ENABLE

#define TEST_I2S_FRAME_SIZE            (128)       // Frame numbers in every writing / reading
#define TEST_I2S_TRANS_SIZE            (4096)      // Trans size

extern const uint8_t test_vad_pcm_start[]            asm("_binary_test_vad_8k_pcm_start");
extern const uint8_t test_vad_pcm_end[]              asm("_binary_test_vad_8k_pcm_end");
extern const uint8_t lp_core_main_vad_bin_start[]    asm("_binary_lp_core_test_app_vad_bin_start");
extern const uint8_t lp_core_main_vad_bin_end[]      asm("_binary_lp_core_test_app_vad_bin_end");
static const char *TAG = "TEST_VAD";

static void load_and_start_lp_core_firmware(ulp_lp_core_cfg_t* cfg, const uint8_t* firmware_start, const uint8_t* firmware_end)
{
    TEST_ASSERT(ulp_lp_core_load_binary(firmware_start,
                                        (firmware_end - firmware_start)) == ESP_OK);

    TEST_ASSERT(ulp_lp_core_run(cfg) == ESP_OK);
}

void test_lp_vad(lp_vad_t vad_id)
{
    esp_err_t ret = ESP_FAIL;
    int pcm_size = test_vad_pcm_end - test_vad_pcm_start;
    printf("pcm_size: %d\n", pcm_size);

    lp_i2s_chan_handle_t rx_handle = NULL;
    lp_i2s_chan_config_t config = {
        .id = 0,
        .role = I2S_ROLE_SLAVE,
        .threshold = 512,
    };
    TEST_ESP_OK(lp_i2s_new_channel(&config, NULL, &rx_handle));

    i2s_chan_handle_t tx_handle = NULL;
    i2s_chan_config_t i2s_channel_config = {
        .id = I2S_NUM_0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4,
        .dma_frame_num = TEST_I2S_FRAME_SIZE,
        .auto_clear = false,
    };
    TEST_ESP_OK(i2s_new_channel(&i2s_channel_config, &tx_handle, NULL));

    lp_i2s_std_config_t lp_std_cfg = {
        .pin_cfg = {
            .bck = 4,
            .ws = 5,
            .din = 6,
        },
    };
    lp_std_cfg.slot_cfg = (lp_i2s_std_slot_config_t)LP_I2S_STD_PCM_SHORT_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    TEST_ESP_OK(lp_i2s_channel_init_std_mode(rx_handle, &lp_std_cfg));

    i2s_std_config_t i2s_std_config = {
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = GPIO_NUM_7,
            .ws   = GPIO_NUM_8,
            .dout = GPIO_NUM_21,
            .din  = -1,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    i2s_std_config.clk_cfg = (i2s_std_clk_config_t)I2S_STD_CLK_DEFAULT_CONFIG(16000);
    i2s_std_config.slot_cfg = (i2s_std_slot_config_t)I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &i2s_std_config));

    // LP VAD Init
    lp_vad_init_config_t init_config = {
        .lp_i2s_chan = rx_handle,
        .vad_config = {
            .init_frame_num = 100,
            .min_energy_thresh = 100,
            .speak_activity_thresh = 10,
            .non_speak_activity_thresh = 30,
            .min_speak_activity_thresh = 3,
            .max_speak_activity_thresh = 100,
        },
    };
    TEST_ESP_OK(lp_core_lp_vad_init(0, &init_config));

    /* Load ULP firmware and start the coprocessor */
    ulp_lp_core_cfg_t cfg = {
        .wakeup_source = ULP_LP_CORE_WAKEUP_SOURCE_LP_VAD,
    };
    load_and_start_lp_core_firmware(&cfg, lp_core_main_vad_bin_start, lp_core_main_vad_bin_end);

    uint8_t *txbuf = (uint8_t *)heap_caps_calloc(1, TEST_I2S_TRANS_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT(txbuf);

    uint8_t *prebuf = (uint8_t *)heap_caps_calloc(1, TEST_I2S_TRANS_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT(prebuf);

    memcpy(prebuf, test_vad_pcm_start, TEST_I2S_TRANS_SIZE);
    memcpy(txbuf, test_vad_pcm_start, TEST_I2S_TRANS_SIZE);

    for (int i = 0; i < TEST_I2S_TRANS_SIZE; i++) {
        ESP_LOGD(TAG, "prebuf[%d]: %d", i, prebuf[i]);
        ESP_LOGD(TAG, "txbuf[%d]: %d", i, txbuf[i]);
    }

    size_t bytes_written = 0;
    TEST_ESP_OK(i2s_channel_preload_data(tx_handle, prebuf, TEST_I2S_TRANS_SIZE, &bytes_written));
    TEST_ESP_OK(lp_i2s_channel_enable(rx_handle));
    TEST_ESP_OK(lp_core_lp_vad_enable(0));
    TEST_ESP_OK(i2s_channel_enable(tx_handle));

    ret = i2s_channel_write(tx_handle, txbuf, TEST_I2S_TRANS_SIZE, &bytes_written, 0);
    if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
        TEST_ESP_OK(ret);
    }
    ESP_LOGD(TAG, "bytes_written: %d", bytes_written);

    while (!ulp_vad_wakup) {
        ;
    }

    ESP_LOGI(TAG, "wakeup");

    TEST_ESP_OK(lp_i2s_channel_disable(rx_handle));
    TEST_ESP_OK(lp_i2s_del_channel(rx_handle));
    TEST_ESP_OK(i2s_channel_disable(tx_handle));
    TEST_ESP_OK(i2s_del_channel(tx_handle));
    free(txbuf);
    free(prebuf);
}

TEST_CASE("LP VAD wakeup test", "[lp_core][lp_vad]")
{
    test_lp_vad(0);
}
#endif  //CONFIG_TEST_LP_CORE_VAD_ENABLE
