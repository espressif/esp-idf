/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "unity.h"
#include "test_utils.h"
#include "test_spi_utils.h"
#include "esp_serial_slave_link/essl_sdio.h"
#include "esp_log.h"
#include "sdmmc_cmd.h"
#include "esp_timer.h"
#include "ccomp_timer.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"
#include "soc/soc_caps.h"


#if SOC_SDMMC_HOST_SUPPORTED && SOC_SDIO_SLAVE_SUPPORTED
#include "soc/sdio_slave_pins.h"
#include "driver/sdio_slave.h"
#include "driver/sdmmc_host.h"

#define TIMEOUT_MAX             UINT32_MAX
#define INT_MASK_ALL            0xff
#define SDIO_SLAVE_QUEUE_SIZE   20
#define RX_BUFFER_SIZE          2048
#define RX_BUFFER_NUM           10
#define TX_BUFFER_SIZE          2048
#define REG_ADDR_MAX            60
//the test should run accross the boundary, i.e. over 0x100000 bytes.
//TEST_CNT > 512
#define TEST_CNT                10000

#define TEST_SDSPI_HOST         SPI2_HOST
#define TEST_SDSPI_DMACHAN      1

#define TEST_RESET_DATA_LEN 10

#ifndef MIN
#define MIN(a, b)   ((a)<(b)? (a): (b))
#endif

typedef enum {
    SDIO_1BIT = 0,
    SDIO_4BIT = 1,
    SDIO_SPI = 2,
} sdio_mode_t;

typedef void (*sdio_test_func)(essl_handle_t handle);

typedef struct {
    const char test_name[16];
    sdio_mode_t sdio_mode;
    uint32_t freq;
    bool check_data;
    bool packet_mode;
} sdio_test_config_t;


sdio_test_config_t test_cfg_array[] = {
    //the first item will be the default config used by all tests
    {
        .test_name = "HS4B",
        .sdio_mode = SDIO_4BIT,
        .freq = SDMMC_FREQ_HIGHSPEED,
        .check_data = true,
    },
    {
        .test_name = "HS1B",
        .sdio_mode = SDIO_1BIT,
        .freq = SDMMC_FREQ_HIGHSPEED,
        .check_data = true,
    },
    {
        .test_name = "SPI",
        .sdio_mode = SDIO_SPI,
        .freq = SDMMC_FREQ_HIGHSPEED,
        .check_data = true,
    },
    //the performance test is only done when psram is not enabled
#if !CONFIG_SPIRAM && !CONFIG_FREERTOS_CHECK_PORT_CRITICAL_COMPLIANCE
    {
        .test_name = "HS4B (perf)",
        .sdio_mode = SDIO_4BIT,
        .freq = SDMMC_FREQ_HIGHSPEED,
    },
    {
        .test_name = "HS1B (perf)",
        .sdio_mode = SDIO_1BIT,
        .freq = SDMMC_FREQ_HIGHSPEED,
    },
    {
        .test_name = "SPI (perf)",
        .sdio_mode = SDIO_SPI,
        .freq = SDMMC_FREQ_HIGHSPEED,
    },
#endif
};

sdio_test_config_t packet_config = {
    .test_name = "HS4B packet",
    .sdio_mode = SDIO_4BIT,
    .freq = SDMMC_FREQ_HIGHSPEED,
    .check_data = true,
    .packet_mode = true,
};

const sdio_test_config_t* default_config = &test_cfg_array[0];

#define TEST_SIZE   (sizeof(test_cfg_array)/sizeof(sdio_test_config_t))


/*******************************************************************************
 * Master
 ******************************************************************************/

static sdmmc_card_t s_card;

typedef void (*test_func_t)(essl_handle_t handle, const sdio_test_config_t* config);

static void init_sdmmc_host(void);
static void init_essl(essl_handle_t *out_handle, const sdio_test_config_t *conf);
static void deinit_essl(essl_handle_t handle, const sdio_test_config_t *conf);

static void test_framework_master(test_func_t test_func, const sdio_test_config_t* config)
{
    unity_send_signal("Master ready");
    unity_wait_for_signal("Slave ready");
    ESP_LOGI(MASTER_TAG, "### Testing %s... ####", config->test_name);
    essl_handle_t handle;
    esp_err_t err;
    init_essl(&handle, config);

    err = essl_init(handle, TIMEOUT_MAX);
    TEST_ESP_OK(err);

    (*test_func)(handle, config);

    deinit_essl(handle, config);
}

static void init_sdmmc_host(void)
{
    esp_err_t err;
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    err = sdmmc_host_init();
    TEST_ESP_OK(err);

    err = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    TEST_ESP_OK(err);
}

static void init_essl(essl_handle_t *out_handle, const sdio_test_config_t *conf)
{
    sdmmc_host_t config;
    esp_err_t err;
    spi_bus_config_t bus_config;
    /* Probe */

    switch (conf->sdio_mode) {
    case SDIO_4BIT:
        ESP_LOGI(MASTER_TAG, "Probe using SD 4-bit...");
        config = (sdmmc_host_t)SDMMC_HOST_DEFAULT();
        config.flags = SDMMC_HOST_FLAG_4BIT;
        config.max_freq_khz = conf->freq;
        init_sdmmc_host();
        break;
    case SDIO_1BIT:
        ESP_LOGI(MASTER_TAG, "Probe using SD 1-bit...");
        config = (sdmmc_host_t)SDMMC_HOST_DEFAULT();
        config.flags = SDMMC_HOST_FLAG_1BIT;
        config.max_freq_khz = conf->freq;
        init_sdmmc_host();
        break;
    case SDIO_SPI:
        bus_config = (spi_bus_config_t) {
            .mosi_io_num = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CMD,
            .miso_io_num = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D0,
            .sclk_io_num = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_CLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };
        err = spi_bus_initialize(TEST_SDSPI_HOST, &bus_config, TEST_SDSPI_DMACHAN);
        TEST_ESP_OK(err);

        sdspi_device_config_t device_config = SDSPI_DEVICE_CONFIG_DEFAULT();
        device_config.host_id = TEST_SDSPI_HOST;
        device_config.gpio_cs = SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D3;
        device_config.gpio_int= SDIO_SLAVE_SLOT1_IOMUX_PIN_NUM_D1;

        err = gpio_install_isr_service(0);
        TEST_ASSERT(err == ESP_OK || err == ESP_ERR_INVALID_STATE);

        sdspi_dev_handle_t sdspi_handle;
        err = sdspi_host_init();
        TEST_ESP_OK(err);
        err = sdspi_host_init_device(&device_config, &sdspi_handle);
        TEST_ESP_OK(err);

        ESP_LOGI(MASTER_TAG, "Probe using SPI...");

        config = (sdmmc_host_t)SDSPI_HOST_DEFAULT();
        config.slot = sdspi_handle;
        break;
    }

    sdmmc_card_t *card = &s_card;
    //wait for at least 5 seconds
    int retry_times = 5;
    do {
        if (sdmmc_card_init(&config, card) == ESP_OK) {
            break;
        }
        ESP_LOGW(MASTER_TAG, "slave init failed, retry...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while (--retry_times);
    TEST_ASSERT_MESSAGE(retry_times != 0, "Initializing slave failed.");
    essl_sdio_config_t ser_config = {
        .card = card,
        .recv_buffer_size = RX_BUFFER_SIZE,
    };
    err = essl_sdio_init_dev(out_handle, &ser_config);
    TEST_ESP_OK(err);

    err = essl_init(*out_handle, TIMEOUT_MAX);
    TEST_ESP_OK(err);
}

static void deinit_essl(essl_handle_t handle, const sdio_test_config_t *conf)
{
    esp_err_t err;
    essl_sdio_deinit_dev(handle);
    if (conf->sdio_mode == SDIO_SPI) {
        gpio_uninstall_isr_service();

        err = sdspi_host_deinit();
        TEST_ESP_OK(err);

        err = spi_bus_free(TEST_SDSPI_HOST);
        TEST_ESP_OK(err);
    } else {
        err = sdmmc_host_deinit();
        TEST_ESP_OK(err);
    }
}

static void send_finish_test(essl_handle_t handle)
{
    //the slave needs a signal to quite the test
    essl_send_slave_intr(handle, BIT(7), TIMEOUT_MAX);
}

static void test_int(essl_handle_t handle, const sdio_test_config_t* config)
{
    esp_err_t err;
    err = essl_set_intr_ena(handle, INT_MASK_ALL, TIMEOUT_MAX);
    TEST_ESP_OK(err);

    err = essl_wait_int(handle, 0);
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_TIMEOUT, err);

    //tests all 8 interrupts of the slave, in which int 7 is used to terminate the test on the slave.
    for (int i = 0; i < 8; i ++) {
        uint32_t int_st;
        err = essl_send_slave_intr(handle, BIT(i), TIMEOUT_MAX);
        TEST_ESP_OK(err);
        //the slave should return interrupt with the same bit in 10 ms
        err = essl_wait_int(handle, 10);
        TEST_ESP_OK(err);

        err = essl_get_intr(handle, NULL, &int_st, TIMEOUT_MAX);
        TEST_ESP_OK(err);
        //check and clear the returned interrupt
        TEST_ASSERT_EQUAL_HEX(BIT(i), int_st);
        err = essl_clear_intr(handle, int_st, TIMEOUT_MAX);
        TEST_ESP_OK(err);
    }
}

static void test_sdio_interrupt_master(void)
{
    test_framework_master(test_int, default_config);
}

static void test_reg(essl_handle_t handle, const sdio_test_config_t* config)
{
    esp_err_t err;
    uint8_t data[REG_ADDR_MAX];
    srand(850);

    //initialize the buffer
    for (int i = 0; i < REG_ADDR_MAX; i++) {
        data[i] = rand();
        err = essl_write_reg(handle, i, data[i], NULL, 10);
        TEST_ESP_OK(err);
    }

    for (int i = 0; i < 512; i++) {
        //randomly write one
        int offset = rand() % REG_ADDR_MAX;
        uint8_t data_write = rand();
        data[offset] = data_write;

        err = essl_write_reg(handle, offset, data_write, NULL, 10);
        TEST_ESP_OK(err);

        //randomly read another one and compare
        offset = rand() % REG_ADDR_MAX;
        uint8_t data_read;
        err = essl_read_reg(handle, offset, &data_read, 10);
        TEST_ESP_OK(err);

        TEST_ASSERT_EQUAL_HEX8(data[offset], data_read);
    }

    send_finish_test(handle);
}

static void test_sdio_reg_master(void)
{
    test_framework_master(test_reg, default_config);
}

static uint8_t tx_buffer[TX_BUFFER_SIZE*2];
static uint8_t rcv_buffer[RX_BUFFER_SIZE*RX_BUFFER_NUM];

static void init_tx_buffer(void)
{
    srand(776);
    for (int i = 0; i < sizeof(tx_buffer); i++) {
        tx_buffer[i] = rand();
    }
}

static void get_master_send_data(int offset, uint8_t** out_start, int* out_len)
{
    int page_cnt = offset / TX_BUFFER_SIZE;
    int offset_in_page = offset % TX_BUFFER_SIZE;
    srand(page_cnt);
    int page_offset = (rand() % (sizeof(tx_buffer) - (TX_BUFFER_SIZE) + 1)) & (~3);
    *out_start = &tx_buffer[page_offset + offset_in_page];
    *out_len = TX_BUFFER_SIZE - offset_in_page;
}

static void log_performance_tohost(uint32_t speed, const sdio_test_config_t* config)
{
    if (!config->check_data) {
        switch (config->sdio_mode) {
        case SDIO_4BIT:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_TOHOST_4BIT, "%" PRIu32, speed);
            break;
        case SDIO_1BIT:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_TOHOST_1BIT, "%" PRIu32, speed);
            break;
        case SDIO_SPI:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_TOHOST_SPI, "%" PRIu32, speed);
            break;
        }
    }
}

static void test_tp_tohost_master(essl_handle_t handle, const sdio_test_config_t* config)
{
    esp_err_t err;
    int expected_length = TEST_CNT * TX_BUFFER_SIZE;
    int recv_size = 4096;
    init_tx_buffer();
    //wait for the slave to get ready
    vTaskDelay(20);

    int remain_length = expected_length;
    int offset = 0;

    // Two counters are used. The `esp_timer_get_time()` is for the typical time, and the
    // `ccomp_timer` is for performance test to reduce influence caused by cache miss.
    int64_t pre_us = esp_timer_get_time();
    err = ccomp_timer_start();
    assert(err == ESP_OK);
    // though the flow is the same, the check of config->check_data influences the throughput much, put it outside
    if (config->check_data) {
        do {
            size_t rcv_len;
            err = essl_get_packet(handle, rcv_buffer, recv_size, &rcv_len, TIMEOUT_MAX);
            TEST_ASSERT(err == ESP_OK || err == ESP_ERR_NOT_FINISHED);
            TEST_ASSERT_LESS_OR_EQUAL(remain_length, rcv_len);

            //compare until all received data are used
            int compared_len = 0;
            do {
                //get the expected master sent data, there may be several segments, so get and compare
                //several times
                uint8_t* cmp_start;
                int seg_len;
                get_master_send_data(offset, &cmp_start, &seg_len);

                int cmp_len = MIN(rcv_len-compared_len, seg_len);

                TEST_ASSERT_EQUAL_HEX8_ARRAY(cmp_start, &rcv_buffer[compared_len], cmp_len);

                compared_len += cmp_len;
                offset += cmp_len;
            } while (compared_len < rcv_len);

            remain_length -= rcv_len;
        } while (remain_length > 0);
    } else {
        do {
            size_t rcv_len;
            err = essl_get_packet(handle, rcv_buffer, recv_size, &rcv_len, TIMEOUT_MAX);
            TEST_ASSERT(err == ESP_OK || err == ESP_ERR_NOT_FINISHED);
            TEST_ASSERT_LESS_OR_EQUAL(remain_length, rcv_len);

            offset += rcv_len;
            remain_length -= rcv_len;
        } while (remain_length > 0);
    }
    int64_t c_time_ms = ccomp_timer_stop()/1000;
    int64_t end_us = esp_timer_get_time();

    uint32_t total_time_ms = (end_us - pre_us)/1000;
    ESP_LOGI(MASTER_TAG, "test done, total time: %" PRIu32 " ms (%d ms compensated), bytes transferred: %d", total_time_ms, (int)c_time_ms, expected_length);

    uint32_t throughput_byte_per_ms = expected_length / c_time_ms;
    ESP_LOGI(MASTER_TAG, "Throughput: compensated %.2lf MB/s, typical %.2lf MB/s",
             throughput_byte_per_ms/1000., expected_length/(total_time_ms*1000.));
    log_performance_tohost(throughput_byte_per_ms, config);

    send_finish_test(handle);
}

static void log_performance_frhost(uint32_t speed, const sdio_test_config_t* config)
{
    if (!config->check_data) {
        switch (config->sdio_mode) {
        case SDIO_4BIT:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_FRHOST_4BIT, "%" PRIu32, speed);
            break;
        case SDIO_1BIT:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_FRHOST_1BIT, "%" PRIu32, speed);
            break;
        case SDIO_SPI:
            TEST_PERFORMANCE_CCOMP_GREATER_THAN(SDIO_THROUGHPUT_KBSEC_FRHOST_SPI, "%" PRIu32, speed);
            break;
        }
    }
}

static void test_tp_frhost_master(essl_handle_t handle, const sdio_test_config_t* config)
{
    esp_err_t err;
    int expected_length = TEST_CNT * TX_BUFFER_SIZE;
    init_tx_buffer();
    //wait for the slave to get ready
    vTaskDelay(20);

    int remain_length = expected_length;
    int offset = 0;

    // Two counters are used. The `esp_timer_get_time()` is for the typical time, and the
    // `ccomp_timer` is for performance test to reduce influence caused by cache miss.
    int64_t pre_us = esp_timer_get_time();
    err = ccomp_timer_start();
    assert(err == ESP_OK);
    do {
        int send_len;
        uint8_t* send_start;
        get_master_send_data(offset, &send_start, &send_len);
        TEST_ASSERT_EQUAL(TX_BUFFER_SIZE, send_len);

        err = essl_send_packet(handle, send_start, send_len, TIMEOUT_MAX);
        TEST_ASSERT(err == ESP_OK);

        remain_length -= send_len;
        offset += send_len;
    } while (remain_length > 0);

    int64_t c_time_ms = ccomp_timer_stop()/1000;
    int64_t end_us = esp_timer_get_time();

    uint32_t total_time_ms = (end_us - pre_us)/1000;
    ESP_LOGI(MASTER_TAG, "test done, total time: %" PRIu32 " ms (%d ms compensated), bytes transferred: %d", total_time_ms, (int)c_time_ms, expected_length);

    uint32_t throughput_byte_per_ms = expected_length / c_time_ms;
    ESP_LOGI(MASTER_TAG, "Throughput: compensated %.2lf MB/s, typical %.2lf MB/s",
             throughput_byte_per_ms/1000., expected_length/(total_time_ms*1000.));
    log_performance_frhost(throughput_byte_per_ms, config);

    send_finish_test(handle);
}

void test_reset_master(essl_handle_t handle, const sdio_test_config_t* config)
{
    init_tx_buffer();
    //wait for the slave to stop, reset and start again
    vTaskDelay(10);

    for (int i = 0; i < 10; i++) {
        WORD_ALIGNED_ATTR uint8_t buffer[TEST_RESET_DATA_LEN];
        size_t read_len;
        esp_err_t err = essl_get_packet(handle, buffer, TEST_RESET_DATA_LEN, &read_len, portMAX_DELAY);
        if (err == ESP_ERR_NOT_FINISHED) {
            TEST_ASSERT_LESS_THAN(10, i);
            err = ESP_OK;
        }
        TEST_ESP_OK(err);
        TEST_ASSERT_EQUAL(TEST_RESET_DATA_LEN, read_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_buffer + 4*i, buffer, read_len);
    }

    for (int i = 0; i < 10; i++) {
        esp_err_t err = essl_send_packet(handle, tx_buffer + i * 8, TEST_RESET_DATA_LEN, portMAX_DELAY);
        TEST_ESP_OK(err);
    }

    send_finish_test(handle);
}

void test_sdio_reset_master(void)
{
    test_framework_master(test_reset_master, &packet_config);
}

/*******************************************************************************
 * Slave
 ******************************************************************************/

typedef struct {
    int queued_cnt;
    bool s_finished;
} slave_context_t;

typedef void (*test_func_slave_t)(slave_context_t *context, const sdio_test_config_t* config);

static slave_context_t  slave_context;

static void event_cb(uint8_t event)
{
    ESP_EARLY_LOGI(SLAVE_TAG, "event: %d", event);
    sdio_slave_send_host_int(event);
    if (event == 7) slave_context.s_finished = true;
}


static void wait_for_finish(slave_context_t *ctx)
{
    while (!ctx->s_finished) {
        vTaskDelay(10);
    }
    //wait for host to read the respond from slave
    vTaskDelay(10);
}

static void test_framework_slave(test_func_slave_t test_func, const sdio_test_config_t* config)
{
    unity_wait_for_signal("Master ready");
    slave_context.s_finished = false;
    esp_err_t err;
    sdio_slave_config_t slave_config = {
        .sending_mode       = (config->packet_mode? SDIO_SLAVE_SEND_PACKET: SDIO_SLAVE_SEND_STREAM),
        .send_queue_size    = SDIO_SLAVE_QUEUE_SIZE,
        .recv_buffer_size   = RX_BUFFER_SIZE,
        .event_cb           = event_cb,
    };
    err = sdio_slave_initialize(&slave_config);
    TEST_ESP_OK(err);

    err = sdio_slave_start();
    TEST_ESP_OK(err);

    unity_send_signal("Slave ready");

    test_func(&slave_context, config);

    sdio_slave_stop();
    sdio_slave_deinit();
}

static void test_int_slave(slave_context_t* ctx, const sdio_test_config_t* config)
{
    wait_for_finish(ctx);
}

static void test_sdio_interrupt_slave(void)
{
    test_framework_slave(test_int_slave, default_config);
}

static void test_tp_tohost_slave(slave_context_t* ctx, const sdio_test_config_t* config)
{
#define QUEUE_FULL()    (ctx->queued_cnt == SDIO_SLAVE_QUEUE_SIZE)
#define QUEUE_EMPTY()   (ctx->queued_cnt == 0)

    init_tx_buffer();
    esp_err_t err;
    int offset = 0;
    for (int i = 0; i < TEST_CNT; i++) {
        do {
            void* arg;
            //when the queue is full, do a blocking wait for 10ms, otherwise non-blocking
            err = sdio_slave_send_get_finished(&arg, QUEUE_FULL()? 1: 0);
            if (err == ESP_OK) {
                ctx->queued_cnt --;
                continue;
            }
            TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, err);
        } while (QUEUE_FULL());

        uint8_t* start;
        int send_len;
        get_master_send_data(offset, &start, &send_len);
        TEST_ASSERT_EQUAL(TX_BUFFER_SIZE, send_len);

        err = sdio_slave_send_queue(start, send_len, NULL, portMAX_DELAY);
        TEST_ESP_OK(err);

        ctx->queued_cnt++;
        offset += TX_BUFFER_SIZE;
    }

    while (!QUEUE_EMPTY()) {
        void* arg;
        err = sdio_slave_send_get_finished(&arg, portMAX_DELAY);
        TEST_ESP_OK(err);
        ctx->queued_cnt--;
    }

    wait_for_finish(ctx);
}

static void slave_parepare_recv_buffer(void)
{
    for (int i = 0; i < RX_BUFFER_NUM; i++) {
        sdio_slave_buf_handle_t buf_handle = sdio_slave_recv_register_buf(&rcv_buffer[i*RX_BUFFER_SIZE]);
        esp_err_t err = sdio_slave_recv_load_buf(buf_handle);
        TEST_ESP_OK(err);
    }

}


static void test_tp_frhost_slave(slave_context_t *ctx, const sdio_test_config_t* config)
{
    esp_err_t err;
    init_tx_buffer();
    slave_parepare_recv_buffer();

    int offset = 0;
    for (int i = 0; i < TEST_CNT; i++) {
        sdio_slave_buf_handle_t buf_handle;
        uint8_t* buf;
        size_t rcv_len;

        err = sdio_slave_recv(&buf_handle, &buf, &rcv_len, portMAX_DELAY);
        TEST_ESP_OK(err);

        if (config->check_data) {
            //compare until all received data are used
            int compared_len = 0;
            do {
                //get the expected master sent data, there may be several segments, so get and compare
                //several times
                uint8_t* cmp_start;
                int seg_len;
                get_master_send_data(offset, &cmp_start, &seg_len);

                int cmp_len = MIN(rcv_len-compared_len, seg_len);

                TEST_ASSERT_EQUAL_HEX8_ARRAY(cmp_start, &buf[compared_len], cmp_len);

                compared_len += cmp_len;
                offset += cmp_len;
            } while (compared_len < rcv_len);
        } else {
            offset += rcv_len;
        }

        err = sdio_slave_recv_load_buf(buf_handle);
        TEST_ESP_OK(err);
    }
    wait_for_finish(ctx);
}

static void slave_tx_rx_short_data(void)
{
    esp_err_t err;
    for (int i = 0; i < 10; i++) {
        err = sdio_slave_send_queue(tx_buffer + 4*i, TEST_RESET_DATA_LEN, (void*)i, portMAX_DELAY);
        TEST_ESP_OK(err);
    }

    for (int i = 0; i < 10; i++) {
        uint8_t* addr;
        size_t size;
        sdio_slave_buf_handle_t recv_handle;
        err = sdio_slave_recv(&recv_handle, &addr, &size, portMAX_DELAY);
        TEST_ESP_OK(err);
        TEST_ASSERT_EQUAL(TEST_RESET_DATA_LEN, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_buffer+i*8, addr, size);
    }

    for (int i = 0; i < 10; i++) {
        void* arg;
        err = sdio_slave_send_get_finished(&arg, portMAX_DELAY);
        TEST_ESP_OK(err);
        TEST_ASSERT_EQUAL(i, arg);
    }
}

void test_reset_slave(slave_context_t *context, const sdio_test_config_t* config)
{
    sdio_slave_stop();

    esp_err_t err = sdio_slave_reset();
    TEST_ESP_OK(err);

    err = sdio_slave_start();
    TEST_ESP_OK(err);

    init_tx_buffer();

    slave_parepare_recv_buffer();

    slave_tx_rx_short_data();

    wait_for_finish(context);
}

void test_sdio_reset_slave(void)
{
    test_framework_slave(test_reset_slave, &packet_config);
}


TEST_CASE_MULTIPLE_DEVICES("sdio interrupt", "[sdio][test_env=sdio_master_slave]", test_sdio_interrupt_master, test_sdio_interrupt_slave);

TEST_CASE_MULTIPLE_DEVICES("sdio register", "[sdio][test_env=sdio_master_slave]", test_sdio_reg_master, test_sdio_interrupt_slave);

#if !CONFIG_FREERTOS_UNICORE
TEST_CASE_MULTIPLE_DEVICES("sdio reset", "[sdio][test_env=sdio_master_slave]", test_sdio_reset_master, test_sdio_reset_slave);
#else
//Currently there is weird issue on the runner, when tested with single core config, seems to relate to receiving
TEST_CASE_MULTIPLE_DEVICES("sdio reset", "[sdio][test_env=sdio_master_slave][ignore]", test_sdio_reset_master, test_sdio_reset_slave);
#endif


static void test_sdio_frhost_master(const void* pset, void* context)
{
    test_framework_master(test_tp_frhost_master, pset);
}

static void test_sdio_frhost_slave(const void* pset, void* context)
{
    test_framework_slave(test_tp_frhost_slave, pset);
}

static void test_sdio_tohost_master(const void* pset, void* context)
{
    test_framework_master(test_tp_tohost_master, pset);
}

static void test_sdio_tohost_slave(const void* pset, void* context)
{
    test_framework_slave(test_tp_tohost_slave, pset);
}


static void null_pre(void** arg)
{

}

static void null_post(void* arg)
{

}

ptest_func_t frhost_master = {
    .pre_test = null_pre,
    .loop = test_sdio_frhost_master,
    .post_test = null_post,
};

ptest_func_t frhost_slave = {
    .pre_test = null_pre,
    .loop = test_sdio_frhost_slave,
    .post_test = null_post,
};

PARAM_GROUP_DECLARE_TYPE(IO_MODE, sdio_test_config_t, test_cfg_array);

#if !CONFIG_FREERTOS_UNICORE
TEST_MASTER_SLAVE(SDIO_FRHOST, test_cfg_array, "[sdio][timeout=180][test_env=sdio_master_slave]", &frhost_master, &frhost_slave);
#else
//Currently there is weird issue on the runner, when tested with single core config, seems to relate to receiving
TEST_MASTER_SLAVE(SDIO_FRHOST, test_cfg_array, "[sdio][timeout=180][test_env=sdio_master_slave][ignore]", &frhost_master, &frhost_slave);
#endif

ptest_func_t tohost_master = {
    .pre_test = null_pre,
    .loop = test_sdio_tohost_master,
    .post_test = null_post,
};

ptest_func_t tohost_slave = {
    .pre_test = null_pre,
    .loop = test_sdio_tohost_slave,
    .post_test = null_post,
};

TEST_MASTER_SLAVE(SDIO_TOHOST, test_cfg_array, "[sdio][timeout=180][test_env=sdio_master_slave]", &tohost_master, &tohost_slave);

#endif //SOC_SDMMC_HOST_SUPPORTED && SOC_SDIO_SLAVE_SUPPORTED
