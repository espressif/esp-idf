/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "unity.h"
#include "driver/i2s_std.h"
#include "esp_attr.h"
#include "soc/soc_caps.h"
#include "esp_private/i2s_platform.h"
#include "esp_private/spi_flash_os.h"
#include "../../test_inc/test_i2s.h"

#if CONFIG_I2S_ISR_IRAM_SAFE

#define GET_DMA_BUFFERS_BY_OFFSET(base_addr, offset)   (uint8_t **)(*((uint32_t *)base_addr + offset))

static bool IRAM_ATTR test_i2s_tx_done_callback(i2s_chan_handle_t handle, i2s_event_data_t *event, void *user_ctx)
{
    int *is_triggered = (int *)user_ctx;
    if (*(uint8_t *)(event->data) != 0) {
        *is_triggered = 1;
    }
    return false;
}

static void IRAM_ATTR test_i2s_iram_write(i2s_chan_handle_t tx_handle)
{
    // Get the DMA buf pointer via the offset of 'bufs' field in i2s_channel_t struct
    size_t offset = i2s_platform_get_dma_buffer_offset() / sizeof(uint32_t); // Get the offset and transfer to unit 'uint32_t' (i.e. 4 bytes)
    uint8_t **dma_bufs = GET_DMA_BUFFERS_BY_OFFSET(tx_handle, offset);

    // disable cache and non-iram ISR handlers
    spi_flash_guard_get()->start();
    // write data into dma buffer directly, the data in dma buffer will be sent automatically
    for (int i=0; i < 100; i++) {
        dma_bufs[0][i] = i + 1;
    }
    // enable cache and non-iram ISR handlers
    spi_flash_guard_get()->end();
}

TEST_CASE("i2s_iram_interrupt_safe", "[i2s]")
{
    i2s_chan_handle_t tx_chan = NULL;
    i2s_chan_handle_t rx_chan = NULL;

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = 6;
    chan_cfg.dma_frame_num = 200;
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &tx_chan, &rx_chan));
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(16000),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = MASTER_MCK_IO,
            .bclk = MASTER_BCK_IO,
            .ws = MASTER_WS_IO,
            .dout = DATA_OUT_IO,
            .din = DATA_OUT_IO, // same gpio to loopback
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    TEST_ESP_OK(i2s_channel_init_std_mode(tx_chan, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(rx_chan, &std_cfg));
    int is_triggerred = 0;
    i2s_event_callbacks_t cbs = {
        .on_recv = NULL,
        .on_recv_q_ovf = NULL,
        .on_sent = test_i2s_tx_done_callback,
        .on_send_q_ovf = NULL,
    };
    TEST_ESP_OK(i2s_channel_register_event_callback(tx_chan, &cbs, &is_triggerred));
    TEST_ESP_OK(i2s_channel_enable(tx_chan));
    TEST_ESP_OK(i2s_channel_enable(rx_chan));

    uint8_t *recv_buf = (uint8_t *)calloc(1, 2000);
    TEST_ASSERT(recv_buf != NULL);
    size_t r_bytes;
    int i = 0;
    test_i2s_iram_write(tx_chan);
    for (int retry = 0; retry < 3; retry++) {
        i2s_channel_read(rx_chan, recv_buf, 2000, &r_bytes, pdMS_TO_TICKS(1000));
        for (i = 0; i < 2000 - 100; i++) {
            if (recv_buf[i] != 0) {
                goto finish;
            }
        }
    }
finish:
    TEST_ESP_OK(i2s_channel_disable(tx_chan));
    TEST_ESP_OK(i2s_channel_disable(rx_chan));
    TEST_ESP_OK(i2s_del_channel(tx_chan));
    TEST_ESP_OK(i2s_del_channel(rx_chan));

    TEST_ASSERT(i < (2000 - 100));
    for (int j = 1; j <= 100; j++) {
        TEST_ASSERT_EQUAL_UINT8(recv_buf[i++], j);
    }
    TEST_ASSERT(is_triggerred);
    free(recv_buf);
}

#endif // CONFIG_PCNT_ISR_IRAM_SAFE
