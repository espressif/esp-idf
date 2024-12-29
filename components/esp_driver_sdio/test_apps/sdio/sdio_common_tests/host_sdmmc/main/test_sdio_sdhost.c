/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file uses ESP32 SDMMC peripheral to test SDIO Slave peripheral SD modes
 */

#include "unity.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "ccomp_timer.h"
#include "string.h"

#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "esp_serial_slave_link/essl_sdio.h"
#include "soc/soc_caps.h"

#include "test_utils.h"
#include "idf_performance.h"
#include "test_dualboard_utils.h"
#include "../../sdio_common_test.h"

static const char *TAG = "test_sdio_sdhost";

#define TEST_HEX_LOG_LEVEL            ESP_LOG_DEBUG
#define TEST_INT_MASK_ALL             0xff
#define TEST_REG_ADDR_MAX             60
#define TEST_TIMEOUT_MAX              UINT32_MAX

typedef struct {
    uint32_t host_flags;
    uint32_t max_freq_khz;
    bool check_data;
} test_sdio_param_t;

/*---------------------------------------------------------------
                    Host Init Settings
---------------------------------------------------------------*/
static sdmmc_card_t s_card;

static void s_master_init(test_sdio_param_t *host_param, essl_handle_t *out_handle, sdmmc_card_t** out_card)
{
    sdmmc_host_t host_config = (sdmmc_host_t)SDMMC_HOST_DEFAULT();
    host_config.flags = host_param->host_flags;
    if (host_config.flags & SDMMC_HOST_FLAG_4BIT) {
        ESP_LOGI(TAG, "Probe using SD 4-bit...");
    } else if (host_config.flags & SDMMC_HOST_FLAG_1BIT) {
        ESP_LOGI(TAG, "Probe using SD 1-bit...");
    }
    host_config.max_freq_khz = host_param->max_freq_khz;
    ESP_LOGI(TAG, "Host max_freq_khz: %d", host_config.max_freq_khz);

    //init sdmmc host
    TEST_ESP_OK(sdmmc_host_init());
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    TEST_ESP_OK(sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config));

    //host init slave
    sdmmc_card_t *card = &s_card;
    //wait for at least 5 seconds
    int retry_times = 5;
    do {
        if (sdmmc_card_init(&host_config, card) == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "slave init failed, retry...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    } while (--retry_times);
    TEST_ASSERT_MESSAGE(retry_times != 0, "Initializing slave failed.");

    //init essl sdmmc host
    essl_sdio_config_t essl_sdio_config = {
        .card = card,
        .recv_buffer_size = TEST_RX_BUFFER_SIZE,
    };
    TEST_ESP_OK(essl_sdio_init_dev(out_handle, &essl_sdio_config));

    TEST_ESP_OK(essl_init(*out_handle, TEST_TIMEOUT_MAX));

    if (out_card) {
        *out_card = card;
    }
}

static void s_master_deinit(void)
{
    free(s_card.host.dma_aligned_buffer);
    s_card.host.dma_aligned_buffer = 0;

    sdmmc_host_deinit();
}

//trigger event 7 to indicate Slave to stop the test
static void s_send_finish_test(essl_handle_t handle)
{
    //the slave needs a signal to quite the test
    essl_send_slave_intr(handle, BIT(7), TEST_TIMEOUT_MAX);
}

/*---------------------------------------------------------------
                Function Tests
---------------------------------------------------------------*/

/*---------------------------------------------------------------
                SDMMC_SDIO: test interrupt
---------------------------------------------------------------*/
TEST_CASE("SDIO_SDMMC: test interrupt", "[sdio]")
{
    esp_err_t ret = ESP_FAIL;
    essl_handle_t handle = NULL;
    test_sdio_param_t test_param = {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    };
    //essl init and sdmmc init
    s_master_init(&test_param, &handle, NULL);

    TEST_ESP_OK(essl_set_intr_ena(handle, TEST_INT_MASK_ALL, TEST_TIMEOUT_MAX));
    ret = essl_wait_int(handle, 0);
    TEST_ASSERT_EQUAL_HEX(ESP_ERR_TIMEOUT, ret);

    //tests all 8 interrupts of the slave, in which int 7 is used to terminate the test on the slave.
    for (int i = 0; i < 8; i ++) {
        esp_rom_printf("to essl_send_slave_intr\n");
        TEST_ESP_OK(essl_send_slave_intr(handle, BIT(i), TEST_TIMEOUT_MAX));
        //the slave should return interrupt with the same bit in 10 ms
        TEST_ESP_OK(essl_wait_int(handle, 10));

        uint32_t int_st;
        TEST_ESP_OK(essl_get_intr(handle, NULL, &int_st, TEST_TIMEOUT_MAX));

        //check and clear the returned interrupt
        TEST_ASSERT_EQUAL_HEX(BIT(i), int_st);
        TEST_ESP_OK(essl_clear_intr(handle, int_st, TEST_TIMEOUT_MAX));
    }

    s_master_deinit();
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test register
---------------------------------------------------------------*/
TEST_CASE("SDIO_SDMMC: test register", "[sdio]")
{
    essl_handle_t handle = NULL;
    test_sdio_param_t test_param = {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    };
    //essl init and sdmmc init
    s_master_init(&test_param, &handle, NULL);

    uint32_t init_val = 30;
    srand(850);
    //initialize the registers
    for (int i = 0; i < TEST_REG_ADDR_MAX; i++) {
        TEST_ESP_OK(essl_write_reg(handle, i, init_val, NULL, 10));
    }

    for (int i = 0; i < TEST_REG_ADDR_MAX; i++) {
        uint8_t data_write = rand() % 0xFF;
        TEST_ESP_OK(essl_write_reg(handle, i, data_write, NULL, 10));

        uint8_t data_read = 0;
        TEST_ESP_OK(essl_read_reg(handle, i, &data_read, 10));

        TEST_ASSERT_EQUAL_HEX8(data_write, data_read);
    }

    s_send_finish_test(handle);
    s_master_deinit();
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test reset
---------------------------------------------------------------*/
TEST_CASE("SDIO_SDMMC: test reset", "[sdio]")
{
    essl_handle_t handle = NULL;
    test_sdio_param_t test_param = {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    };
    //essl init and sdmmc init
    s_master_init(&test_param, &handle, NULL);

    //wait for the slave to stop, reset and start again
    vTaskDelay(10);

    //rx
    WORD_ALIGNED_ATTR uint8_t slave_tx_buffer[TEST_RESET_DATA_LEN] = {};
    WORD_ALIGNED_ATTR uint8_t host_rx_buffer[TEST_RESET_DATA_LEN] = {};
    size_t read_len = 0;
    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        test_fill_random_to_buffer(i, slave_tx_buffer, TEST_RESET_DATA_LEN);
        esp_err_t ret = essl_get_packet(handle, host_rx_buffer, TEST_RESET_DATA_LEN, &read_len, portMAX_DELAY);
        if (ret == ESP_ERR_NOT_FINISHED) {
            printf("not finished\n");
            ret = ESP_OK;
        }
        TEST_ESP_OK(ret);
        TEST_ASSERT_EQUAL(TEST_RESET_DATA_LEN, read_len);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(slave_tx_buffer, host_rx_buffer, read_len);
    }

    //tx
    WORD_ALIGNED_ATTR uint8_t host_tx_buffer[TEST_RESET_BUF_NUMS][TEST_RESET_DATA_LEN] = {};
    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        test_fill_random_to_buffer(i, host_tx_buffer[i], TEST_RESET_DATA_LEN);
        TEST_ESP_OK(essl_send_packet(handle, host_tx_buffer[i], TEST_RESET_DATA_LEN, TEST_TIMEOUT_MAX));
    }

    s_send_finish_test(handle);
    s_master_deinit();
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test fixed addr
---------------------------------------------------------------*/
#include "soc/soc.h"
#define HOST_SLCHOST_CONF_W0_REG        (DR_REG_SLCHOST_BASE + 0x6C)

TEST_CASE("SDIO_SDMMC: test fixed addr", "[sdio]")
{
    essl_handle_t handle = NULL;
    sdmmc_card_t* card;
    test_sdio_param_t test_param = {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    };
    //essl init and sdmmc init
    s_master_init(&test_param, &handle, &card);

    vTaskDelay(10);

    const int test_size = 128;
    const int write_addr = 6;
    uint8_t buf[test_size] = {};
    srand(850);
    for (int i = 0; i < test_size; i++) {
        buf[i] = rand();
    }
    ESP_LOG_BUFFER_HEX("write_val", buf, test_size);

    TEST_ESP_OK(sdmmc_io_write_bytes(card, 1, ((HOST_SLCHOST_CONF_W0_REG + write_addr) & 0x3FF) | SDMMC_IO_FIXED_ADDR, buf, test_size));

    const int max_size = 64;
    uint8_t read_buf[max_size] = {};
    TEST_ESP_OK(sdmmc_io_read_bytes(card, 1, HOST_SLCHOST_CONF_W0_REG & 0x3FF, read_buf, max_size));
    ESP_LOG_BUFFER_HEX("read_all", read_buf, max_size);
    for (int i = 0; i < max_size; i++) {
        if (i >= 24 && i < 28) {
            continue;
        }
        if (i >= 32 && i < 48) {
            continue;
        }
        if (i == write_addr) {
            TEST_ASSERT_EQUAL_HEX8(buf[test_size - 1], read_buf[i]);
        } else {
            TEST_ASSERT_EQUAL_HEX8(0xcc, read_buf[i]);
        }
    }

    const int read_size = (test_size > max_size ? max_size : test_size);
    memset(read_buf, 0, read_size);
    TEST_ESP_OK(sdmmc_io_read_bytes(card, 1, ((HOST_SLCHOST_CONF_W0_REG + write_addr) & 0x3FF) | SDMMC_IO_FIXED_ADDR, read_buf, read_size));
    ESP_LOG_BUFFER_HEX("read_fixed", read_buf, read_size);
    for (int i = 0; i < read_size; i++) {
        TEST_ASSERT_EQUAL_HEX8(buf[test_size - 1], read_buf[i]);
    }

    s_send_finish_test(handle);
    s_master_deinit();
}

/*---------------------------------------------------------------
                Transaction Tests
---------------------------------------------------------------*/
test_sdio_param_t test_param_lists[TEST_TARNS_PARAM_NUMS] = {
    {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    },
    {
        .host_flags = SDMMC_HOST_FLAG_1BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    },
};

/*---------------------------------------------------------------
                SDMMC_SDIO: test from host
---------------------------------------------------------------*/
static void test_from_host(bool check_data)
{
    test_prepare_buffer_pool(TEST_RX_BUFFER_SIZE * 4, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    for (int i = 0; i < TEST_TARNS_PARAM_NUMS; i++) {
        ESP_LOGI(TAG, "host mode: %s", (test_param_lists[i].host_flags & SDMMC_HOST_FLAG_4BIT) ? "4BIT Mode" : "1BIT Mode");
        ESP_LOGI(TAG, "host speed: %"PRIu32" kHz", test_param_lists[i].max_freq_khz);

        essl_handle_t handle = NULL;
        s_master_init(&test_param_lists[i], &handle, NULL);

        // Two counters are used. The `esp_timer_get_time()` is for the typical time, and the
        // `ccomp_timer` is for performance test to reduce influence caused by cache miss.
        int64_t pre_us = esp_timer_get_time();
        TEST_ESP_OK(ccomp_timer_start());
        uint32_t expected_length = TEST_TRANS_NUMS * TEST_RX_BUFFER_SIZE;

        void *tx_buf_ptr = NULL;
        for (int j = 0; j < TEST_TRANS_NUMS; j++) {
            ESP_LOGD(TAG, "j: %d", j);

            test_get_buffer_from_pool(j, TEST_RX_BUFFER_SIZE, &tx_buf_ptr);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, tx_buf_ptr, TEST_RX_BUFFER_SIZE, TEST_HEX_LOG_LEVEL);
            TEST_ESP_OK(essl_send_packet(handle, tx_buf_ptr, TEST_RX_BUFFER_SIZE, TEST_TIMEOUT_MAX));
        }

        int64_t c_time_ms = ccomp_timer_stop() / 1000;
        int64_t end_us = esp_timer_get_time();

        uint32_t total_time_ms = (end_us - pre_us) / 1000;
        ESP_LOGI(TAG, "test done, total time: %" PRIu32 " ms (%d ms compensated), bytes transferred: %"PRIu32, total_time_ms, (int)c_time_ms, expected_length);

        uint32_t throughput_byte_per_ms = expected_length / c_time_ms;
        ESP_LOGI(TAG, "Throughput: compensated %"PRIu32"KB/s (%.2lf MB/s), typical %.2lf MB/s",
                 throughput_byte_per_ms, throughput_byte_per_ms / 1000., expected_length / (total_time_ms * 1000.));

        esp_rom_delay_us(50 * 1000);
        s_send_finish_test(handle);
        s_master_deinit();
    }

    test_destroy_buffer_pool();
}

TEST_CASE("SDIO_SDMMC: test from host", "[sdio]")
{
    test_from_host(true);
}

TEST_CASE("SDIO_SDMMC: test from host (Performance)", "[sdio_speed]")
{
    test_from_host(false);
}

static void test_to_host(bool check_data)
{
    //prepare buffer
    test_prepare_buffer_pool(TEST_RX_BUFFER_SIZE * 21, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    int recv_size = 4096;
    uint8_t *host_rx_buffer = (uint8_t *)heap_caps_calloc(1, recv_size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    for (int i = 0; i < TEST_TARNS_PARAM_NUMS; i++) {
        ESP_LOGI(TAG, "host mode: %s", (test_param_lists[i].host_flags & SDMMC_HOST_FLAG_4BIT) ? "4BIT Mode" : "1BIT Mode");
        ESP_LOGI(TAG, "host speed: %"PRIu32" kHz", test_param_lists[i].max_freq_khz);

        essl_handle_t handle = NULL;
        s_master_init(&test_param_lists[i], &handle, NULL);

        esp_err_t ret;
        int offset = 0;
        void *tx_buf_ptr = NULL;
        uint32_t expected_length = TEST_TRANS_NUMS * TEST_RX_BUFFER_SIZE;
        int remain_length = expected_length;

        // Two counters are used. The `esp_timer_get_time()` is for the typical time, and the
        // `ccomp_timer` is for performance test to reduce influence caused by cache miss.
        int64_t pre_us = esp_timer_get_time();
        TEST_ESP_OK(ccomp_timer_start());
        do {
            size_t rcv_len;
            ret = essl_get_packet(handle, host_rx_buffer, recv_size, &rcv_len, TEST_TIMEOUT_MAX);
            TEST_ASSERT(ret == ESP_OK || ret == ESP_ERR_NOT_FINISHED);
            TEST_ASSERT_LESS_OR_EQUAL(remain_length, rcv_len);

            if (check_data) {
                size_t compared_len = 0;
                do {
                    test_get_buffer_from_pool(offset, TEST_RX_BUFFER_SIZE, &tx_buf_ptr);
                    TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_buf_ptr, &host_rx_buffer[compared_len], TEST_RX_BUFFER_SIZE);
                    compared_len += TEST_RX_BUFFER_SIZE;
                    offset += TEST_RX_BUFFER_SIZE;
                } while (compared_len < rcv_len);
            }

            remain_length -= rcv_len;
        } while (remain_length > 0);

        int64_t c_time_ms = ccomp_timer_stop() / 1000;
        int64_t end_us = esp_timer_get_time();

        uint32_t total_time_ms = (end_us - pre_us) / 1000;
        ESP_LOGI(TAG, "test done, total time: %" PRIu32 " ms (%d ms compensated), bytes transferred: %"PRIu32, total_time_ms, (int)c_time_ms, expected_length);

        uint32_t throughput_byte_per_ms = expected_length / c_time_ms;
        ESP_LOGI(TAG, "Throughput: compensated %"PRIu32"KB/s %.2lf MB/s, typical %.2lf MB/s",
                 throughput_byte_per_ms, throughput_byte_per_ms / 1000., expected_length / (total_time_ms * 1000.));

        esp_rom_delay_us(50 * 1000);
        s_send_finish_test(handle);
        s_master_deinit();
    }

    free(host_rx_buffer);
    test_destroy_buffer_pool();
}

TEST_CASE("SDIO_SDMMC: test to host", "[sdio]")
{
    test_to_host(true);
}

TEST_CASE("SDIO_SDMMC: test sleep retention", "[sdio_retention]")
{
    essl_handle_t handle = NULL;
    test_sdio_param_t test_param = {
        .host_flags = SDMMC_HOST_FLAG_4BIT | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF,
        .max_freq_khz = SDMMC_FREQ_HIGHSPEED,
    };
    //essl init and sdmmc init
    s_master_init(&test_param, &handle, NULL);

    s_send_finish_test(handle);
    s_master_deinit();
}

TEST_CASE("SDIO_SDMMC: test to host (Performance)", "[sdio_speed]")
{
    test_to_host(false);
}
