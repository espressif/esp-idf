/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "test_utils.h"
#include "esp_heap_caps.h"
#include "driver/spi_master.h"
#include "esp_private/spi_master_internal.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"
#include "driver/spi_slave_hd.h"
#include "driver/spi_slave.h"
#include "soc/spi_pins.h"
#include "test_spi_utils.h"

__attribute__((unused)) static const char *TAG = "SCT";

/*-----------------------------------------------------------
 * HD SCT Functional Test
 *-----------------------------------------------------------*/
#define TEST_HD_TIMES                  4
//Master write, slave read, wrt slave reg
#define TEST_HD_BUF_0_ID               12
#define TEST_HD_BUF_0_VAL              0x99
//Master read, slave write, wrt slave reg
#define TEST_HD_BUF_1_ID               13
#define TEST_HD_BUF_1_VAL              0xAA

#define TEST_HD_DATA_LEN               64
#define TEST_HD_DATA_LEN_PER_SEG       32

static void hd_master(void)
{
    spi_device_handle_t handle;

    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.max_transfer_sz = 4092 * 10;

    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.command_bits = 8;
    devcfg.address_bits = 8;
    devcfg.dummy_bits = 8;
    devcfg.clock_speed_hz = 10 * 1000;
    devcfg.input_delay_ns = 0;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;

    TEST_ESP_OK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(SPI2_HOST, &devcfg, &handle));
    unity_send_signal("Master ready");

    //Test data preparation
    uint32_t master_tx_val = TEST_HD_BUF_0_VAL;
    uint8_t *master_tx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *master_rx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t master_rx_val = 0;
    uint8_t *slave_tx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    test_fill_random_to_buffers_dualboard(199, master_tx_buf, slave_tx_buf, TEST_HD_DATA_LEN);

    spi_multi_transaction_t *ret_seg_trans = NULL;

    //---------------------Master TX---------------------------//
    spi_multi_transaction_t tx_seg_trans[TEST_HD_TIMES] = {
        {
            .base = {
                .cmd = 0x1,
                .addr = TEST_HD_BUF_0_ID,
                .length = 4 * 8,
                .tx_buffer = (uint8_t *) &master_tx_val,
            },
        },
        //TEST_HD_DATA_LEN of TX data, split into 2 segments. `TEST_HD_DATA_LEN_PER_SEG` per segment
        {
            .base = {
                .cmd = 0x3,
                .length = TEST_HD_DATA_LEN_PER_SEG * 8,
                .tx_buffer = master_tx_buf,
            },
            .dummy_bits = 8,
            .seg_trans_flags = SPI_MULTI_TRANS_DUMMY_LEN_UPDATED,
        },
        {
            .base = {
                .cmd = 0x3,
                .length = TEST_HD_DATA_LEN_PER_SEG * 8,
                .tx_buffer = master_tx_buf + TEST_HD_DATA_LEN_PER_SEG,
            },
            .dummy_bits = 8,
            .seg_trans_flags = SPI_MULTI_TRANS_DUMMY_LEN_UPDATED,
        },
        {
            .base = {
                .cmd = 0x7,
            }
        },
    };

    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, true));
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, tx_seg_trans, TEST_HD_TIMES, portMAX_DELAY));
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ASSERT(ret_seg_trans == tx_seg_trans);
    ESP_LOG_BUFFER_HEX("Master tx", master_tx_buf, TEST_HD_DATA_LEN);
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, false));

    //---------------------Master RX---------------------------//
    spi_multi_transaction_t rx_seg_trans[TEST_HD_TIMES] = {
        {
            .base = {
                .cmd = 0x2,
                .addr = TEST_HD_BUF_1_ID,
                .rxlength = 4 * 8,
                .rx_buffer = (uint8_t *) &master_rx_val,
            },
        },
        // TEST_HD_DATA_LEN of TX data, split into 2 segments. `TEST_HD_DATA_LEN_PER_SEG` per segment
        {
            .base = {
                .cmd = 0x4,
                .rxlength = TEST_HD_DATA_LEN_PER_SEG * 8,
                .rx_buffer = master_rx_buf,
            },
            .dummy_bits = 8,
            .seg_trans_flags = SPI_MULTI_TRANS_DUMMY_LEN_UPDATED,
        },
        {
            .base = {
                .cmd = 0x4,
                .rxlength = TEST_HD_DATA_LEN_PER_SEG * 8,
                .rx_buffer = master_rx_buf + TEST_HD_DATA_LEN_PER_SEG,
            },
            .dummy_bits = 8,
            .seg_trans_flags = SPI_MULTI_TRANS_DUMMY_LEN_UPDATED,
        },
        {
            .base = {
                .cmd = 0x8,
            }
        },
    };
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, true));

    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, rx_seg_trans, TEST_HD_TIMES, portMAX_DELAY));
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ASSERT(ret_seg_trans == rx_seg_trans);

    ESP_LOGI("Master", "Slave Reg[%d] value is: 0x%" PRIx32, TEST_HD_BUF_1_ID, master_rx_val);
    TEST_ASSERT(master_rx_val == TEST_HD_BUF_1_VAL);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_tx_buf, master_rx_buf, TEST_HD_DATA_LEN);
    ESP_LOG_BUFFER_HEX("Master rx", master_rx_buf, TEST_HD_DATA_LEN);

    //Memory Recycle
    free(master_tx_buf);
    free(master_rx_buf);
    free(slave_tx_buf);

    TEST_ESP_OK(spi_bus_remove_device(handle));
    TEST_ESP_OK(spi_bus_free(SPI2_HOST));
}

static void hd_slave(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_hd_slot_config_t slave_hd_cfg = SPI_SLOT_TEST_DEFAULT_CONFIG();
    slave_hd_cfg.dma_chan = SPI_DMA_CH_AUTO,

    TEST_ESP_OK(spi_slave_hd_init(SPI2_HOST, &buscfg, &slave_hd_cfg));

    spi_slave_hd_data_t *ret_trans = NULL;

    //Test data preparation
    uint32_t slave_tx_val = TEST_HD_BUF_1_VAL;
    uint8_t *slave_tx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_rx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint32_t slave_rx_val = 0;
    uint8_t *master_tx_buf = heap_caps_calloc(1, TEST_HD_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    test_fill_random_to_buffers_dualboard(199, master_tx_buf, slave_tx_buf, TEST_HD_DATA_LEN);

    unity_wait_for_signal("Master ready");

    //---------------------Slave RX---------------------------//
    spi_slave_hd_data_t slave_rx_trans = {
        .data = slave_rx_buf,
        .len = TEST_HD_DATA_LEN,
    };
    TEST_ESP_OK(spi_slave_hd_queue_trans(SPI2_HOST, SPI_SLAVE_CHAN_RX, &slave_rx_trans, portMAX_DELAY));
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_hd_get_trans_res(SPI2_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
    TEST_ASSERT(ret_trans == &slave_rx_trans);

    TEST_ASSERT_EQUAL_HEX8_ARRAY(master_tx_buf, slave_rx_buf, TEST_HD_DATA_LEN);
    ESP_LOG_BUFFER_HEX("Slave rx", slave_rx_buf, TEST_HD_DATA_LEN);

    spi_slave_hd_read_buffer(SPI2_HOST, TEST_HD_BUF_0_ID, (uint8_t *)&slave_rx_val, 4);
    ESP_LOGI("Slave", "Slave Reg[%d] value is: 0x%" PRIx32, TEST_HD_BUF_0_ID, slave_rx_val);
    TEST_ASSERT(slave_rx_val == TEST_HD_BUF_0_VAL);

    //---------------------Slave TX---------------------------//
    spi_slave_hd_write_buffer(SPI2_HOST, TEST_HD_BUF_1_ID, (uint8_t *)&slave_tx_val, 4);
    spi_slave_hd_data_t slave_tx_trans = {
        .data = slave_tx_buf,
        .len = TEST_HD_DATA_LEN,
    };
    TEST_ESP_OK(spi_slave_hd_queue_trans(SPI2_HOST, SPI_SLAVE_CHAN_TX, &slave_tx_trans, portMAX_DELAY));
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_hd_get_trans_res(SPI2_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
    TEST_ASSERT(ret_trans == &slave_tx_trans);
    ESP_LOG_BUFFER_HEX("Slave tx", slave_tx_buf, TEST_HD_DATA_LEN);

    //Memory Recycle
    free(slave_tx_buf);
    free(slave_rx_buf);
    free(master_tx_buf);

    TEST_ESP_OK(spi_slave_hd_deinit(SPI2_HOST));
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Master_SCT_HD_Functional", "[spi_ms]", hd_master, hd_slave);

#define SCT_LONG_TRANS_SEG_LEN  10000   //anything lager than 4092
TEST_CASE("spi_master: test_sct_dma_desc_oob_on_tail", "[spi]")
{
    spi_device_handle_t handle;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.max_transfer_sz = 4092 * 8;

    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.clock_speed_hz = 100 * 1000; //low speed ensure cpu faster then HW
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;

    TEST_ESP_OK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(SPI2_HOST, &devcfg, &handle));
    printf("master init OK\n");
    uint8_t *master_tx_buf = heap_caps_calloc(1, SCT_LONG_TRANS_SEG_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    //---------------------Master TX---------------------------//
    spi_multi_transaction_t tx_seg_trans_cmd[1] = { //uing 1 dma desc, test trans without data phases
        {
            .base = {
                .cmd = 0x7,
            }
        },
    };
    spi_multi_transaction_t tx_seg_trans_dummy[1] = { //using 4 dma desc
        {
            .base = {
                .length = SCT_LONG_TRANS_SEG_LEN * 8,
                .tx_buffer = master_tx_buf,
            },
        },
    };
    spi_multi_transaction_t tx_seg_trans_data[1] = {  //using 4 dma desc, should using (last 3 + first 1) of desc pool
        {
            .base = {
                .cmd = 0x3,
                .length = SCT_LONG_TRANS_SEG_LEN * 8,
                .tx_buffer = master_tx_buf,
            },
            .dummy_bits = 8,
            .seg_trans_flags = SPI_MULTI_TRANS_DUMMY_LEN_UPDATED,
        },
    };

    spi_multi_transaction_t *ret_seg_trans;
    printf("enabling sct multi trans mode ...\n");
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, true));
    printf("start sct transaction\n");
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, tx_seg_trans_cmd, 1, portMAX_DELAY));
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, tx_seg_trans_dummy, 1, portMAX_DELAY));
    vTaskDelay(10 / portTICK_PERIOD_MS);  //ensure `tx_seg_trans_cmd` had finish
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, tx_seg_trans_data, 1, portMAX_DELAY));

    printf("waiting result ...\n");
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, false));

    free(master_tx_buf);
    TEST_ESP_OK(spi_bus_remove_device(handle));
    TEST_ESP_OK(spi_bus_free(SPI2_HOST));
}

/*-----------------------------------------------------------
 * Sleep Retention Test
 *-----------------------------------------------------------*/
#define TEST_SLP_DATA_LEN               64
//Master write, slave read, wrt slave reg
#define TEST_SLP_BUF_ID                 12
#define TEST_SLP_BUF_VAL                0x11223344

static void sleep_master(void)
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
#if ESP_SLEEP_POWER_DOWN_CPU
    sleep_cpu_configure(true);
#endif
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    spi_device_handle_t handle;
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    buscfg.max_transfer_sz = 4092 * 10;
#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    buscfg.flags |= SPICOMMON_BUSFLAG_SLP_ALLOW_PD;
#endif
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.command_bits = 8;
    devcfg.address_bits = 8;
    devcfg.dummy_bits = 8;
    devcfg.flags = SPI_DEVICE_HALFDUPLEX;
    TEST_ESP_OK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    TEST_ESP_OK(spi_bus_add_device(SPI2_HOST, &devcfg, &handle));
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, true));
    unity_send_signal("Master ready");

    //Test data preparation
    uint32_t master_tx_val = TEST_SLP_BUF_VAL;
    uint8_t *master_tx_buf = heap_caps_calloc(1, TEST_SLP_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    test_fill_random_to_buffers_dualboard(199, master_tx_buf, master_tx_buf, TEST_SLP_DATA_LEN);
    //---------------------Master TX---------------------------//
    spi_multi_transaction_t *ret_seg_trans = NULL;
    spi_multi_transaction_t tx_seg_trans[3] = {
        {
            .base = {
                .cmd = 0x1,
                .addr = TEST_SLP_BUF_ID,
                .length = 4 * 8,
                .tx_buffer = (uint8_t *) &master_tx_val,
            },
        },
        {
            .base = {
                .cmd = 0x3,
                .addr = 0xf2,
                .length = TEST_SLP_DATA_LEN * 8,
                .tx_buffer = master_tx_buf,
            },
            .dummy_bits = 8,
        },
        {
            .base = {
                .cmd = 0x7,
            },
        },
    };

    unity_wait_for_signal("Slave ready");
    printf("Going into sleep with power down ...\n");
    TEST_ESP_OK(esp_light_sleep_start());
    printf("Waked up!\n");
    // check if the sleep happened as expected
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_SPI_SUPPORT_SLEEP_RETENTION && CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL((buscfg.flags & SPICOMMON_BUSFLAG_SLP_ALLOW_PD) ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif
    TEST_ESP_OK(spi_device_queue_multi_trans(handle, tx_seg_trans, 3, portMAX_DELAY));
    TEST_ESP_OK(spi_device_get_multi_trans_result(handle, &ret_seg_trans, portMAX_DELAY));
    TEST_ASSERT(ret_seg_trans == tx_seg_trans);
    ESP_LOG_BUFFER_HEX("Master tx", master_tx_buf, TEST_SLP_DATA_LEN);

    free(master_tx_buf);
    TEST_ESP_OK(spi_bus_multi_trans_mode_enable(handle, false));
    TEST_ESP_OK(spi_bus_remove_device(handle));
    TEST_ESP_OK(spi_bus_free(SPI2_HOST));
    esp_sleep_set_sleep_context(NULL);
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
}

static void sleep_slave(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_hd_slot_config_t slave_hd_cfg = SPI_SLOT_TEST_DEFAULT_CONFIG();
    slave_hd_cfg.dma_chan = SPI_DMA_CH_AUTO,
    TEST_ESP_OK(spi_slave_hd_init(SPI2_HOST, &buscfg, &slave_hd_cfg));

    //Test data preparation
    uint32_t slave_rx_val = 0;
    uint8_t *slave_rx_buf = heap_caps_calloc(1, TEST_SLP_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *master_tx_buf = heap_caps_calloc(1, TEST_SLP_DATA_LEN, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    test_fill_random_to_buffers_dualboard(199, master_tx_buf, master_tx_buf, TEST_SLP_DATA_LEN);
    //---------------------Slave RX---------------------------//
    spi_slave_hd_data_t *ret_trans = NULL;
    spi_slave_hd_data_t slave_rx_trans = {
        .data = slave_rx_buf,
        .len = TEST_SLP_DATA_LEN,
    };
    unity_wait_for_signal("Master ready");
    TEST_ESP_OK(spi_slave_hd_queue_trans(SPI2_HOST, SPI_SLAVE_CHAN_RX, &slave_rx_trans, portMAX_DELAY));
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_hd_get_trans_res(SPI2_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
    TEST_ASSERT(ret_trans == &slave_rx_trans);

    spitest_cmp_or_dump(master_tx_buf, slave_rx_buf, TEST_SLP_DATA_LEN);
    spi_slave_hd_read_buffer(SPI2_HOST, TEST_SLP_BUF_ID, (uint8_t *)&slave_rx_val, 4);
    ESP_LOGI("Slave", "Slave Reg[%d] value is: 0x%" PRIx32, TEST_SLP_BUF_ID, slave_rx_val);
    TEST_ASSERT(slave_rx_val == TEST_SLP_BUF_VAL);

    free(master_tx_buf);
    free(slave_rx_buf);
    TEST_ESP_OK(spi_slave_hd_deinit(SPI2_HOST));
}
TEST_CASE_MULTIPLE_DEVICES("test_spi_master_sct_sleep_retention", "[spi_ms]", sleep_master, sleep_slave);
