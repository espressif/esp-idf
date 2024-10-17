/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "unity.h"
#include "driver/lp_i2s.h"
#include "driver/i2s_types.h"
#include "driver/lp_i2s_std.h"
#include "driver/i2s_std.h"
#include "soc/soc_caps.h"

#define TEST_I2S_FRAME_SIZE            (128)       // Frame numbers in every writing / reading
#define TEST_I2S_PRELOAD_SIZE          (4096)      // Preload size

typedef enum {
    TEST_I2S_MODE_STD_PHILIPS,
    TEST_I2S_MODE_STD_MSB,
    TEST_I2S_MODE_STD_PCM,
} test_i2s_mode_t;

static const char *TAG = "LP_I2S";

TEST_CASE("LP I2S exhausted allocation test", "[lp_i2s]")
{
    lp_i2s_chan_handle_t rx_handle[SOC_LP_I2S_NUM + 1] = {};
    lp_i2s_chan_config_t config = {
        .id = 0,
        .role = I2S_ROLE_SLAVE,
        .threshold = 512,
    };
    for (int i = 0; i < SOC_LP_I2S_NUM; i++) {
        printf("i: %d\n", i);
        TEST_ESP_OK(lp_i2s_new_channel(&config, NULL, &rx_handle[i]));
    }

    TEST_ASSERT(lp_i2s_new_channel(&config, NULL, &rx_handle[SOC_LP_I2S_NUM]) == ESP_ERR_NOT_FOUND);

    for (int i = 0; i < SOC_LP_I2S_NUM; i++) {
        TEST_ESP_OK(lp_i2s_del_channel(rx_handle[i]));
    }
}

static void s_test_i2s_init(test_i2s_mode_t mode, size_t buf_size, lp_i2s_chan_handle_t *rx_chan, i2s_chan_handle_t *tx_chan, uint8_t **tx_buf, uint8_t **pre_buf, uint8_t **rx_buf)
{
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
        .auto_clear = true
    };
    TEST_ESP_OK(i2s_new_channel(&i2s_channel_config, &tx_handle, NULL));

    lp_i2s_std_config_t lp_std_cfg = {
        .pin_cfg = {
            .bck = 4,
            .ws = 5,
            .din = 6,
        },
    };
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

    switch (mode) {
    case TEST_I2S_MODE_STD_PHILIPS:
        lp_std_cfg.slot_cfg = (lp_i2s_std_slot_config_t)LP_I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(lp_i2s_channel_init_std_mode(rx_handle, &lp_std_cfg));

        i2s_std_config.clk_cfg = (i2s_std_clk_config_t)I2S_STD_CLK_DEFAULT_CONFIG(16000);
        i2s_std_config.slot_cfg = (i2s_std_slot_config_t)I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &i2s_std_config));
        break;
    case TEST_I2S_MODE_STD_MSB:
        lp_std_cfg.slot_cfg = (lp_i2s_std_slot_config_t)LP_I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(lp_i2s_channel_init_std_mode(rx_handle, &lp_std_cfg));

        i2s_std_config.clk_cfg = (i2s_std_clk_config_t)I2S_STD_CLK_DEFAULT_CONFIG(16000);
        i2s_std_config.slot_cfg = (i2s_std_slot_config_t)I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &i2s_std_config));
        break;
    case TEST_I2S_MODE_STD_PCM:
        lp_std_cfg.slot_cfg = (lp_i2s_std_slot_config_t)LP_I2S_STD_PCM_SHORT_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(lp_i2s_channel_init_std_mode(rx_handle, &lp_std_cfg));

        i2s_std_config.clk_cfg = (i2s_std_clk_config_t)I2S_STD_CLK_DEFAULT_CONFIG(16000);
        i2s_std_config.slot_cfg = (i2s_std_slot_config_t)I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO);
        TEST_ESP_OK(i2s_channel_init_std_mode(tx_handle, &i2s_std_config));
        break;
    default:
        abort();
    }
    *rx_chan = rx_handle;
    *tx_chan = tx_handle;

    uint8_t *txbuf = (uint8_t *)heap_caps_calloc(1, buf_size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT(txbuf);
    for (int i = 0; i < buf_size; i++) {
        txbuf[i] = 0x55 + 0x10 * (i % 8);
    }

    uint8_t *prebuf = (uint8_t *)heap_caps_calloc(1, TEST_I2S_PRELOAD_SIZE, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT(prebuf);
    for (int i = 0; i < TEST_I2S_PRELOAD_SIZE; i++) {
        prebuf[i] = 0x55 + 0x10 * (i % 8);
        ESP_LOGD(TAG, "prebuf[%d]: 0x%x", i, prebuf[i]);
    }

    uint8_t *rxbuf = (uint8_t *)heap_caps_calloc(1, buf_size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    TEST_ASSERT(rxbuf);

    *tx_buf = txbuf;
    *pre_buf = prebuf;
    *rx_buf = rxbuf;
}

static void s_data_check(uint8_t *rx_buf, size_t buf_size)
{
    for (int i = 0; i < buf_size; i++) {
        ESP_DRAM_LOGD(TAG, "i: %d, 0x55 + ((i & 0x03) << 4): 0x%x, rx_buf: 0x%"PRIx8, i, 0x55 + ((i & 0x03) << 4), rx_buf[i]);
        if (0x55 + 0x10 * (i % 8) != rx_buf[i]) {
            printf("i: %d, rx_buf[i] addr: %p\n", i, &rx_buf[i]);
        }
        TEST_ASSERT_EQUAL(0x55 + 0x10 * (i % 8), rx_buf[i]);
    }
}

static void s_isr_test_flow(lp_i2s_chan_handle_t rx_chan, i2s_chan_handle_t tx_chan, size_t buf_size, uint8_t *tx_buf, uint8_t *pre_buf, uint8_t *rx_buf)
{
    esp_err_t ret = ESP_FAIL;
    size_t bytes_written = 0;
    TEST_ESP_OK(i2s_channel_preload_data(tx_chan, pre_buf, TEST_I2S_PRELOAD_SIZE, &bytes_written));
    TEST_ESP_OK(lp_i2s_channel_enable(rx_chan));
    ESP_LOGI(TAG, "reading...");
    TEST_ESP_OK(i2s_channel_enable(tx_chan));

    for (int i = 0; i < 100; i++) {
        ret = i2s_channel_write(tx_chan, tx_buf, buf_size, &bytes_written, 10);
        if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
            TEST_ESP_OK(ret);
        }
    }
}

static void s_test_i2s_deinit(lp_i2s_chan_handle_t rx_chan, i2s_chan_handle_t tx_chan, uint8_t *tx_buf, uint8_t *pre_buf, uint8_t *rx_buf)
{
    TEST_ESP_OK(lp_i2s_channel_disable(rx_chan));
    TEST_ESP_OK(lp_i2s_del_channel(rx_chan));
    TEST_ESP_OK(i2s_channel_disable(tx_chan));
    TEST_ESP_OK(i2s_del_channel(tx_chan));
    free(tx_buf);
    free(rx_buf);
    free(pre_buf);
}

TEST_CASE("test LP I2S read for STD", "[lp_i2s]")
{
    esp_err_t ret = ESP_FAIL;
    lp_i2s_chan_handle_t rx_chan = NULL;
    i2s_chan_handle_t tx_chan = NULL;

    size_t buf_size = 128;
    size_t bytes_written = 0;
    uint8_t *tx_buf = NULL;
    uint8_t *pre_buf = NULL;
    uint8_t *rx_buf = NULL;

    for (int i = TEST_I2S_MODE_STD_PHILIPS; i <= TEST_I2S_MODE_STD_PCM; i++) {
        s_test_i2s_init(i, buf_size, &rx_chan, &tx_chan, &tx_buf, &pre_buf, &rx_buf);

        lp_i2s_trans_t trans = {
            .buffer = rx_buf,
            .buflen = buf_size,
        };
        TEST_ESP_OK(i2s_channel_preload_data(tx_chan, pre_buf, TEST_I2S_PRELOAD_SIZE, &bytes_written));
        TEST_ESP_OK(lp_i2s_channel_enable(rx_chan));
        ESP_LOGI(TAG, "reading...");
        TEST_ESP_OK(i2s_channel_enable(tx_chan));

        for (int i = 0; i < 100; i++) {
            ret = i2s_channel_write(tx_chan, tx_buf, buf_size, &bytes_written, 0);
            if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
                TEST_ESP_OK(ret);
            }

            TEST_ESP_OK(lp_i2s_channel_read(rx_chan, &trans, LP_I2S_MAX_DELAY));
            s_data_check(rx_buf, buf_size);
        }

        ESP_LOGI(TAG, "success");
        s_test_i2s_deinit(rx_chan, tx_chan, tx_buf, pre_buf, rx_buf);
    }
}

static bool s_lp_i2s_on_thresh_met(lp_i2s_chan_handle_t handle, lp_i2s_evt_data_t *edata, void *user_data)
{
    ESP_DRAM_LOGD(TAG, "edata->trans.received_size: %d", edata->trans.received_size);
    s_data_check(edata->trans.buffer, edata->trans.received_size);

    return false;
}

static bool s_lp_i2s_on_request_new_trans(lp_i2s_chan_handle_t handle, lp_i2s_evt_data_t *edata, void *user_data)
{
    lp_i2s_trans_t trans = *(lp_i2s_trans_t *)user_data;
    edata->trans.buffer = trans.buffer;
    edata->trans.buflen = trans.buflen;

    return false;
}

TEST_CASE("test LP I2S read from ISR for STD", "[lp_i2s]")
{
    lp_i2s_chan_handle_t rx_chan = NULL;
    i2s_chan_handle_t tx_chan = NULL;
    size_t buf_size = 512;

    for (int i = TEST_I2S_MODE_STD_PHILIPS; i <= TEST_I2S_MODE_STD_PCM; i++) {
        uint8_t *tx_buf = NULL;
        uint8_t *pre_buf = NULL;
        uint8_t *rx_buf = NULL;
        s_test_i2s_init(i, buf_size, &rx_chan, &tx_chan, &tx_buf, &pre_buf, &rx_buf);

        lp_i2s_trans_t trans = {
            .buffer = rx_buf,
            .buflen = buf_size,
        };

        lp_i2s_evt_cbs_t cbs = {
            .on_thresh_met = s_lp_i2s_on_thresh_met,
            .on_request_new_trans = s_lp_i2s_on_request_new_trans,
        };
        TEST_ESP_OK(lp_i2s_register_event_callbacks(rx_chan, &cbs, &trans));

        s_isr_test_flow(rx_chan, tx_chan, buf_size, tx_buf, pre_buf, rx_buf);

        ESP_LOGI(TAG, "success");
        s_test_i2s_deinit(rx_chan, tx_chan, tx_buf, pre_buf, rx_buf);
    }
}

TEST_CASE("test lp_i2s_channel_read_until_bytes", "[lp_i2s]")
{
    esp_err_t ret = ESP_FAIL;
    lp_i2s_chan_handle_t rx_chan = NULL;
    i2s_chan_handle_t tx_chan = NULL;
    size_t buf_size = 2048;
    size_t bytes_written = 0;
    uint8_t *tx_buf = NULL;
    uint8_t *pre_buf = NULL;
    uint8_t *rx_buf = NULL;
    s_test_i2s_init(TEST_I2S_MODE_STD_PHILIPS, buf_size, &rx_chan, &tx_chan, &tx_buf, &pre_buf, &rx_buf);

    TEST_ESP_OK(i2s_channel_preload_data(tx_chan, pre_buf, TEST_I2S_PRELOAD_SIZE, &bytes_written));
    TEST_ESP_OK(lp_i2s_channel_enable(rx_chan));
    ESP_LOGI(TAG, "reading...");
    TEST_ESP_OK(i2s_channel_enable(tx_chan));

    ret = i2s_channel_write(tx_chan, tx_buf, buf_size, &bytes_written, 0);
    if (ret != ESP_OK && ret != ESP_ERR_TIMEOUT) {
        TEST_ESP_OK(ret);
    }

    lp_i2s_trans_t trans = {
        .buffer = rx_buf,
        .buflen = buf_size,
    };
    TEST_ESP_OK(lp_i2s_channel_read_until_bytes(rx_chan, &trans));
    s_data_check(rx_buf, buf_size);

    ESP_LOGI(TAG, "success");
    s_test_i2s_deinit(rx_chan, tx_chan, tx_buf, pre_buf, rx_buf);
}
