/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
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
#include "soc/spi_periph.h"
#include "test_utils.h"
#include "test/test_common_spi.h"
#include "soc/gpio_periph.h"

#include "hal/spi_ll.h"

#if (TEST_SPI_PERIPH_NUM >= 2)
//These will be only enabled on chips with 2 or more SPI peripherals

/********************************************************************************
 *      Test SIO
 ********************************************************************************/
#if CONFIG_IDF_TARGET_ESP32
#define MASTER_DIN_SIGNAL    HSPID_IN_IDX
#else
#define MASTER_DIN_SIGNAL    FSPID_IN_IDX
#endif
static void inner_connect(spi_bus_config_t bus)
{
    //Master MOSI(spid_out) output to `mosi_num`
    spitest_gpio_output_sel(bus.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    //Slave MOSI(spid_in) input to `mosi_num`
    spitest_gpio_input_sel(bus.mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spid_in);


    //Master MOSI input(spid_in) to `miso_num`, due to SIO mode, we use Master's `spid_in` to receive data
    spitest_gpio_input_sel(bus.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_in);
    //Slave MISO output(spiq_out)
    spitest_gpio_output_sel(bus.miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
    //Force this signal goes through gpio matrix
    GPIO.func_in_sel_cfg[MASTER_DIN_SIGNAL].sig_in_sel = 1;
}

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


    same_pin_func_sel(bus_cfg, dev_cfg, 0);
    inner_connect(bus_cfg);


    WORD_ALIGNED_ATTR uint8_t master_rx_buffer[320];
    WORD_ALIGNED_ATTR uint8_t slave_rx_buffer[320];

    spi_transaction_t mst_trans;
    spi_slave_transaction_t slv_trans;
    spi_slave_transaction_t* ret;

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


//TODO IDF-4455
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3, ESP32S3, ESP32C2, ESP32H2)
//These tests are ESP32 only due to lack of runners
/********************************************************************************
 *      Test SIO Master & Slave
 ********************************************************************************/
//if test_mosi is false, test on miso of slave, otherwise test on mosi of slave
void test_sio_master_round(bool test_mosi)
{
    spi_device_handle_t spi;
    WORD_ALIGNED_ATTR uint8_t rx_buffer[320];

    if (test_mosi) {
        ESP_LOGI(MASTER_TAG, "======== TEST MOSI ===========");
    } else {
        ESP_LOGI(MASTER_TAG, "======== TEST MISO ===========");
    }

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    if (!test_mosi) bus_cfg.mosi_io_num = bus_cfg.miso_io_num;
    bus_cfg.miso_io_num = -1;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, 0));

    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE;
    dev_cfg.clock_speed_hz = 1*1000*1000;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    for (int i = 0; i < 8; i ++) {
        int tlen = i*2+1;
        int rlen = 9-i;
        spi_transaction_t t = {
            .length = tlen*8,
            .tx_buffer = spitest_master_send+i,
            .rxlength = rlen*8,
            .rx_buffer = rx_buffer+i,
        };
        memset(rx_buffer, 0x66, sizeof(rx_buffer));

        //get signal
        unity_wait_for_signal("slave ready");

        TEST_ESP_OK(spi_device_transmit(spi, &t));
        uint8_t* exp_ptr = spitest_slave_send+i;
        ESP_LOG_BUFFER_HEXDUMP("master tx", t.tx_buffer, tlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("exp tx", exp_ptr, rlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("master rx", t.rx_buffer, rlen, ESP_LOG_INFO);
        if (!test_mosi) {
            TEST_ASSERT_EQUAL_HEX8_ARRAY(exp_ptr+tlen, t.rx_buffer, rlen);
        }
    }

    master_free_device_bus(spi);
}

void test_sio_master(void)
{
    test_sio_master_round(true);
    unity_send_signal("master ready");
    test_sio_master_round(false);
}

void test_sio_slave_round(bool test_mosi)
{
    WORD_ALIGNED_ATTR uint8_t rx_buffer[320];

    if (test_mosi) {
        ESP_LOGI(SLAVE_TAG, "======== TEST MOSI ===========");
    } else {
        ESP_LOGI(SLAVE_TAG, "======== TEST MISO ===========");
    }

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.mosi_io_num = spi_periph_signal[TEST_SLAVE_HOST].spid_iomux_pin;
    bus_cfg.miso_io_num = spi_periph_signal[TEST_SLAVE_HOST].spiq_iomux_pin;
    bus_cfg.sclk_io_num = spi_periph_signal[TEST_SLAVE_HOST].spiclk_iomux_pin;

    spi_slave_interface_config_t slv_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slv_cfg.spics_io_num = spi_periph_signal[TEST_SLAVE_HOST].spics0_iomux_pin;
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slv_cfg, 0));

    for (int i = 0; i < 8; i++) {
        int tlen = 9-i;
        int rlen = i*2+1;
        spi_slave_transaction_t t = {
            .length = (tlen+rlen)*8,
            .tx_buffer = spitest_slave_send+i,
            .rx_buffer = rx_buffer,
        };

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &t, portMAX_DELAY));

        ESP_LOG_BUFFER_HEXDUMP("slave tx", t.tx_buffer, tlen+rlen, ESP_LOG_INFO);

        //send signal_idx
        unity_send_signal("slave ready");

        uint8_t *exp_ptr = spitest_master_send+i;
        spi_slave_transaction_t* ret_t;
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_t, portMAX_DELAY));

        ESP_LOG_BUFFER_HEXDUMP("exp tx", exp_ptr, tlen+rlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("slave rx", t.rx_buffer, tlen+rlen, ESP_LOG_INFO);
        if (test_mosi) {
            TEST_ASSERT_EQUAL_HEX8_ARRAY(exp_ptr, t.rx_buffer, rlen);
        }
    }

    spi_slave_free(TEST_SLAVE_HOST);
}

void test_sio_slave(void)
{
    test_sio_slave_round(true);
    unity_wait_for_signal("master ready");
    test_sio_slave_round(false);
}

TEST_CASE_MULTIPLE_DEVICES("sio mode", "[spi][test_env=Example_SPI_Multi_device]", test_sio_master, test_sio_slave);
#endif //#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32S2, ESP32C3, ESP32C2, ESP32H2)
