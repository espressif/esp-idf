/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "unity_test_utils_cache.h"
#include "driver/dac_oneshot.h"
#include "driver/dac_continuous.h"
#include "esp_heap_caps.h"
#include "esp_err.h"
#include "esp_log.h"

#define BUF_SIZE    2000

typedef struct {
    int     cnt;
    bool    result;
} test_dac_intr_data_t;

static void IRAM_ATTR test_dac_direct_set_safety(void *usr_ctx)
{
    dac_oneshot_handle_t handle = (dac_oneshot_handle_t)usr_ctx;
    dac_oneshot_output_voltage(handle, 128);
}

static void IRAM_ATTR test_dac_dma_iram_safety(void *usr_ctx)
{
    uint8_t *data = (uint8_t *)usr_ctx;
    for (int i = 0; i < BUF_SIZE; i++) {
        data[i] = i % 128 + 1;
    }
}

static void IRAM_ATTR test_dac_dma_intr_iram_safety(void *usr_ctx)
{
    test_dac_intr_data_t *data = (test_dac_intr_data_t *)usr_ctx;
    data->cnt = 0;
    esp_rom_delay_us(100 * 1000);
    data->result = data->cnt > 0;
}

static bool IRAM_ATTR test_dac_on_convert_done_cb(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data)
{
    test_dac_intr_data_t *data = (test_dac_intr_data_t *)user_data;
    data->cnt++;
    return false;
}

TEST_CASE("DAC_IRAM_safe_test", "[dac]")
{
    dac_oneshot_handle_t oneshot_handle;
    TEST_ESP_OK(dac_oneshot_new_channel(&(dac_oneshot_config_t) {
        .chan_id = DAC_CHAN_0
    }, &oneshot_handle));

    /* Test direct voltage setting safety */
    unity_utils_run_cache_disable_stub(test_dac_direct_set_safety, oneshot_handle);

    TEST_ESP_OK(dac_oneshot_del_channel(oneshot_handle));

    dac_continuous_handle_t cont_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 8,
        .buf_size = 2048,
        .freq_hz = 40000,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,     // If the frequency is out of range, try 'DAC_DIGI_CLK_SRC_APLL'
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* Allocate continuous channel */
    TEST_ESP_OK(dac_continuous_new_channels(&cont_cfg, &cont_handle));
    /* Register a callback to check if the interrupt can still triggered when cache is disabled */
    dac_event_callbacks_t cbs = {
        .on_convert_done = test_dac_on_convert_done_cb,
        .on_stop = NULL,
    };
    test_dac_intr_data_t intr_data = {
        .cnt = 0,
        .result = false,
    };
    TEST_ESP_OK(dac_continuous_register_event_callback(cont_handle, &cbs, &intr_data));
    /* Enable the channels in the group */
    TEST_ESP_OK(dac_continuous_enable(cont_handle));

    /* Real data in internal memory */
    uint8_t *data = (uint8_t *)heap_caps_calloc(1, BUF_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT(data);
    for (int i = 0; i < BUF_SIZE; i++) {
        data[i] = i % 256;
    }
    dac_continuous_write_cyclically(cont_handle, data, BUF_SIZE, NULL);
    unity_utils_run_cache_disable_stub(test_dac_dma_iram_safety, data);
    unity_utils_run_cache_disable_stub(test_dac_dma_intr_iram_safety, &intr_data);
    TEST_ASSERT(intr_data.result);

    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
    free(data);
}
