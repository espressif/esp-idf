/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/gpio_etm.h"
#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "driver/i2s_etm.h"
#include "hal/i2s_ll.h"
#include "hal/etm_ll.h"
#include "../../test_inc/test_i2s.h"

#include "soc/soc_etm_struct.h"

#define TEST_DESC_NUM   4
#define TEST_FRAME_NUM  256
#define TEST_BUFF_SIZE  (TEST_DESC_NUM * TEST_FRAME_NUM)

#define TEST_GPIO_ETM_NUM   DATA_IN_IO

static i2s_chan_handle_t s_tx_handle = NULL;
static i2s_chan_handle_t s_rx_handle = NULL;

#if ETM_LL_SUPPORT_STATUS
static void s_i2s_etm_check_status(void)
{
    i2s_dev_t *hw = I2S_LL_GET_HW(0);
    bool is_tx_done = i2s_ll_get_etm_tx_done_event_status(hw);
    bool is_rx_done = i2s_ll_get_etm_rx_done_event_status(hw);
    bool is_tx_reach_thresh = i2s_ll_get_etm_tx_threshold_event_status(hw);
    bool is_rx_reach_thresh = i2s_ll_get_etm_rx_threshold_event_status(hw);
    printf("tx done st %d, rx done st %d, tx x st %d, rx x st %d\n",
           is_tx_done, is_rx_done, is_tx_reach_thresh, is_rx_reach_thresh);
    // TODO: IDF-10512 enable the TX_DONE and RX_DONE check after refactor. Currently not support
    // TEST_ASSERT(is_tx_done);
    // TEST_ASSERT(is_rx_done);
    TEST_ASSERT(is_tx_reach_thresh);
    TEST_ASSERT(is_rx_reach_thresh);
}
#endif  // ETM_LL_SUPPORT_STATUS

static void s_i2s_init(void *buf)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    chan_cfg.dma_desc_num = TEST_DESC_NUM;
    chan_cfg.dma_frame_num = TEST_FRAME_NUM;
    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(48000),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(32, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = I2S_TEST_MASTER_DEFAULT_PIN,
    };
    std_cfg.gpio_cfg.mclk = -1;  // no need mclk
    std_cfg.gpio_cfg.din = DATA_OUT_IO;  // data loopback

    /* I2S channels init */
    TEST_ESP_OK(i2s_new_channel(&chan_cfg, &s_tx_handle, &s_rx_handle));
    TEST_ESP_OK(i2s_channel_init_std_mode(s_tx_handle, &std_cfg));
    TEST_ESP_OK(i2s_channel_init_std_mode(s_rx_handle, &std_cfg));
    // TODO: IDF-10512 rx_stop_mode is necessary for rx_done event, enable it when supported
    // I2S0.rx_conf.rx_stop_mode = 1;

    size_t w_bytes = TEST_BUFF_SIZE;
    while (w_bytes == TEST_BUFF_SIZE) {
        TEST_ESP_OK(i2s_channel_preload_data(s_tx_handle, buf, TEST_BUFF_SIZE, &w_bytes));
    }
}

static void s_i2s_deinit(void)
{
    TEST_ESP_OK(i2s_del_channel(s_rx_handle));
    TEST_ESP_OK(i2s_del_channel(s_tx_handle));
    s_tx_handle = NULL;
    s_rx_handle = NULL;
}

static void s_gpio_init(void)
{
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = (1ULL << TEST_GPIO_ETM_NUM),
    };
    TEST_ESP_OK(gpio_config(&gpio_cfg));
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_ETM_NUM, 0));
}

TEST_CASE("i2s_etm_event_test", "[etm]")
{
    uint32_t *buf = calloc(1, TEST_BUFF_SIZE);
    assert(buf);
    for (int i = 0; i < TEST_BUFF_SIZE / 4; i++) {
        buf[i] = i;
    }

    /* I2S init */
    s_i2s_init(buf);

    /* GPIO init */
    s_gpio_init();

    /* GPIO ETM task */
    gpio_etm_task_config_t gpio_task_cfg = {
        .action = GPIO_ETM_TASK_ACTION_SET,
    };
    esp_etm_task_handle_t gpio_task_handle;
    TEST_ESP_OK(gpio_new_etm_task(&gpio_task_cfg, &gpio_task_handle));
    TEST_ESP_OK(gpio_etm_task_add_gpio(gpio_task_handle, TEST_GPIO_ETM_NUM));

    /* I2S Event init */
    i2s_etm_event_config_t i2s_evt_cfg = {
        .event_type = I2S_ETM_EVENT_REACH_THRESH,
        .threshold = 64,
    };
    esp_etm_event_handle_t i2s_evt_handle;
    TEST_ESP_OK(i2s_new_etm_event(s_rx_handle, &i2s_evt_cfg, &i2s_evt_handle));

    /* ETM connect */
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t etm_channel = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &etm_channel));
    TEST_ESP_OK(esp_etm_channel_connect(etm_channel, i2s_evt_handle, gpio_task_handle));
    TEST_ESP_OK(esp_etm_channel_enable(etm_channel));
    esp_etm_dump(stdout);

    TEST_ESP_OK(i2s_channel_enable(s_tx_handle));
    TEST_ESP_OK(i2s_channel_enable(s_rx_handle));

    TEST_ESP_OK(i2s_channel_read(s_rx_handle, buf, TEST_BUFF_SIZE, NULL, portMAX_DELAY));

#if ETM_LL_SUPPORT_STATUS
    s_i2s_etm_check_status();
#else
    TEST_ASSERT(gpio_get_level(TEST_GPIO_ETM_NUM));
#endif  // ETM_LL_SUPPORT_STATUS

    /* Test finished, free the resources */
    TEST_ESP_OK(i2s_channel_disable(s_rx_handle));
    TEST_ESP_OK(i2s_channel_disable(s_tx_handle));
    free(buf);

    TEST_ESP_OK(esp_etm_channel_disable(etm_channel));
    TEST_ESP_OK(esp_etm_del_event(i2s_evt_handle));
    TEST_ESP_OK(gpio_etm_task_rm_gpio(gpio_task_handle, TEST_GPIO_ETM_NUM));
    TEST_ESP_OK(esp_etm_del_task(gpio_task_handle));
    TEST_ESP_OK(esp_etm_del_channel(etm_channel));

    s_i2s_deinit();
}

TEST_CASE("i2s_etm_task_test", "[etm]")
{
    uint32_t *buf = calloc(1, TEST_BUFF_SIZE);
    assert(buf);
    for (int i = 0; i < TEST_BUFF_SIZE / 4; i++) {
        buf[i] = i;
    }

    /* I2S init */
    s_i2s_init(buf);

    /* GPIO init */
    s_gpio_init();

    /* GPIO ETM event */
    gpio_etm_event_config_t gpio_event_cfg = {
        .edges = {GPIO_ETM_EVENT_EDGE_POS, GPIO_ETM_EVENT_EDGE_NEG},
    };
    esp_etm_event_handle_t gpio_pos_event_handle;
    esp_etm_event_handle_t gpio_neg_event_handle;
    TEST_ESP_OK(gpio_new_etm_event(&gpio_event_cfg, &gpio_pos_event_handle, &gpio_neg_event_handle));
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_pos_event_handle, TEST_GPIO_ETM_NUM));
    TEST_ESP_OK(gpio_etm_event_bind_gpio(gpio_neg_event_handle, TEST_GPIO_ETM_NUM));

    /* I2S Task init */
    i2s_etm_task_config_t i2s_start_task_cfg = {
        .task_type = I2S_ETM_TASK_START,
    };
    esp_etm_task_handle_t i2s_start_task_handle;
    TEST_ESP_OK(i2s_new_etm_task(s_tx_handle, &i2s_start_task_cfg, &i2s_start_task_handle));
    i2s_etm_task_config_t i2s_stop_task_cfg = {
        .task_type = I2S_ETM_TASK_STOP,
    };
    esp_etm_task_handle_t i2s_stop_task_handle;
    TEST_ESP_OK(i2s_new_etm_task(s_tx_handle, &i2s_stop_task_cfg, &i2s_stop_task_handle));

    /* ETM connect */
    esp_etm_channel_config_t etm_config = {};
    esp_etm_channel_handle_t i2s_etm_start_chan = NULL;
    esp_etm_channel_handle_t i2s_etm_stop_chan = NULL;
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &i2s_etm_start_chan));
    TEST_ESP_OK(esp_etm_new_channel(&etm_config, &i2s_etm_stop_chan));
    TEST_ESP_OK(esp_etm_channel_connect(i2s_etm_start_chan, gpio_pos_event_handle, i2s_start_task_handle));
    TEST_ESP_OK(esp_etm_channel_connect(i2s_etm_stop_chan, gpio_neg_event_handle, i2s_stop_task_handle));
    TEST_ESP_OK(esp_etm_channel_enable(i2s_etm_start_chan));
    TEST_ESP_OK(esp_etm_channel_enable(i2s_etm_stop_chan));
    esp_etm_dump(stdout);

    TEST_ESP_OK(i2s_channel_enable(s_tx_handle));
    TEST_ESP_OK(i2s_channel_enable(s_rx_handle));

    /* Test */
    // TX not started, read timeout
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, i2s_channel_read(s_rx_handle, buf, TEST_BUFF_SIZE, NULL, 100));
    // start TX via GPIO pos event
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_ETM_NUM, 1));
    // RX can receive data normally
    TEST_ESP_OK(i2s_channel_read(s_rx_handle, buf, TEST_BUFF_SIZE, NULL, 100));
    // Stop TX via GPIO neg event
    TEST_ESP_OK(gpio_set_level(TEST_GPIO_ETM_NUM, 0));
    // TX stopped, read will timeout when no legacy data in the queue
    esp_err_t ret = ESP_OK;
    // Receive will timeout after TX stopped
    for (int i = 0; i < 20 && ret == ESP_OK; i++) {
        ret = i2s_channel_read(s_rx_handle, buf, TEST_BUFF_SIZE, NULL, 100);
    }
    TEST_ESP_ERR(ESP_ERR_TIMEOUT, ret);

    /* Test finished, free the resources */
    TEST_ESP_OK(i2s_channel_disable(s_rx_handle));
    TEST_ESP_OK(i2s_channel_disable(s_tx_handle));
    free(buf);

    TEST_ESP_OK(esp_etm_channel_disable(i2s_etm_start_chan));
    TEST_ESP_OK(esp_etm_channel_disable(i2s_etm_stop_chan));
    TEST_ESP_OK(esp_etm_del_event(gpio_pos_event_handle));
    TEST_ESP_OK(esp_etm_del_event(gpio_neg_event_handle));
    TEST_ESP_OK(esp_etm_del_task(i2s_start_task_handle));
    TEST_ESP_OK(esp_etm_del_task(i2s_stop_task_handle));
    TEST_ESP_OK(esp_etm_del_channel(i2s_etm_start_chan));
    TEST_ESP_OK(esp_etm_del_channel(i2s_etm_stop_chan));

    s_i2s_deinit();
}
