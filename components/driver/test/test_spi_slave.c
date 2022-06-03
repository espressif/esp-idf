/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi_slave device driver
*/

#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "test/test_common_spi.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_rom_gpio.h"


#if (TEST_SPI_PERIPH_NUM >= 2)
//These will only be enabled on chips with 2 or more SPI peripherals

#ifndef CONFIG_SPIRAM
//This test should be removed once the timing test is merged.

static spi_device_handle_t spi;
static WORD_ALIGNED_ATTR uint8_t master_txbuf[320];
static WORD_ALIGNED_ATTR uint8_t master_rxbuf[320];
static WORD_ALIGNED_ATTR uint8_t slave_txbuf[320];
static WORD_ALIGNED_ATTR uint8_t slave_rxbuf[320];

static const uint8_t master_send[] = { 0x93, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0xaa, 0xcc, 0xff, 0xee, 0x55, 0x77, 0x88, 0x43 };
static const uint8_t slave_send[] = { 0xaa, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10, 0x13, 0x57, 0x9b, 0xdf, 0x24, 0x68, 0xac, 0xe0 };

static inline void int_connect( uint32_t gpio, uint32_t sigo, uint32_t sigi )
{
    esp_rom_gpio_connect_out_signal( gpio, sigo, false, false );
    esp_rom_gpio_connect_in_signal( gpio, sigi, false );
}

static void master_init( spi_device_handle_t* spi)
{
    esp_err_t ret;
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=UNCONNECTED_PIN,
        .quadhd_io_num=-1
    };
    spi_device_interface_config_t devcfg={
        .clock_speed_hz=4*1000*1000,            //currently only up to 4MHz for internel connect
        .mode=0,                                //SPI mode 0
        .spics_io_num=PIN_NUM_CS,               //CS pin
        .queue_size=7,                          //We want to be able to queue 7 transactions at a time
        .pre_cb=NULL,
        .cs_ena_posttrans=5,
        .cs_ena_pretrans=1,
    };
    //Initialize the SPI bus
    ret=spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    TEST_ASSERT(ret==ESP_OK);
    //Attach the LCD to the SPI bus
    ret=spi_bus_add_device(TEST_SPI_HOST, &devcfg, spi);
    TEST_ASSERT(ret==ESP_OK);
}

static void slave_init(void)
{
    //Configuration for the SPI bus
    spi_bus_config_t buscfg={
        .mosi_io_num=PIN_NUM_MOSI,
        .miso_io_num=PIN_NUM_MISO,
        .sclk_io_num=PIN_NUM_CLK
    };
    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg={
        .mode=0,
        .spics_io_num=PIN_NUM_CS,
        .queue_size=3,
        .flags=0,
    };
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);
    //Initialize SPI slave interface
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
}

static void custom_setup(void) {
    //Initialize buffers
    memset(master_txbuf, 0, sizeof(master_txbuf));
    memset(master_rxbuf, 0, sizeof(master_rxbuf));
    memset(slave_txbuf, 0, sizeof(slave_txbuf));
    memset(slave_rxbuf, 0, sizeof(slave_rxbuf));

    //Initialize SPI Master
    master_init( &spi );
    //Initialize SPI Slave
    slave_init();

    //Do internal connections
    int_connect( PIN_NUM_MOSI,  spi_periph_signal[TEST_SPI_HOST].spid_out,      spi_periph_signal[TEST_SLAVE_HOST].spiq_in );
    int_connect( PIN_NUM_MISO,  spi_periph_signal[TEST_SLAVE_HOST].spiq_out,    spi_periph_signal[TEST_SPI_HOST].spid_in );
    int_connect( PIN_NUM_CS,    spi_periph_signal[TEST_SPI_HOST].spics_out[0],  spi_periph_signal[TEST_SLAVE_HOST].spics_in );
    int_connect( PIN_NUM_CLK,   spi_periph_signal[TEST_SPI_HOST].spiclk_out,    spi_periph_signal[TEST_SLAVE_HOST].spiclk_in );
}

static void custom_teardown(void) {
    TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);
    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}

TEST_CASE("test fullduplex slave with only RX direction","[spi]")
{
    custom_setup();

    memcpy(master_txbuf, master_send, sizeof(master_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t* out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length=8*32;
        slave_t.tx_buffer=NULL;
        slave_t.rx_buffer=slave_rxbuf;

        // Colorize RX buffer with known pattern
        memset( slave_rxbuf, 0x66, sizeof(slave_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32*(i+1);
        if ( t.length != 0 ) {
            t.tx_buffer = master_txbuf;
            t.rx_buffer = NULL;
        }
        spi_device_transmit( spi, (spi_transaction_t*)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master tx", t.tx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "slave rx", slave_t.rx_buffer, (slave_t.trans_len+7)/8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( t.tx_buffer, slave_t.rx_buffer, t.length/8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

TEST_CASE("test fullduplex slave with only TX direction","[spi]")
{
    custom_setup();

    memcpy(slave_txbuf, slave_send, sizeof(slave_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t* out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length=8*32;
        slave_t.tx_buffer=slave_txbuf;
        slave_t.rx_buffer=NULL;

        // Colorize RX buffer with known pattern
        memset( master_rxbuf, 0x66, sizeof(master_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32*(i+1);
        if ( t.length != 0 ) {
            t.tx_buffer = NULL;
            t.rx_buffer = master_rxbuf;
        }
        spi_device_transmit( spi, (spi_transaction_t*)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master rx", t.rx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "slave tx", slave_t.tx_buffer, (slave_t.trans_len+7)/8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( slave_t.tx_buffer, t.rx_buffer, t.length/8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

TEST_CASE("test slave send unaligned","[spi]")
{
    custom_setup();

    memcpy(master_txbuf, master_send, sizeof(master_send));
    memcpy(slave_txbuf, slave_send, sizeof(slave_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t* out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length=8*32;
        slave_t.tx_buffer=slave_txbuf+i;
        slave_t.rx_buffer=slave_rxbuf;

        // Colorize RX buffers with known pattern
        memset( master_rxbuf, 0x66, sizeof(master_rxbuf));
        memset( slave_rxbuf, 0x66, sizeof(slave_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32*(i+1);
        if ( t.length != 0 ) {
            t.tx_buffer = master_txbuf+i;
            t.rx_buffer = master_rxbuf+i;
        }
        spi_device_transmit( spi, (spi_transaction_t*)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master tx", t.tx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "master rx", t.rx_buffer, t.length/8 );
        ESP_LOG_BUFFER_HEX( "slave tx", slave_t.tx_buffer, (slave_t.trans_len+7)/8);
        ESP_LOG_BUFFER_HEX( "slave rx", slave_t.rx_buffer, (slave_t.trans_len+7)/8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( t.tx_buffer, slave_t.rx_buffer, t.length/8 );
        TEST_ASSERT_EQUAL_HEX8_ARRAY( slave_t.tx_buffer, t.rx_buffer, t.length/8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

#endif // !CONFIG_SPIRAM

#endif // #if (TEST_SPI_PERIPH_NUM >= 2)

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2)
#if (TEST_SPI_PERIPH_NUM == 1)
//These tests are for chips which only have 1 SPI controller
/********************************************************************************
 *      Test By Master & Slave (2 boards)
 *
 * Master (C3, C2, H2) && Slave (C3, C2, H2):
 *      PIN | Master     | Slave      |
 *      ----| ---------  | ---------  |
 *      CS  | 10         | 10         |
 *      CLK | 6          | 6          |
 *      MOSI| 7          | 7          |
 *      MISO| 2          | 2          |
 *      GND | GND        | GND        |
 *
 ********************************************************************************/
#define BUF_SIZE 320

static void unaligned_test_master(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, 0));

    spi_device_handle_t spi;
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    devcfg.clock_speed_hz = 4 * 1000 * 1000;
    devcfg.queue_size = 7;
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &spi));

    unity_send_signal("Master ready");

    uint8_t *master_send_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    uint8_t *master_recv_buf = heap_caps_calloc(BUF_SIZE, 1, MALLOC_CAP_DMA);
    //This buffer is used for 2-board test and should be assigned totally the same as the ``test_slave_loop`` does.
    uint8_t *slave_send_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    srand(199);
    for (int i = 0; i < BUF_SIZE; i++) {
        master_send_buf[i] = rand();
    }
    srand(299);
    for (int i = 0; i < BUF_SIZE; i++) {
        slave_send_buf[i] = rand();
    }

    for (int i = 0; i < 4; i++) {
        uint32_t length_in_bytes = 4 * (i + 1);
        spi_transaction_t t = {
            .tx_buffer = master_send_buf + i,
            .rx_buffer = master_recv_buf,
            .length = length_in_bytes * 8,
        };

        vTaskDelay(50);
        unity_wait_for_signal("Slave ready");
        TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t*)&t));

        //show result
        ESP_LOG_BUFFER_HEX("master tx:", master_send_buf+i, length_in_bytes);
        ESP_LOG_BUFFER_HEX("master rx:", master_recv_buf, length_in_bytes);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_send_buf+i, master_recv_buf, length_in_bytes);

        //clean
        memset(master_recv_buf, 0x00, BUF_SIZE);
    }

    free(master_send_buf);
    free(master_recv_buf);
    free(slave_send_buf);
    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}

static void unaligned_test_slave(void)
{
    unity_wait_for_signal("Master ready");
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));

    uint8_t *slave_send_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    uint8_t *slave_recv_buf = heap_caps_calloc(BUF_SIZE, 1, MALLOC_CAP_DMA);
    //This buffer is used for 2-board test and should be assigned totally the same as the ``test_slave_loop`` does.
    uint8_t *master_send_buf = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    srand(199);
    for (int i = 0; i < BUF_SIZE; i++) {
        master_send_buf[i] = rand();
    }
    srand(299);
    for (int i = 0; i < BUF_SIZE; i++) {
        slave_send_buf[i] = rand();
    }

    for (int i = 0; i < 4; i++) {
        uint32_t mst_length_in_bytes = 4 * (i + 1);
        spi_slave_transaction_t slave_t = {
            .tx_buffer = slave_send_buf + i,
            .rx_buffer = slave_recv_buf,
            .length = 32 * 8,
        };

        unity_send_signal("Slave ready");
        TEST_ESP_OK(spi_slave_transmit(TEST_SPI_HOST, &slave_t, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX("slave tx:", slave_send_buf + i, mst_length_in_bytes);
        ESP_LOG_BUFFER_HEX("slave rx:", slave_recv_buf, mst_length_in_bytes);

        TEST_ASSERT_EQUAL(mst_length_in_bytes * 8, slave_t.trans_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(master_send_buf + i, slave_recv_buf, mst_length_in_bytes);

        //clean
        memset(slave_recv_buf, 0x00, BUF_SIZE);
    }

    free(slave_send_buf);
    free(slave_recv_buf);
    free(master_send_buf);
    TEST_ASSERT(spi_slave_free(TEST_SPI_HOST) == ESP_OK);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Slave_Unaligned_Test", "[spi_ms][test_env=Example_SPI_Multi_device][timeout=120]", unaligned_test_master, unaligned_test_slave);

#endif  //#if (TEST_SPI_PERIPH_NUM == 1)
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(...)
