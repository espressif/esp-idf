/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
 Tests for the spi_slave device driver
*/

#include <string.h>
#include "sdkconfig.h"
#include "unity.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "hal/spi_slave_hal.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"
#include "esp_private/cache_utils.h"
#include "esp_private/spi_slave_internal.h"
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

static void master_init( spi_device_handle_t *spi)
{
    esp_err_t ret;
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = UNCONNECTED_PIN,
        .quadhd_io_num = -1
    };
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4 * 1000 * 1000,      //currently only up to 4MHz for internel connect
        .mode = 0,                              //SPI mode 0
        .spics_io_num = PIN_NUM_CS,             //CS pin
        .queue_size = 7,                        //We want to be able to queue 7 transactions at a time
        .pre_cb = NULL,
        .cs_ena_posttrans = 5,
        .cs_ena_pretrans = 1,
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    TEST_ASSERT(ret == ESP_OK);
    //Attach the LCD to the SPI bus
    ret = spi_bus_add_device(TEST_SPI_HOST, &devcfg, spi);
    TEST_ASSERT(ret == ESP_OK);
}

static void slave_init(void)
{
    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK
    };
    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = PIN_NUM_CS,
        .queue_size = 3,
        .flags = 0,
    };
    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(PIN_NUM_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(PIN_NUM_CS, GPIO_PULLUP_ONLY);
    //Initialize SPI slave interface
    TEST_ESP_OK(spi_slave_initialize(TEST_SLAVE_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
}

static void custom_setup(void)
{
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

static void custom_teardown(void)
{
    TEST_ASSERT(spi_slave_free(TEST_SLAVE_HOST) == ESP_OK);
    TEST_ASSERT(spi_bus_remove_device(spi) == ESP_OK);
    TEST_ASSERT(spi_bus_free(TEST_SPI_HOST) == ESP_OK);
}

TEST_CASE("test fullduplex slave with only RX direction", "[spi]")
{
    custom_setup();

    memcpy(master_txbuf, master_send, sizeof(master_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t *out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length = 8 * 32;
        slave_t.tx_buffer = NULL;
        slave_t.rx_buffer = slave_rxbuf;

        // Colorize RX buffer with known pattern
        memset( slave_rxbuf, 0x66, sizeof(slave_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32 * (i + 1);
        if ( t.length != 0 ) {
            t.tx_buffer = master_txbuf;
            t.rx_buffer = NULL;
        }
        spi_device_transmit( spi, (spi_transaction_t *)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master tx", t.tx_buffer, t.length / 8 );
        ESP_LOG_BUFFER_HEX( "slave rx", slave_t.rx_buffer, (slave_t.trans_len + 7) / 8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( t.tx_buffer, slave_t.rx_buffer, t.length / 8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

TEST_CASE("test fullduplex slave with only TX direction", "[spi]")
{
    custom_setup();

    memcpy(slave_txbuf, slave_send, sizeof(slave_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t *out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length = 8 * 32;
        slave_t.tx_buffer = slave_txbuf;
        slave_t.rx_buffer = NULL;

        // Colorize RX buffer with known pattern
        memset( master_rxbuf, 0x66, sizeof(master_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32 * (i + 1);
        if ( t.length != 0 ) {
            t.tx_buffer = NULL;
            t.rx_buffer = master_rxbuf;
        }
        spi_device_transmit( spi, (spi_transaction_t *)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master rx", t.rx_buffer, t.length / 8 );
        ESP_LOG_BUFFER_HEX( "slave tx", slave_t.tx_buffer, (slave_t.trans_len + 7) / 8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( slave_t.tx_buffer, t.rx_buffer, t.length / 8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

TEST_CASE("test slave send unaligned", "[spi]")
{
    custom_setup();

    memcpy(master_txbuf, master_send, sizeof(master_send));
    memcpy(slave_txbuf, slave_send, sizeof(slave_send));

    for ( int i = 0; i < 4; i ++ ) {
        //slave send
        spi_slave_transaction_t slave_t;
        spi_slave_transaction_t *out;
        memset(&slave_t, 0, sizeof(spi_slave_transaction_t));
        slave_t.length = 8 * 32;
        slave_t.tx_buffer = slave_txbuf + i;
        slave_t.rx_buffer = slave_rxbuf;

        // Colorize RX buffers with known pattern
        memset( master_rxbuf, 0x66, sizeof(master_rxbuf));
        memset( slave_rxbuf, 0x66, sizeof(slave_rxbuf));

        TEST_ESP_OK(spi_slave_queue_trans(TEST_SLAVE_HOST, &slave_t, portMAX_DELAY));

        //send
        spi_transaction_t t = {};
        t.length = 32 * (i + 1);
        if ( t.length != 0 ) {
            t.tx_buffer = master_txbuf + i;
            t.rx_buffer = master_rxbuf + i;
        }
        spi_device_transmit( spi, (spi_transaction_t *)&t );

        //wait for end
        TEST_ESP_OK(spi_slave_get_trans_result(TEST_SLAVE_HOST, &out, portMAX_DELAY));

        //show result
        ESP_LOGI(SLAVE_TAG, "trans_len: %d", slave_t.trans_len);
        ESP_LOG_BUFFER_HEX( "master tx", t.tx_buffer, t.length / 8 );
        ESP_LOG_BUFFER_HEX( "master rx", t.rx_buffer, t.length / 8 );
        ESP_LOG_BUFFER_HEX( "slave tx", slave_t.tx_buffer, (slave_t.trans_len + 7) / 8);
        ESP_LOG_BUFFER_HEX( "slave rx", slave_t.rx_buffer, (slave_t.trans_len + 7) / 8);

        TEST_ASSERT_EQUAL_HEX8_ARRAY( t.tx_buffer, slave_t.rx_buffer, t.length / 8 );
        TEST_ASSERT_EQUAL_HEX8_ARRAY( slave_t.tx_buffer, t.rx_buffer, t.length / 8 );
        TEST_ASSERT_EQUAL( t.length, slave_t.trans_len );
    }

    custom_teardown();

    ESP_LOGI(SLAVE_TAG, "test passed.");
}

#endif // !CONFIG_SPIRAM

#endif // #if (TEST_SPI_PERIPH_NUM >= 2)

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
        TEST_ESP_OK(spi_device_transmit(spi, (spi_transaction_t *)&t));

        //show result
        ESP_LOG_BUFFER_HEX("master tx:", master_send_buf + i, length_in_bytes);
        ESP_LOG_BUFFER_HEX("master rx:", master_recv_buf, length_in_bytes);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_send_buf + i, master_recv_buf, length_in_bytes);

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

TEST_CASE_MULTIPLE_DEVICES("SPI_Slave_Unaligned_Test", "[spi_ms][timeout=120]", unaligned_test_master, unaligned_test_slave);
#endif  //#if (TEST_SPI_PERIPH_NUM == 1)

#if CONFIG_SPI_SLAVE_ISR_IN_IRAM
#define TEST_IRAM_TRANS_NUM     8
#define TEST_TRANS_LEN          120
#define TEST_BUFFER_SZ          (TEST_IRAM_TRANS_NUM*TEST_TRANS_LEN)

static void test_slave_iram_master_normal(void)
{
    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_initialize(TEST_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_handle_t dev_handle = {0};
    spi_device_interface_config_t devcfg = SPI_DEVICE_TEST_DEFAULT_CONFIG();
    TEST_ESP_OK(spi_bus_add_device(TEST_SPI_HOST, &devcfg, &dev_handle));

    uint8_t *master_send = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DMA);
    uint8_t *master_recv = heap_caps_calloc(1, TEST_BUFFER_SZ, MALLOC_CAP_DMA);
    uint8_t *master_exp  = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DEFAULT);
    get_tx_buffer(1001, master_send, master_exp, TEST_BUFFER_SZ);
    spi_transaction_t trans_cfg = {
        .tx_buffer = master_send,
        .rx_buffer = master_recv,
        .user = master_exp,
        .length = TEST_TRANS_LEN * 8,
    };

    //first trans to trigger slave enter isr
    unity_send_signal("Master ready");
    unity_wait_for_signal("Slave ready");
    TEST_ESP_OK(spi_device_transmit(dev_handle, &trans_cfg));

    for (uint8_t cnt = 0; cnt < TEST_IRAM_TRANS_NUM; cnt ++) {
        trans_cfg.tx_buffer = master_send + TEST_TRANS_LEN * cnt;
        trans_cfg.rx_buffer = master_recv + TEST_TRANS_LEN * cnt;
        trans_cfg.user = master_exp + TEST_TRANS_LEN * cnt;
        unity_wait_for_signal("Slave ready");
        TEST_ESP_OK(spi_device_transmit(dev_handle, &trans_cfg));

        ESP_LOG_BUFFER_HEX("master tx", trans_cfg.tx_buffer, TEST_TRANS_LEN);
        ESP_LOG_BUFFER_HEX_LEVEL("master rx", trans_cfg.rx_buffer, TEST_TRANS_LEN, ESP_LOG_DEBUG);
        ESP_LOG_BUFFER_HEX_LEVEL("master exp", trans_cfg.user, TEST_TRANS_LEN, ESP_LOG_DEBUG);
        spitest_cmp_or_dump(trans_cfg.user, trans_cfg.rx_buffer, TEST_TRANS_LEN);
    }

    free(master_send);
    free(master_recv);
    free(master_exp);
    spi_bus_remove_device(dev_handle);
    spi_bus_free(TEST_SPI_HOST);
}

//------------------------------------test slave func-----------------------------------------
static IRAM_ATTR void ESP_LOG_BUFFER_HEX_ISR(const char *tag, const uint8_t *buff, const uint32_t byte_len)
{
    esp_rom_printf(DRAM_STR("%s: "), tag);
    for (uint16_t i = 0; i < byte_len; i++) {
        if (0 == i % 16) {
            esp_rom_printf(DRAM_STR("\n"));
        }
        esp_rom_printf(DRAM_STR("%02x "), buff[i]);
    }
    esp_rom_printf(DRAM_STR("\n"));
}

static uint32_t isr_iram_cnt;
static uint32_t iram_test_fail;
static IRAM_ATTR void test_slave_iram_post_trans_cbk(spi_slave_transaction_t *curr_trans)
{
    isr_iram_cnt ++;

    // first trans is the trigger trans with random data by master
    if (isr_iram_cnt > 1) {
        ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave tx"), curr_trans->tx_buffer, curr_trans->trans_len / 8);
        if (memcmp(curr_trans->rx_buffer, curr_trans->user, curr_trans->trans_len / 8)) {
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave rx"), curr_trans->rx_buffer, curr_trans->trans_len / 8);
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave exp"), curr_trans->user, curr_trans->trans_len / 8);
            iram_test_fail = true;
        }
    }
    if (isr_iram_cnt <= TEST_IRAM_TRANS_NUM) {
        // str "Send signal: [Slave ready]!\n" used for CI to run test automatically
        // here use `esp_rom_printf` instead `unity_send_signal` because cache is disabled by test
        esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
    }
}

static IRAM_ATTR void test_slave_isr_iram(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.intr_flags |= ESP_INTR_FLAG_IRAM;
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.flags = SPI_SLAVE_NO_RETURN_RESULT;
    slvcfg.queue_size = 16;
    slvcfg.post_trans_cb = test_slave_iram_post_trans_cbk;
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &bus_cfg, &slvcfg, SPI_DMA_CH_AUTO));

    uint8_t *slave_iram_send = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_iram_recv = heap_caps_calloc(1, TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_iram_exp  = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    get_tx_buffer(1001, slave_iram_exp, slave_iram_send, TEST_BUFFER_SZ);
    spi_slave_transaction_t trans_cfg[TEST_IRAM_TRANS_NUM] = {0};

    unity_wait_for_signal("Master ready");
    trans_cfg[0].tx_buffer = slave_iram_send;
    trans_cfg[0].rx_buffer = slave_iram_recv;
    trans_cfg[0].user = slave_iram_exp;
    trans_cfg[0].length = TEST_TRANS_LEN * 8;
    spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg[0], portMAX_DELAY);

    // mount several transaction first
    for (uint8_t i = 0; i < TEST_IRAM_TRANS_NUM; i++) {
        trans_cfg[i].tx_buffer = slave_iram_send + TEST_TRANS_LEN * i;
        trans_cfg[i].rx_buffer = slave_iram_recv + TEST_TRANS_LEN * i;
        trans_cfg[i].user = slave_iram_exp + TEST_TRANS_LEN * i;
        trans_cfg[i].length = TEST_TRANS_LEN * 8;
        spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg[i], portMAX_DELAY);
    }

    // disable cache then send signal `ready` to start transaction
    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
    while (isr_iram_cnt <= TEST_IRAM_TRANS_NUM) {
        esp_rom_delay_us(10);
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    if (iram_test_fail) {
        TEST_FAIL();
    }

    free(slave_iram_send);
    free(slave_iram_recv);
    free(slave_iram_exp);
    spi_slave_free(TEST_SPI_HOST);
}

TEST_CASE_MULTIPLE_DEVICES("SPI_Slave: Test_ISR_IRAM_disable_cache", "[spi_ms]", test_slave_iram_master_normal, test_slave_isr_iram);

static uint32_t isr_trans_cnt, isr_trans_test_fail;
static IRAM_ATTR void test_trans_in_isr_post_trans_cbk(spi_slave_transaction_t *curr_trans)
{
    isr_trans_cnt ++;

    //first trans is the trigger trans with random data
    if (isr_trans_cnt > 1) {
        ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave tx"), curr_trans->tx_buffer, curr_trans->trans_len / 8);

        if (memcmp(curr_trans->rx_buffer, curr_trans->user, curr_trans->trans_len / 8)) {
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave rx"), curr_trans->rx_buffer, curr_trans->trans_len / 8);
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave exp"), curr_trans->user, curr_trans->trans_len / 8);
            isr_trans_test_fail = true;
        }

        curr_trans->tx_buffer = (uint8_t *)curr_trans->tx_buffer + TEST_TRANS_LEN;
        curr_trans->rx_buffer = (uint8_t *)curr_trans->rx_buffer + TEST_TRANS_LEN;
        curr_trans->user = (uint8_t *)curr_trans->user + TEST_TRANS_LEN;
    }

    if (isr_trans_cnt <= TEST_IRAM_TRANS_NUM) {
        if (ESP_OK == spi_slave_queue_trans_isr(TEST_SPI_HOST, curr_trans)) {
            // use `esp_rom_printf` instead `unity_send_signal` because cache is disabled by test
            esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
        } else {
            esp_rom_printf(DRAM_STR("SPI Add trans in isr fail, Queue full\n"));
        }
    }
}

static IRAM_ATTR void spi_slave_trans_in_isr(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.intr_flags |= ESP_INTR_FLAG_IRAM;
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.flags = SPI_SLAVE_NO_RETURN_RESULT;
    slvcfg.queue_size = 16;
    slvcfg.post_trans_cb = test_trans_in_isr_post_trans_cbk;
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &bus_cfg, &slvcfg, SPI_DMA_CH_AUTO));

    uint8_t *slave_isr_send = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_isr_recv = heap_caps_calloc(1, TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_isr_exp  = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    get_tx_buffer(1001, slave_isr_exp, slave_isr_send, TEST_BUFFER_SZ);
    spi_slave_transaction_t trans_cfg = {
        .tx_buffer = slave_isr_send,
        .rx_buffer = slave_isr_recv,
        .user = slave_isr_exp,
        .length = TEST_TRANS_LEN * 8,
    };

    unity_wait_for_signal("Master ready");
    //start a trans by normal API first to trigger spi isr
    spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg, portMAX_DELAY);
    spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
    while (isr_trans_cnt <= TEST_IRAM_TRANS_NUM) {
        esp_rom_delay_us(10);
    }
    spi_flash_enable_interrupts_caches_and_other_cpu();
    if (isr_trans_test_fail) {
        TEST_FAIL();
    }

    free(slave_isr_send);
    free(slave_isr_recv);
    free(slave_isr_exp);
    spi_slave_free(TEST_SPI_HOST);
}
TEST_CASE_MULTIPLE_DEVICES("SPI_Slave: Test_Queue_Trans_in_ISR", "[spi_ms]", test_slave_iram_master_normal, spi_slave_trans_in_isr);

uint32_t dummy_data[2] = {0x38383838, 0x5b5b5b5b};
spi_slave_transaction_t dummy_trans[2];
static uint32_t queue_reset_isr_trans_cnt, test_queue_reset_isr_fail;
static IRAM_ATTR void test_queue_reset_in_isr_post_trans_cbk(spi_slave_transaction_t *curr_trans)
{
    queue_reset_isr_trans_cnt ++;

    //first trans is the trigger trans with random data
    if (queue_reset_isr_trans_cnt > 1) {
        ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave tx"), curr_trans->tx_buffer, curr_trans->length / 8);

        if (memcmp(curr_trans->rx_buffer, curr_trans->user, curr_trans->length / 8)) {
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave rx"), curr_trans->rx_buffer, curr_trans->length / 8);
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("slave exp"), curr_trans->user, curr_trans->length / 8);
            test_queue_reset_isr_fail = true;
        }

        if (queue_reset_isr_trans_cnt > 4) {
            // add some confusing transactions
            dummy_data[0] ++;
            dummy_data[1] --;
            spi_slave_queue_trans_isr(TEST_SPI_HOST, &dummy_trans[0]);
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("Queue Hacked hahhhhh..."), dummy_trans[0].tx_buffer, dummy_trans[0].length / 8);
            spi_slave_queue_trans_isr(TEST_SPI_HOST, &dummy_trans[1]);
            ESP_LOG_BUFFER_HEX_ISR(DRAM_STR("Queue Hacked hahhhhh..."), dummy_trans[1].tx_buffer, dummy_trans[1].length / 8);
            if (ESP_OK == spi_slave_queue_reset_isr(TEST_SPI_HOST)) {
                esp_rom_printf(DRAM_STR("Queue reset done, continue\n"));
            }
        }

        curr_trans->tx_buffer = (uint8_t *)curr_trans->tx_buffer + TEST_TRANS_LEN;
        curr_trans->rx_buffer = (uint8_t *)curr_trans->rx_buffer + TEST_TRANS_LEN;
        curr_trans->user = (uint8_t *)curr_trans->user + TEST_TRANS_LEN;
    }

    if (queue_reset_isr_trans_cnt <= TEST_IRAM_TRANS_NUM) {
        if (ESP_OK == spi_slave_queue_trans_isr(TEST_SPI_HOST, curr_trans)) {
            // use `esp_rom_printf` instead `unity_send_signal` because cache is disabled by test
            esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
        } else {
            esp_rom_printf(DRAM_STR("SPI Add trans in isr fail, Queue full\n"));
        }
    }
}

static IRAM_ATTR void spi_queue_reset_in_isr(void)
{
    spi_bus_config_t bus_cfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    bus_cfg.intr_flags |= ESP_INTR_FLAG_IRAM;
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.flags = SPI_SLAVE_NO_RETURN_RESULT;
    slvcfg.queue_size = 16;
    slvcfg.post_trans_cb = test_queue_reset_in_isr_post_trans_cbk;
    TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &bus_cfg, &slvcfg, SPI_DMA_CH_AUTO));

    uint8_t *slave_isr_send = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_isr_recv = heap_caps_calloc(1, TEST_BUFFER_SZ, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    uint8_t *slave_isr_exp  = heap_caps_malloc(TEST_BUFFER_SZ, MALLOC_CAP_DEFAULT | MALLOC_CAP_INTERNAL);
    get_tx_buffer(1001, slave_isr_exp, slave_isr_send, TEST_BUFFER_SZ);
    spi_slave_transaction_t trans_cfg = {
        .tx_buffer = slave_isr_send,
        .rx_buffer = slave_isr_recv,
        .user = slave_isr_exp,
        .length = TEST_TRANS_LEN * 8,
    };

    unity_wait_for_signal("Master ready");
    for (uint8_t i = 0; i < 2; i++) {
        dummy_trans[i].tx_buffer = &dummy_data[i];
        dummy_trans[i].rx_buffer = &dummy_data[i];
        dummy_trans[i].user = &dummy_data[i];
        dummy_trans[i].length = sizeof(uint32_t) * 8;
    }
    // start a trans by normal API first to trigger spi isr
    spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg, portMAX_DELAY);
    // spi_flash_disable_interrupts_caches_and_other_cpu();
    esp_rom_printf(DRAM_STR("Send signal: [Slave ready]!\n"));
    while (queue_reset_isr_trans_cnt <= TEST_IRAM_TRANS_NUM) {
        esp_rom_delay_us(10);
    }
    // spi_flash_enable_interrupts_caches_and_other_cpu();
    if (test_queue_reset_isr_fail) {
        TEST_FAIL();
    }

    free(slave_isr_send);
    free(slave_isr_recv);
    free(slave_isr_exp);
    spi_slave_free(TEST_SPI_HOST);
}
TEST_CASE_MULTIPLE_DEVICES("SPI_Slave: Test_Queue_Reset_in_ISR", "[spi_ms]", test_slave_iram_master_normal, spi_queue_reset_in_isr);
#endif // CONFIG_SPI_SLAVE_ISR_IN_IRAM

#if (SOC_CPU_CORES_NUM > 1) && (!CONFIG_FREERTOS_UNICORE)

#define TEST_ISR_CNT    100
static void test_slave_isr_core_setup_cbk(spi_slave_transaction_t *curr_trans)
{
    *((int *)curr_trans->user) += esp_cpu_get_core_id();
}

TEST_CASE("test_slave_isr_pin_to_core", "[spi]")
{
    uint32_t slave_send;
    uint32_t slave_recive;
    uint32_t slave_expect;

    spi_bus_config_t buscfg = SPI_BUS_TEST_DEFAULT_CONFIG();
    spi_slave_interface_config_t slvcfg = SPI_SLAVE_TEST_DEFAULT_CONFIG();
    slvcfg.post_setup_cb = test_slave_isr_core_setup_cbk;

    spi_slave_transaction_t trans_cfg = {
        .tx_buffer = &slave_send,
        .rx_buffer = &slave_recive,
        .user = &slave_expect,
        .length = sizeof(uint32_t) * 8,
    };

    slave_expect = 0;
    for (int i = 0; i < TEST_ISR_CNT; i++) {
        TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg, portMAX_DELAY));
        TEST_ESP_OK(spi_slave_free(TEST_SPI_HOST));
    }
    printf("Test Slave ISR Not Assign: %d : %ld\n", TEST_ISR_CNT, slave_expect);
    // by default the esp_intr_alloc is called on ESP_MAIN_TASK_AFFINITY_CPU0 now
    TEST_ASSERT_EQUAL_UINT32(0, slave_expect);

    //-------------------------------------CPU1---------------------------------------
    buscfg.isr_cpu_id = ESP_INTR_CPU_AFFINITY_1;

    slave_expect = 0;
    for (int i = 0; i < TEST_ISR_CNT; i++) {
        TEST_ESP_OK(spi_slave_initialize(TEST_SPI_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO));
        TEST_ESP_OK(spi_slave_queue_trans(TEST_SPI_HOST, &trans_cfg, portMAX_DELAY));
        vTaskDelay(1);  // Waiting ISR on core 1 to be done.
        TEST_ESP_OK(spi_slave_free(TEST_SPI_HOST));
    }
    printf("Test Slave ISR Assign CPU1: %d : %ld\n", TEST_ISR_CNT, slave_expect);
    TEST_ASSERT_EQUAL_UINT32(TEST_ISR_CNT, slave_expect);
}

#endif
