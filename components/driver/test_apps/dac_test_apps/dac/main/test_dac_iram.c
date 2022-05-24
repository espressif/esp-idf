/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/dac_driver.h"
#include "esp_private/spi_flash_os.h"
#include "esp_heap_caps.h"
#include "esp_err.h"

#define BUF_SIZE    2000

static void IRAM_ATTR test_dac_direct_set_safety(dac_channels_handle_t handle)
{
    spi_flash_guard_get()->start();
    dac_channels_set_voltage(handle, 128);
    spi_flash_guard_get()->end();
}

static void IRAM_ATTR test_dac_dma_iram_safety(dac_channels_handle_t handle, uint8_t *data, uint32_t len)
{
    spi_flash_guard_get()->start();
    // Change the data of DMA
    for (int i = 0; i < len; i++) {
        data[i] = i % 100;
    }
    spi_flash_guard_get()->end();
}

TEST_CASE("DAC_IRAM_safe_test", "[dac]")
{
    dac_channels_handle_t handle;
    dac_channels_config_t cfg = {.chan_sel = DAC_CHANNEL_MASK_BOTH};
    dac_conti_config_t dma_cfg = {
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .desc_num = 10,
        .freq_hz = 40000,
    };

    /* Real data in internal memory */
    uint8_t *data = (uint8_t *)heap_caps_calloc(1, BUF_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT(data);
    for (int i = 0; i < BUF_SIZE; i++) {
        data[i] = i % 256;
    }
    /* Get ready for dma transmition */
    TEST_ESP_OK(dac_new_channels(&cfg, &handle));
    TEST_ESP_OK(dac_channels_enable(handle));
    /* Test direct voltage setting safety */
    test_dac_direct_set_safety(handle);

    TEST_ESP_OK(dac_channels_init_continuous_mode(handle, &dma_cfg));
    TEST_ESP_OK(dac_channels_enable_continuous_mode(handle));

    /* Simulate cache off */
    dac_channels_write_cyclically(handle, data, BUF_SIZE, NULL, 1000);
    test_dac_dma_iram_safety(handle, data, BUF_SIZE);

    /* Deregister DAC DMA channel group */
    TEST_ESP_OK(dac_channels_disable_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_deinit_continuous_mode(handle));
    TEST_ESP_OK(dac_channels_disable(handle));
    TEST_ESP_OK(dac_del_channels(handle));
    free(data);
}
