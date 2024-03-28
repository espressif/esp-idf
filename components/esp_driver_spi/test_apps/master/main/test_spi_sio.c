/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
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
#include "test_spi_utils.h"
#include "soc/gpio_periph.h"

#include "hal/spi_ll.h"

#if (TEST_SPI_PERIPH_NUM >= 2)
//These will be only enabled on chips with 2 or more SPI peripherals

/********************************************************************************
 *      Test SIO
 ********************************************************************************/
#if CONFIG_IDF_TARGET_ESP32
#define MASTER_DIN_SIGNAL    HSPID_IN_IDX
#elif CONFIG_IDF_TARGET_ESP32P4
#define MASTER_DIN_SIGNAL    SPI2_D_PAD_IN_IDX
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

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32P4)    //IDF-7503 slave support
/********************************************************************************
 *                             Test SIO Master
 * SIO Slave is not supported, and one unit test is limited to one feature, so,,,
 * sio master test can be split to signal-input and single-output
 *
 * for single-output:      master               slave
 *                       cs-----cs  ------------- cs
 *                       clk----clk ------------- clk
 *                       d------mosi------------- mosi
 *                       q      miso------------- miso
 * master can get input on mosi pin after output finish in sio mode, but in this
 * case, master can get no data from slave, so check assert on the slave.
 *
 * ------------------------------------------------------------------------------
 * for single-input:       master               slave
 *                       cs-----cs  ------------- cs
 *                       clk----clk ------------- clk
 *                       d-\    mosi------------- mosi
 *                       q  \\--miso------------- miso
 * In this case, master can get input data from slave after output finish, but
 * slave can get no data from master due to internal broke, besides output data
 * from both master and slave on miso line will get conflict in master's output
 * frame.
 ********************************************************************************/
#define TRANS_LEN       1024
#define MAX_TRANS_BUFF  64
#define TEST_NUM        8

WORD_ALIGNED_ATTR uint8_t sio_master_rx_buff[TRANS_LEN];
WORD_ALIGNED_ATTR uint8_t sio_slave_rx_buff [TRANS_LEN];

void test_sio_master_trans(bool sio_master_in)
{
    spi_device_handle_t dev_0;
    uint8_t *master_tx_max = heap_caps_calloc(TRANS_LEN * 2, 1, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL_MESSAGE(master_tx_max, "malloc failed, exit.\n");

    // write somethin to a long buffer for test long transmission
    for (uint16_t i = 0; i < TRANS_LEN; i++) {
        master_tx_max[i] = i;
        master_tx_max[TRANS_LEN * 2 - i - 1] = i;
    }

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    if (sio_master_in) {
        // normally, spi read data from port Q and write data to port D
        // test master input from port D (output default.), so link port D (normally named mosi) to miso pin.
        bus_cfg.mosi_io_num = bus_cfg.miso_io_num;
        printf("\n====================Test sio master input====================\n");
    } else {
        printf("\n============Test sio master output, data checked by slave.=============\n");
    }
    bus_cfg.miso_io_num = -1;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE;
    dev_cfg.clock_speed_hz = 1 * 1000 * 1000;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &dev_0));
    printf("CS:CLK:MO:MI: %d\t%d\t%d\t%d\n", dev_cfg.spics_io_num, bus_cfg.sclk_io_num, bus_cfg.mosi_io_num, bus_cfg.miso_io_num);

    unity_send_signal("Master ready");
    for (int i = 0; i < TEST_NUM; i ++) {
        spi_transaction_t trans = {};
        if (sio_master_in) {
            // master input only section
            trans.rxlength = (i + 1) * 8 * 8;
            // test a huge data for last transmission
            if (i >= TEST_NUM - 1) {
                trans.rxlength = TRANS_LEN * 8;
            }
            trans.rx_buffer = sio_master_rx_buff;
            trans.length = 0;
            trans.tx_buffer = NULL;
            memset(sio_master_rx_buff, 0, sizeof(sio_master_rx_buff));
        } else {
            // master output only section
            trans.length = MAX_TRANS_BUFF / (i + 1) * 8;
            // test a huge data for last transmission
            if (i >= TEST_NUM - 1) {
                trans.length = TRANS_LEN * 8;
            }
            trans.tx_buffer = master_tx_max;
            trans.rxlength = 0;
            trans.rx_buffer = NULL;
            // use some different data
            trans.tx_buffer += (i % 2) ? TRANS_LEN : 0;
        }

        //get signal
        unity_wait_for_signal("Slave ready");

        TEST_ESP_OK(spi_device_transmit(dev_0, &trans));
        if (sio_master_in) {
            ESP_LOG_BUFFER_HEXDUMP("master rx", trans.rx_buffer, trans.rxlength / 8, ESP_LOG_INFO);
            TEST_ASSERT_EQUAL_HEX8_ARRAY(master_tx_max + i, trans.rx_buffer, trans.rxlength / 8);
        } else {
            printf("%d master output\n", trans.length / 8);
            ESP_LOG_BUFFER_HEXDUMP("master tx", trans.tx_buffer, trans.length / 8, ESP_LOG_INFO);
        }
    }

    free(master_tx_max);
    master_free_device_bus(dev_0);
}

void test_sio_slave_emulate(bool sio_master_in)
{
    uint8_t *slave_tx_max = heap_caps_calloc(TRANS_LEN * 2, 1, MALLOC_CAP_DMA);
    TEST_ASSERT_NOT_NULL_MESSAGE(slave_tx_max, "malloc failed, exit.\n");

    // write somethin to a long buffer for test long transmission
    for (uint16_t i = 0; i < TRANS_LEN; i++) {
        slave_tx_max[i] = i;
        slave_tx_max[TRANS_LEN * 2 - i - 1] = i;
    }

    if (sio_master_in) {
        printf("\n==================Test sio master input.================\n");
    } else {
        printf("\n==================Test sio master output.=================\n");
    }

    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slv_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slv_cfg, SPI_DMA_CH_AUTO));
    printf("CS:CLK:MO:MI: %d\t%d\t%d\t%d\n", slv_cfg.spics_io_num, bus_cfg.sclk_io_num, bus_cfg.mosi_io_num, bus_cfg.miso_io_num);

    unity_wait_for_signal("Master ready");
    for (int i = 0; i < TEST_NUM; i++) {
        spi_slave_transaction_t trans = {};
        if (sio_master_in) {
            // slave output only section
            trans.length = (i + 1) * 8 * 8;
            // test a huge data for last transmission
            if (i >= TEST_NUM - 1) {
                trans.length = TRANS_LEN * 8;
            }
            trans.tx_buffer = slave_tx_max + i;
            trans.rx_buffer = NULL;
        } else {
            // slave input only section
            trans.length = MAX_TRANS_BUFF / (i + 1) * 8;
            // test a huge data for last transmission
            if (i >= TEST_NUM - 1) {
                trans.length = TRANS_LEN * 8;
            }
            trans.tx_buffer = NULL;
            trans.rx_buffer = sio_slave_rx_buff;
            memset(sio_slave_rx_buff, 0, sizeof(sio_slave_rx_buff));
        }

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &trans, portMAX_DELAY));
        unity_send_signal("Slave ready");

        spi_slave_transaction_t *p_slave_ret;
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &p_slave_ret, portMAX_DELAY));

        if (sio_master_in) {
            ESP_LOG_BUFFER_HEXDUMP("Slave tx", trans.tx_buffer, trans.length / 8, ESP_LOG_INFO);
        } else {
            ESP_LOG_BUFFER_HEXDUMP("Slave rx", trans.rx_buffer, trans.length / 8, ESP_LOG_INFO);
            TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_tx_max + TRANS_LEN * (i % 2), trans.rx_buffer, trans.length / 8);
        }
    }

    free(slave_tx_max);
    spi_slave_free(TEST_SLAVE_HOST);
}

void test_master_run(void)
{
    test_sio_master_trans(false);
    test_sio_master_trans(true);
}

void test_slave_run(void)
{
    test_sio_slave_emulate(false);
    test_sio_slave_emulate(true);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Master:Test_SIO_Mode_Multi_Board", "[spi_ms][test_env=generic_multi_device]", test_master_run, test_slave_run);
#endif  //p4 slave support
