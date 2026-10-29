/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
#include "dac_test_utils.h"

#define SAMPLE_COUNT    2000

typedef struct {
    int     cnt;
    bool    result;
} test_dac_intr_data_t;

static void IRAM_ATTR test_dac_direct_set_safety(void *usr_ctx)
{
    dac_oneshot_handle_t handle = usr_ctx;
    dac_oneshot_output_code(handle, 128);
}

static void IRAM_ATTR test_dac_dma_intr_iram_safety(void *usr_ctx)
{
    test_dac_intr_data_t *data = usr_ctx;
    data->cnt = 0;
    esp_rom_delay_us(100 * 1000);
    data->result = data->cnt > 0;
}

static bool IRAM_ATTR test_dac_on_convert_done_cb(dac_continuous_handle_t handle, const dac_event_data_t *event, void *user_data)
{
    (void)handle;
    (void)event;
    test_dac_intr_data_t *data = user_data;
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
        .buf_size = 512,
        .freq_hz = 40000,
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
    dac_test_sample_t *data = heap_caps_calloc(SAMPLE_COUNT, sizeof(dac_test_sample_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT(data);
    const int16_t full_scale = 1 << dac_continuous_get_bitwidth(cont_handle);
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        data[i] = i % full_scale;
    }
    TEST_ESP_OK(dac_continuous_write_cyclically(cont_handle, data, SAMPLE_COUNT, NULL));
    unity_utils_run_cache_disable_stub(test_dac_dma_intr_iram_safety, &intr_data);
    TEST_ASSERT(intr_data.result);

    TEST_ESP_OK(dac_continuous_disable(cont_handle));
    TEST_ESP_OK(dac_continuous_del_channels(cont_handle));
    free(data);
}
