/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file uses ESP32 SDMMC peripheral to test SDIO Slave peripheral SD modes
 */

#include <inttypes.h>
#include "unity.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/sdio_slave.h"
#include "soc/soc_caps.h"
#include "soc/sdio_slave_pins.h"
#include "soc/sdio_slc_struct.h"

#include "test_dualboard_utils.h"
#include "../../sdio_common_test.h"

#define TEST_HEX_LOG_LEVEL            ESP_LOG_DEBUG
#define TEST_SDIO_SLAVE_QUEUE_SIZE    20

static const char *TAG = "test_sdio";

/*---------------------------------------------------------------
                    Slave Init Settings
---------------------------------------------------------------*/
typedef struct {
    int queued_cnt;
    bool s_finished;
} s_test_slv_ctx_t;

static s_test_slv_ctx_t s_test_slv_ctx;

//callback to event 7, indicating test finish in these tests
static void s_event_cb(uint8_t event)
{
    ESP_EARLY_LOGI(TAG, "event: %d", event);
    sdio_slave_send_host_int(event);
    if (event == 7) {
        s_test_slv_ctx.s_finished = true;
    }
}

static void wait_for_finish(s_test_slv_ctx_t *ctx)
{
    while (!ctx->s_finished) {
        vTaskDelay(10);
    }
    //wait for host to read the respond from slave
    vTaskDelay(10);
}

static void s_slave_init(sdio_slave_sending_mode_t mode)
{
    s_test_slv_ctx.s_finished = false;
    s_test_slv_ctx.queued_cnt = 0;

    sdio_slave_config_t slave_config = {
        .sending_mode       = mode,
        .send_queue_size    = TEST_SDIO_SLAVE_QUEUE_SIZE,
        .recv_buffer_size   = TEST_RX_BUFFER_SIZE,
        .event_cb           = s_event_cb,
    };
    TEST_ESP_OK(sdio_slave_initialize(&slave_config));
}

/*---------------------------------------------------------------
                Function Tests
---------------------------------------------------------------*/

/*---------------------------------------------------------------
                SDMMC_SDIO: test interrupt
---------------------------------------------------------------*/
TEST_CASE("SDIO_Slave: test interrupt", "[sdio]")
{
    s_slave_init(SDIO_SLAVE_SEND_STREAM);
    TEST_ESP_OK(sdio_slave_start());
    ESP_LOGI(TAG, "slave ready");

    wait_for_finish(&s_test_slv_ctx);

    sdio_slave_stop();
    sdio_slave_deinit();
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test register
---------------------------------------------------------------*/
TEST_CASE("SDIO_Slave: test register", "[sdio]")
{
    s_slave_init(SDIO_SLAVE_SEND_STREAM);
    TEST_ESP_OK(sdio_slave_start());
    ESP_LOGI(TAG, "slave ready");

    wait_for_finish(&s_test_slv_ctx);

    sdio_slave_stop();
    sdio_slave_deinit();
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test reset
---------------------------------------------------------------*/
static void test_reset(bool reset_hw)
{
    s_slave_init(SDIO_SLAVE_SEND_PACKET);
    TEST_ESP_OK(sdio_slave_start());
    ESP_LOGI(TAG, "slave ready");

    sdio_slave_stop();
    if (!reset_hw) {
        TEST_ESP_OK(sdio_slave_reset());
    } else {
        TEST_ESP_OK(sdio_slave_reset_hw());
    }
    TEST_ESP_OK(sdio_slave_start());

    //tx
    WORD_ALIGNED_ATTR uint8_t slave_tx_buffer[TEST_RESET_BUF_NUMS][TEST_RESET_DATA_LEN] = {};
    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        test_fill_random_to_buffer(i, slave_tx_buffer[i], TEST_RESET_DATA_LEN);
        TEST_ESP_OK(sdio_slave_send_queue(slave_tx_buffer[i], TEST_RESET_DATA_LEN, (void *)i, portMAX_DELAY));
    }

    //rx
    sdio_slave_buf_handle_t buf_handle[TEST_RESET_BUF_NUMS] = {};
    WORD_ALIGNED_ATTR uint8_t slave_rx_buffer[TEST_RESET_BUF_NUMS][TEST_RESET_DATA_LEN] = {};

    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        buf_handle[i] = sdio_slave_recv_register_buf(slave_rx_buffer[i]);
        TEST_ASSERT(buf_handle[i]);
        TEST_ESP_OK(sdio_slave_recv_load_buf(buf_handle[i]));
    }

    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        void* arg;
        TEST_ESP_OK(sdio_slave_send_get_finished(&arg, portMAX_DELAY));
        TEST_ASSERT_EQUAL(i, arg);
    }

    WORD_ALIGNED_ATTR uint8_t host_tx_buffer[TEST_RESET_DATA_LEN] = {};
    for (int i = 0; i < TEST_RESET_BUF_NUMS; i++) {
        test_fill_random_to_buffer(i, host_tx_buffer, TEST_RESET_DATA_LEN);
        uint8_t* addr;
        size_t size;
        sdio_slave_buf_handle_t recv_handle;
        TEST_ESP_OK(sdio_slave_recv(&recv_handle, &addr, &size, portMAX_DELAY));
        TEST_ASSERT_EQUAL(TEST_RESET_DATA_LEN, size);
        TEST_ASSERT_EQUAL_HEX8_ARRAY(host_tx_buffer, addr, size);
    }

    wait_for_finish(&s_test_slv_ctx);

    sdio_slave_stop();
    sdio_slave_deinit();
}

TEST_CASE("SDIO_Slave: test reset", "[sdio]")
{
    test_reset(false);
}

TEST_CASE("SDIO_Slave: test reset hw", "[sdio]")
{
    test_reset(true);
}

/*---------------------------------------------------------------
                SDMMC_SDIO: test fixed addr
---------------------------------------------------------------*/
TEST_CASE("SDIO_Slave: test fixed addr", "[sdio]")
{
    s_slave_init(SDIO_SLAVE_SEND_PACKET);
    TEST_ESP_OK(sdio_slave_start());
    ESP_LOGI(TAG, "slave ready");

    for (int i = 0; i < 64; i++) {
        sdio_slave_write_reg(i, 0xcc);
    }

    wait_for_finish(&s_test_slv_ctx);

    sdio_slave_stop();
    sdio_slave_deinit();
}
/*---------------------------------------------------------------
                Transaction Tests
---------------------------------------------------------------*/
#define TEST_SLAVE_TRANS_BUF_NUMS    10

/*---------------------------------------------------------------
                From Host Tests
---------------------------------------------------------------*/
static void test_from_host(bool check_data)
{
    //prepare buffer
    test_prepare_buffer_pool(TEST_RX_BUFFER_SIZE * 4, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
    uint8_t *slave_rx_buffer[TEST_SLAVE_TRANS_BUF_NUMS] = {};
    for (int i = 0; i < TEST_SLAVE_TRANS_BUF_NUMS; i++) {
        slave_rx_buffer[i] = (uint8_t *)heap_caps_calloc(1, TEST_RX_BUFFER_SIZE, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);
        TEST_ASSERT(slave_rx_buffer[i]);
    }

    for (int i = 0; i < TEST_TARNS_PARAM_NUMS; i++) {
        //slave init
        s_slave_init(SDIO_SLAVE_SEND_STREAM);
        TEST_ESP_OK(sdio_slave_start());
        ESP_LOGI(TAG, "slave ready");

        //slave load rx buffer
        sdio_slave_buf_handle_t buf_handle[TEST_SLAVE_TRANS_BUF_NUMS] = {};
        for (int j = 0; j < TEST_SLAVE_TRANS_BUF_NUMS; j++) {
            buf_handle[j] = sdio_slave_recv_register_buf(slave_rx_buffer[j]);
            TEST_ASSERT(buf_handle[j]);
            TEST_ESP_OK(sdio_slave_recv_load_buf(buf_handle[j]));
        }

        void *tx_buf_ptr = NULL;
        for (int j = 0; j < TEST_TRANS_NUMS; j++) {
            ESP_LOGD(TAG, "j: %d", j);

            sdio_slave_buf_handle_t used_buf_handle = NULL;
            uint8_t* buf = NULL;
            size_t rcv_len = 0;

            TEST_ESP_OK(sdio_slave_recv(&used_buf_handle, &buf, &rcv_len, portMAX_DELAY));
            ESP_LOGD(TAG, "rcv_len: 0d%d", rcv_len);
            ESP_LOG_BUFFER_HEX_LEVEL(TAG, buf, TEST_RX_BUFFER_SIZE, TEST_HEX_LOG_LEVEL);

            if (check_data) {
                size_t alignment = 4;
#if CONFIG_TEST_SDIO_HOST_TARGET_ESP32P4
                alignment = 64;
#endif
                test_get_buffer_from_pool(j, TEST_RX_BUFFER_SIZE, alignment, &tx_buf_ptr);
                ESP_LOG_BUFFER_HEX_LEVEL("Expect data", tx_buf_ptr, TEST_RX_BUFFER_SIZE, TEST_HEX_LOG_LEVEL);
                TEST_ASSERT_EQUAL_HEX8_ARRAY(tx_buf_ptr, buf, rcv_len);
            }

            TEST_ESP_OK(sdio_slave_recv_load_buf(used_buf_handle));
        }

        wait_for_finish(&s_test_slv_ctx);
        sdio_slave_stop();
        sdio_slave_deinit();
    }

    for (int i = 0; i < TEST_SLAVE_TRANS_BUF_NUMS; i++) {
        free(slave_rx_buffer[i]);
    }
    test_destroy_buffer_pool();
}

TEST_CASE("SDIO_Slave: test from host", "[sdio]")
{
    test_from_host(true);
}

TEST_CASE("SDIO_Slave: test from host (Performance)", "[sdio_speed]")
{
    test_from_host(false);
}

/*---------------------------------------------------------------
                To Host Tests
---------------------------------------------------------------*/
#define QUEUE_FULL()    (s_test_slv_ctx.queued_cnt == TEST_SDIO_SLAVE_QUEUE_SIZE)
#define QUEUE_EMPTY()   (s_test_slv_ctx.queued_cnt == 0)

static void test_to_host(void)
{
    //prepare buffer
    test_prepare_buffer_pool(TEST_RX_BUFFER_SIZE * 21, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT | MALLOC_CAP_DMA);

    for (int i = 0; i < TEST_TARNS_PARAM_NUMS; i++) {
        //slave init
        s_slave_init(SDIO_SLAVE_SEND_STREAM);
        TEST_ESP_OK(sdio_slave_start());
        ESP_LOGI(TAG, "slave ready");

        esp_err_t err = ESP_OK;
        int offset = 0;
        void *tx_buf_ptr = NULL;
        for (int j = 0; j < TEST_TRANS_NUMS; j++) {
            do {
                void* arg;
                //when the queue is full, do a blocking wait for 10ms, otherwise non-blocking
                err = sdio_slave_send_get_finished(&arg, QUEUE_FULL() ? 1 : 0);
                if (err == ESP_OK) {
                    s_test_slv_ctx.queued_cnt --;
                    continue;
                }
                TEST_ASSERT_EQUAL(ESP_ERR_TIMEOUT, err);
            } while (QUEUE_FULL());

            size_t alignment = 4;
#if CONFIG_TEST_SDIO_HOST_TARGET_ESP32P4
            alignment = 64;
#endif
            test_get_buffer_from_pool(offset, TEST_RX_BUFFER_SIZE, alignment, &tx_buf_ptr);
            TEST_ESP_OK(sdio_slave_send_queue((uint8_t *)tx_buf_ptr, TEST_RX_BUFFER_SIZE, NULL, portMAX_DELAY));

            s_test_slv_ctx.queued_cnt++;
            offset += TEST_RX_BUFFER_SIZE;
        }

        while (!QUEUE_EMPTY()) {
            void* arg;
            TEST_ESP_OK(sdio_slave_send_get_finished(&arg, portMAX_DELAY));
            s_test_slv_ctx.queued_cnt--;
        }

        wait_for_finish(&s_test_slv_ctx);
        sdio_slave_stop();
        sdio_slave_deinit();
    }

    test_destroy_buffer_pool();
}

TEST_CASE("SDIO_Slave: test to host", "[sdio]")
{
    test_to_host();
}

TEST_CASE("SDIO_Slave: test to host (Performance)", "[sdio_speed]")
{
    test_to_host();
}

/*---------------------------------------------------------------
                Standalone Tests (no host required)
---------------------------------------------------------------*/
#if CONFIG_IDF_TARGET_ESP32
#define TEST_RECV_LINK_PARKED()     (SLC.slc0_tx_link.park)
#define TEST_SEND_LINK_BUF()        (SLC.slc0_rxlink_dscr_bf1)
#elif CONFIG_IDF_TARGET_ESP32C6
#define TEST_RECV_LINK_PARKED()     (SLC.slc0tx_link.slc0_txlink_park)
#define TEST_SEND_LINK_BUF()        (SLC.slc0_rxlink_dscr_bf1.val)
#else
#define TEST_RECV_LINK_PARKED()     (SLC.slc_slc0tx_link.slc0_txlink_park)
#define TEST_SEND_LINK_BUF()        (SLC.slc_slc0_rxlink_dscr_bf1.val)
#endif
// Only the low bits of the buffer address are compared, high bits of the register carry status flags
#define TEST_SEND_LINK_BUF_MASK     0xFFFFF

#define TEST_RECV_START_ROUNDS      20
#define TEST_RECV_START_BUF_NUMS    8
#define TEST_SEND_START_ROUNDS      10
#define TEST_SEND_START_PKT_NUMS    4
#define TEST_SEND_START_PKT_LEN     64

/*---------------------------------------------------------------
                SDIO_Slave: test recv start
---------------------------------------------------------------*/
TEST_CASE("SDIO_Slave: test recv start with loaded buffers", "[sdio_standalone]")
{
    static DMA_ATTR uint8_t recv_buffer[TEST_RECV_START_BUF_NUMS][TEST_RX_BUFFER_SIZE];
    int lost = 0;

    for (int round = 0; round < TEST_RECV_START_ROUNDS; round++) {
        s_slave_init(SDIO_SLAVE_SEND_STREAM);
        // Loading more than one buffer before start leaves a recv link restart pending
        for (int i = 0; i < TEST_RECV_START_BUF_NUMS; i++) {
            sdio_slave_buf_handle_t handle = sdio_slave_recv_register_buf(recv_buffer[i]);
            TEST_ASSERT(handle);
            TEST_ESP_OK(sdio_slave_recv_load_buf(handle));
        }
        TEST_ESP_OK(sdio_slave_start());

        // A started link leaves park as soon as it fetches the first descriptor
        esp_rom_delay_us(1000);
        if (TEST_RECV_LINK_PARKED()) {
            ESP_LOGE(TAG, "round %d: recv link is not started", round);
            lost++;
        }

        sdio_slave_stop();
        sdio_slave_deinit();
    }
    TEST_ASSERT_EQUAL_INT(0, lost);
}

/*---------------------------------------------------------------
                SDIO_Slave: test send start
---------------------------------------------------------------*/
static DMA_ATTR uint32_t s_send_start_buffer[TEST_SEND_START_PKT_NUMS][TEST_SEND_START_PKT_LEN / 4];

static void test_send_start_queue(void)
{
    for (int i = 0; i < TEST_SEND_START_PKT_NUMS; i++) {
        TEST_ESP_OK(sdio_slave_send_queue((uint8_t *)s_send_start_buffer[i], TEST_SEND_START_PKT_LEN, (void *)i, portMAX_DELAY));
    }
}

static int test_send_start(sdio_slave_sending_mode_t mode, bool queue_before_start)
{
    int lost = 0;

    for (int round = 0; round < TEST_SEND_START_ROUNDS; round++) {
        s_slave_init(mode);
        if (queue_before_start) {
            test_send_start_queue();
        }
        TEST_ESP_OK(sdio_slave_start());
        if (!queue_before_start) {
            test_send_start_queue();
        }

        // Without a host reading, a started link stays on one of the queued buffers
        esp_rom_delay_us(1000);
        uint32_t link_buf = TEST_SEND_LINK_BUF() & TEST_SEND_LINK_BUF_MASK;
        bool started = false;
        for (int i = 0; i < TEST_SEND_START_PKT_NUMS; i++) {
            if (link_buf == ((uint32_t)s_send_start_buffer[i] & TEST_SEND_LINK_BUF_MASK)) {
                started = true;
            }
        }
        if (!started) {
            ESP_LOGE(TAG, "mode %d, queue %s start, round %d: send link is not started (buf %05"PRIX32")",
                     mode, queue_before_start ? "before" : "after", round, link_buf);
            lost++;
        }

        sdio_slave_stop();
        TEST_ESP_OK(sdio_slave_reset());
        sdio_slave_deinit();
    }
    return lost;
}

TEST_CASE("SDIO_Slave: test send start with queued packets", "[sdio_standalone]")
{
    int lost = 0;
    lost += test_send_start(SDIO_SLAVE_SEND_PACKET, false);
    lost += test_send_start(SDIO_SLAVE_SEND_PACKET, true);
    lost += test_send_start(SDIO_SLAVE_SEND_STREAM, false);
    lost += test_send_start(SDIO_SLAVE_SEND_STREAM, true);
    TEST_ASSERT_EQUAL_INT(0, lost);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
#include "esp_private/sleep_sys_periph.h"
#include "esp_private/sleep_retention.h"

TEST_CASE("SDIO_Slave: test sleep retention", "[sdio_retention]")
{
    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);

    s_slave_init(SDIO_SLAVE_SEND_STREAM);
    TEST_ESP_OK(sdio_slave_start());
    ESP_LOGI(TAG, "slave ready");

    TEST_ASSERT_EQUAL_INT32(false, peripheral_domain_pd_allowed());
    sleep_retention_dump_modules(stdout);

    wait_for_finish(&s_test_slv_ctx);

    sdio_slave_stop();
    sdio_slave_deinit();

    TEST_ASSERT_EQUAL_INT32(true, peripheral_domain_pd_allowed());
}
#endif
