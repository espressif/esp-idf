/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi_master device driver
*/

#include "sdkconfig.h"

#include "esp_heap_caps.h"
#include "esp_log.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_private/spi_slave_internal.h"
#include "soc/spi_pins.h"
#include "soc/spi_periph.h"

#define TEST_BUF_SIZE    32
#define TEST_TIMES       4

static void test_master(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(SPI2_HOST, &buscfg, 0));

    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000 * 1000,
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 16,
        .cs_ena_pretrans = 0,
        .cs_ena_posttrans = 0,
        .input_delay_ns = 62.5,
    };
    TEST_ESP_OK(spi_bus_add_device(SPI2_HOST, &devcfg, &spi));

    unity_send_signal("Master ready");

    uint8_t *mst_tx_buf[TEST_TIMES] = {};
    uint8_t *mst_rx_buf[TEST_TIMES] = {};
    uint8_t *slv_tx_buf[TEST_TIMES] = {};
    for (int i = 0; i < TEST_TIMES; i++) {
        uint32_t test_seed = i * 100 + 99;
        mst_tx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        mst_rx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        slv_tx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        test_fill_random_to_buffers_dualboard(test_seed, mst_tx_buf[i], slv_tx_buf[i], TEST_BUF_SIZE);
    }

    //Trans0
    spi_transaction_t trans0 = {
        .tx_buffer = mst_tx_buf[0],
        .rx_buffer = mst_rx_buf[0],
        .length = TEST_BUF_SIZE * 8,
    };
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&trans0));
    //show result
    ESP_LOG_BUFFER_HEX("master tx:", mst_tx_buf[0], TEST_BUF_SIZE);
    ESP_LOG_BUFFER_HEX("master rx:", mst_rx_buf[0], TEST_BUF_SIZE);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_tx_buf[0], mst_rx_buf[0], TEST_BUF_SIZE);

    //Trans1
    spi_transaction_t trans1 = {
        .tx_buffer = mst_tx_buf[1],
        .rx_buffer = mst_rx_buf[1],
        .length = TEST_BUF_SIZE * 8,
    };
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&trans1));
    //show result
    ESP_LOG_BUFFER_HEX("master tx:", mst_tx_buf[1], TEST_BUF_SIZE);
    ESP_LOG_BUFFER_HEX("master rx:", mst_rx_buf[1], TEST_BUF_SIZE);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_tx_buf[1], mst_rx_buf[1], TEST_BUF_SIZE);

    //Trans2
    spi_transaction_t trans2 = {
        .tx_buffer = mst_tx_buf[2],
        .rx_buffer = mst_rx_buf[2],
        .length = TEST_BUF_SIZE * 8,
    };
    // unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&trans2));
    //show result
    ESP_LOG_BUFFER_HEX("master tx:", mst_tx_buf[2], TEST_BUF_SIZE);
    ESP_LOG_BUFFER_HEX("master rx:", mst_rx_buf[2], TEST_BUF_SIZE);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_tx_buf[2], mst_rx_buf[2], TEST_BUF_SIZE);

    //Trans3
    spi_transaction_t trans3 = {
        .tx_buffer = mst_tx_buf[3],
        .rx_buffer = mst_rx_buf[3],
        .length = TEST_BUF_SIZE * 8,
    };
    // unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&trans3));
    //show result
    ESP_LOG_BUFFER_HEX("master tx:", mst_tx_buf[3], TEST_BUF_SIZE);
    ESP_LOG_BUFFER_HEX("master rx:", mst_rx_buf[3], TEST_BUF_SIZE);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_tx_buf[3], mst_rx_buf[3], TEST_BUF_SIZE);

    for (int i = 0; i < TEST_TIMES; i++) {
        free(mst_tx_buf[i]);
        free(mst_rx_buf[i]);
        free(slv_tx_buf[i]);
    }

    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(SPI2_HOST) == ESP_OK);
}

static void test_slave(void)
{
    unity_wait_for_signal("Master ready");
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 10,
    };
    TEST_ESP_OK(spi_slave_initialize(SPI2_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));

    uint8_t *slv_tx_buf[TEST_TIMES] = {};
    uint8_t *slv_rx_buf[TEST_TIMES] = {};
    uint8_t *mst_tx_buf[TEST_TIMES] = {};
    for (int i = 0; i < TEST_TIMES; i++) {
        uint32_t test_seed = i * 100 + 99;
        slv_tx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        slv_rx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        mst_tx_buf[i] = heap_caps_calloc(TEST_BUF_SIZE, 1, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
        test_fill_random_to_buffers_dualboard(test_seed, mst_tx_buf[i], slv_tx_buf[i], TEST_BUF_SIZE);
    }

    spi_slave_transaction_t trans[TEST_TIMES] = {};
    spi_slave_transaction_t *ret_trans = NULL;

    //Trans0
    trans[0].tx_buffer = slv_tx_buf[0];
    trans[0].rx_buffer = slv_rx_buf[0];
    trans[0].length = TEST_BUF_SIZE * 8;
    trans[0].flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &trans[0], portMAX_DELAY));
    unity_send_signal("Slave ready");
    TEST_ESP_OK(spi_slave_get_trans_result(SPI2_HOST, &ret_trans, portMAX_DELAY));
    TEST_ASSERT(ret_trans == &trans[0]);
    ESP_LOG_BUFFER_HEX("slave tx:", slv_tx_buf[0], TEST_BUF_SIZE);
    ESP_LOG_BUFFER_HEX("slave rx:", slv_rx_buf[0], TEST_BUF_SIZE);

    /**
     * Now we push few more dummy transactions to the slave trans queue,
     * to see if `spi_slave_queue_reset()` works
     */
    uint32_t dummy_data0 = 0xAA;
    spi_slave_transaction_t dummy_trans0 = {
        .tx_buffer = &dummy_data0,
        .length = sizeof(uint32_t) * 8,
        .flags = SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO,
    };
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &dummy_trans0, portMAX_DELAY));
    uint32_t dummy_data1 = 0xBB;
    spi_slave_transaction_t dummy_trans1 = {
        .tx_buffer = &dummy_data1,
        .length = sizeof(uint32_t) * 8,
        .flags = SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO,
    };
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &dummy_trans1, portMAX_DELAY));
    uint32_t dummy_data2 = 0xCC;
    spi_slave_transaction_t dummy_trans2 = {
        .tx_buffer = &dummy_data2,
        .length = sizeof(uint32_t) * 8,
        .flags = SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO,
    };
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &dummy_trans2, portMAX_DELAY));
    uint32_t dummy_data3 = 0xDD;
    spi_slave_transaction_t dummy_trans3 = {
        .tx_buffer = &dummy_data3,
        .length = sizeof(uint32_t) * 8,
        .flags = SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO,
    };
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &dummy_trans3, portMAX_DELAY));

    //Trans1~3
    spi_slave_queue_reset(SPI2_HOST);
    trans[1].tx_buffer = slv_tx_buf[1];
    trans[1].rx_buffer = slv_rx_buf[1];
    trans[1].length = TEST_BUF_SIZE * 8;
    trans[1].flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &trans[1], portMAX_DELAY));

    trans[2].tx_buffer = slv_tx_buf[2];
    trans[2].rx_buffer = slv_rx_buf[2];
    trans[2].length = TEST_BUF_SIZE * 8;
    trans[2].flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &trans[2], portMAX_DELAY));

    trans[3].tx_buffer = slv_tx_buf[3];
    trans[3].rx_buffer = slv_rx_buf[3];
    trans[3].length = TEST_BUF_SIZE * 8;
    trans[3].flags |= SPI_SLAVE_TRANS_DMA_BUFFER_ALIGN_AUTO;
    TEST_ESP_OK(spi_slave_queue_trans(SPI2_HOST, &trans[3], portMAX_DELAY));

    unity_send_signal("Slave ready");
    for (int i = 1; i < TEST_TIMES; i++) {
        TEST_ESP_OK(spi_slave_get_trans_result(SPI2_HOST, &ret_trans, portMAX_DELAY));
        TEST_ASSERT(ret_trans == &trans[i]);
        ESP_LOG_BUFFER_HEX("slave tx:", slv_tx_buf[i], TEST_BUF_SIZE);
        ESP_LOG_BUFFER_HEX("slave rx:", slv_rx_buf[i], TEST_BUF_SIZE);
    }

    for (int i = 0; i < TEST_TIMES; i++) {
        free(slv_tx_buf[i]);
        free(slv_rx_buf[i]);
        free(mst_tx_buf[i]);
    }

    TEST_ESP_OK(spi_slave_free(SPI2_HOST));
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Slave_Reset_Queue_Test", "[spi_ms][timeout=120]", test_master, test_slave);
