/*
 * SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi sio mode
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "soc/spi_periph.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "test_dualboard_utils.h"

#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
#include "driver/spi_slave_hd.h"
#include "esp_serial_slave_link/essl_spi.h"
#endif

#if (TEST_SPI_PERIPH_NUM >= 2)
//These will be only enabled on chips with 2 or more SPI peripherals

/********************************************************************************
 *      Test SIO
 ********************************************************************************/

TEST_CASE("SPI Single Board Test SIO", "[spi]")
{
    //Master init
    spi_device_handle_t spi;
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();

    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_DISABLED));

    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE;
    dev_cfg.clock_speed_hz = 4 * 1000 * 1000;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    //Slave init
    bus_cfg.flags = 0;
    spi_slave_interface_config_t slv_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slv_cfg, SPI_DMA_DISABLED));

    same_pin_func_sel(TEST_SPI_HOST, TEST_SLAVE_HOST, bus_cfg, dev_cfg.spics_io_num);
    // fix sio internal connection
    spitest_gpio_input_sel(bus_cfg.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_in);

    WORD_ALIGNED_ATTR uint8_t master_rx_buffer[320];
    WORD_ALIGNED_ATTR uint8_t slave_rx_buffer[320];

    spi_transaction_t mst_trans;
    spi_slave_transaction_t slv_trans;
    spi_slave_transaction_t *ret;

    for (int i = 0; i < 8; i ++) {
        int tlen = i * 2 + 1;
        int rlen = 9 - i;

        ESP_LOGI("spi", "=========== TEST(%d) Master TX, Slave RX ==========", i);

        //Slave RX
        memset(&slv_trans, 0x0, sizeof(spi_slave_transaction_t));
        memset(slave_rx_buffer, 0x66, sizeof(slave_rx_buffer));
        slv_trans.length = tlen * 8;
        slv_trans.rx_buffer = slave_rx_buffer + tlen * 8;
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slv_trans, portMAX_DELAY));

        //Master TX
        memset(&mst_trans, 0x0, sizeof(spi_transaction_t));
        mst_trans.length = tlen * 8;
        mst_trans.tx_buffer = spitest_master_send;
        TEST_ESP_OK(spi_device_transmit(spi, &mst_trans));

        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret, portMAX_DELAY));
        TEST_ASSERT(ret == &slv_trans);

        ESP_LOG_BUFFER_HEXDUMP("master tx", mst_trans.tx_buffer, tlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("slave rx", slv_trans.rx_buffer, tlen, ESP_LOG_INFO);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(mst_trans.tx_buffer, slv_trans.rx_buffer, tlen);

        ESP_LOGI("spi", "=========== TEST(%d) Master RX, Slave TX ==========", i);
        //Slave TX
        memset(&slv_trans, 0x0, sizeof(spi_slave_transaction_t));

        slv_trans.length = rlen * 8;
        slv_trans.tx_buffer = spitest_slave_send + rlen * 8;
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slv_trans, portMAX_DELAY));

        //Master RX
        memset(&mst_trans, 0x0, sizeof(spi_transaction_t));
        memset(master_rx_buffer, 0x66, sizeof(master_rx_buffer));
        mst_trans.rxlength = rlen * 8;
        mst_trans.rx_buffer = master_rx_buffer;
        TEST_ESP_OK(spi_device_transmit(spi, &mst_trans));

        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret, portMAX_DELAY));
        TEST_ASSERT(ret == &slv_trans);

        ESP_LOG_BUFFER_HEXDUMP("slave tx", slv_trans.tx_buffer, rlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("master rx", mst_trans.rx_buffer, rlen, ESP_LOG_INFO);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(slv_trans.tx_buffer, mst_trans.rx_buffer, rlen);
    }

    spi_slave_free(TEST_SLAVE_HOST);
    master_free_device_bus(spi);
}
#endif //#if (TEST_SPI_PERIPH_NUM >= 2)

/********************************************************************************
 *                             Test SIO Master + SIO Slave HD
 ********************************************************************************/
#if SOC_SPI_SUPPORT_SLAVE_HD_VER2
#define TRANS_LEN       1024
#define TEST_NUM        8

void test_sio_master_trans(void)
{
    spi_device_handle_t dev;
    uint8_t *master_tx = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *master_rx = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *rx_exp = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    TEST_ASSERT_TRUE_MESSAGE(master_tx && master_rx && rx_exp, "malloc failed, exit.\n");
    test_fill_random_to_buffers_dualboard(1, master_tx, rx_exp, TRANS_LEN);

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.miso_io_num = -1;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE;
    dev_cfg.command_bits = 8;
    dev_cfg.address_bits = 8;
    dev_cfg.dummy_bits = 8;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &dev));
    printf("CS:CLK:SIO: %d\t%d\t%d\n", dev_cfg.spics_io_num, bus_cfg.sclk_io_num, bus_cfg.mosi_io_num);

    unity_send_signal("Master ready");
    for (int i = TEST_NUM; i > 0; i --) {
        size_t trans_len = TRANS_LEN >> i;

        unity_wait_for_signal("Slave ready");
        TEST_ESP_OK(essl_spi_wrdma(dev, master_tx, trans_len, -1, 0));
        ESP_LOG_BUFFER_HEXDUMP("master tx", master_tx, trans_len, ESP_LOG_INFO);

        memset(master_rx, 0, trans_len);
        TEST_ESP_OK(essl_spi_rddma(dev, master_rx, trans_len, -1, 0));
        ESP_LOG_BUFFER_HEXDUMP("master rx", master_rx, trans_len, ESP_LOG_INFO);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(rx_exp, master_rx, trans_len);
    }

    free(master_tx);
    free(master_rx);
    free(rx_exp);
    master_free_device_bus(dev);
}

void test_sio_slave_trans(void)
{
    spi_slave_hd_data_t *ret_trans;
    uint8_t *slave_tx = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *slave_rx = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    uint8_t *rx_exp = heap_caps_malloc(TRANS_LEN, MALLOC_CAP_DMA);
    TEST_ASSERT_TRUE_MESSAGE(slave_tx && slave_rx && rx_exp, "malloc failed, exit.\n");
    test_fill_random_to_buffers_dualboard(1, rx_exp, slave_tx, TRANS_LEN);

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.miso_io_num = -1;
    bus_cfg.max_transfer_sz = TRANS_LEN;
    spi_slave_hd_slot_config_t slave_hd_cfg = SPI_SLOT_TEST_DEFAULT_CONFIG();
    slave_hd_cfg.flags = SPI_SLAVE_HD_3WIRE_MODE;
    printf("CS:CLK:SIO: %d\t%d\t%d\n", (int)slave_hd_cfg.spics_io_num, bus_cfg.sclk_io_num, bus_cfg.mosi_io_num);

    unity_wait_for_signal("Master ready");
    TEST_ESP_OK(spi_slave_hd_init(TEST_SLAVE_HOST, &bus_cfg, &slave_hd_cfg));

    for (int i = TEST_NUM; i > 0; i --) {
        size_t trans_len = TRANS_LEN >> i;
        spi_slave_hd_data_t rx_trans = {
            .data = slave_rx,
            .len = trans_len,
            .flags = SPI_SLAVE_HD_TRANS_DMA_BUFFER_ALIGN_AUTO,
        };
        spi_slave_hd_data_t tx_trans = {
            .data = slave_tx,
            .len = trans_len,
        };

        memset(slave_rx, 0, trans_len);
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &rx_trans, portMAX_DELAY));
        TEST_ESP_OK(spi_slave_hd_queue_trans(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &tx_trans, portMAX_DELAY));
        unity_send_signal("Slave ready");
        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_RX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT_EQUAL(&rx_trans, ret_trans);
        ESP_LOG_BUFFER_HEXDUMP("Slave rx", rx_trans.data, trans_len, ESP_LOG_INFO);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(rx_exp, rx_trans.data, trans_len);

        TEST_ESP_OK(spi_slave_hd_get_trans_res(TEST_SLAVE_HOST, SPI_SLAVE_CHAN_TX, &ret_trans, portMAX_DELAY));
        TEST_ASSERT_EQUAL(&tx_trans, ret_trans);
        ESP_LOG_BUFFER_HEXDUMP("Slave tx", tx_trans.data, trans_len, ESP_LOG_INFO);
    }

    free(slave_tx);
    free(slave_rx);
    free(rx_exp);
    spi_slave_hd_deinit(TEST_SLAVE_HOST);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Master:Test_SIO_Mode_Multi_Board", "[spi_ms][test_env=generic_multi_device]", test_sio_master_trans, test_sio_slave_trans);
#endif // SOC_SPI_SUPPORT_SLAVE_HD_VER2
