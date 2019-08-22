/*
 Tests for the spi sio mode
*/

#include <esp_types.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/xtensa_api.h"
#include "unity.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "soc/spi_periph.h"
#include "test_utils.h"
#include "test/test_common_spi.h"
#include "soc/gpio_periph.h"
#include "sdkconfig.h"
#include "hal/spi_ll.h"


/********************************************************************************
 *      Test SIO
 ********************************************************************************/
TEST_CASE("local test sio", "[spi]")
{
    spi_device_handle_t spi;
    WORD_ALIGNED_ATTR uint8_t master_rx_buffer[320];
    WORD_ALIGNED_ATTR uint8_t slave_rx_buffer[320];

    uint32_t padding[16];
    for (int i = 0; i < 16; i++)
        padding[i] = 0xcccccccc;;

    spi_ll_write_buffer(SPI_LL_GET_HW(TEST_SPI_HOST), (uint8_t*)&padding, 8*64);
    spi_ll_write_buffer(SPI_LL_GET_HW(TEST_SLAVE_HOST), (uint8_t*)&padding, 8*64);

    /* This test use a strange connection to test the SIO mode:
     * master spid -> slave spid
     * slave spiq -> master spid
     */
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_device_interface_config_t dev_cfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slv_cfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slv_cfg.spics_io_num = dev_cfg.spics_io_num;
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &bus_cfg, &slv_cfg, 0));

    int miso_io_num = bus_cfg.miso_io_num;
    int mosi_io_num = bus_cfg.mosi_io_num;
    bus_cfg.mosi_io_num = bus_cfg.miso_io_num;
    bus_cfg.miso_io_num = -1;
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &bus_cfg, 0));

    dev_cfg.flags = SPI_DEVICE_HALFDUPLEX | SPI_DEVICE_3WIRE;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &dev_cfg, &spi));

    spitest_gpio_output_sel(mosi_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spid_out);
    spitest_gpio_output_sel(miso_io_num, FUNC_GPIO, spi_periph_signal[TEST_SLAVE_HOST].spiq_out);
    spitest_gpio_output_sel(dev_cfg.spics_io_num, FUNC_GPIO, spi_periph_signal[TEST_SPI_HOST].spics_out[0]);
    spitest_gpio_output_sel(bus_cfg.sclk_io_num, FUNC_GPIO,  spi_periph_signal[TEST_SPI_HOST].spiclk_out);

    for (int i = 0; i < 8; i ++) {
        int tlen = i*2+1;
        int rlen = 9-i;

        ESP_LOGI(MASTER_TAG, "=========== TEST%d ==========", i);

        spi_transaction_t master_t = {
            .length = tlen*8,
            .tx_buffer = spitest_master_send+i,
            .rxlength = rlen*8,
            .rx_buffer = master_rx_buffer+i,
        };
        spi_slave_transaction_t slave_t = {
            .length = (tlen+rlen)*8,
            .tx_buffer = spitest_slave_send+i,
            .rx_buffer = slave_rx_buffer,
        };
        memset(master_rx_buffer, 0x66, sizeof(master_rx_buffer));
        memset(slave_rx_buffer, 0x66, sizeof(slave_rx_buffer));
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        TEST_ESP_OK(spi_device_transmit(spi, &master_t));
        spi_slave_transaction_t* ret_t;
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &ret_t, portMAX_DELAY));
        TEST_ASSERT(ret_t == &slave_t);

        ESP_LOG_BUFFER_HEXDUMP("master tx", master_t.tx_buffer, tlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("slave rx", slave_t.rx_buffer, tlen+rlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("slave tx", slave_t.tx_buffer, tlen+rlen, ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP("master rx", master_t.rx_buffer, rlen, ESP_LOG_INFO);

        TEST_ASSERT_EQUAL_HEX8_ARRAY(master_t.tx_buffer, slave_t.rx_buffer, tlen);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_t.tx_buffer + tlen, master_t.rx_buffer, rlen);
    }

    spi_slave_free(TEST_SLAVE_HOST);
    master_free_device_bus(spi);
}
