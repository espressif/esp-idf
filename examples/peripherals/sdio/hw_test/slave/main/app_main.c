/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/sdio_slave.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_log_buffer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/sdio_slave_pins.h"

#define TAG                  "example_slave"
#define TEST_FRAME_LEN       16
#define SDIO_BUFFER_SIZE     128
#define RX_BUFFER_COUNT      16
#define TX_BUFFER_COUNT      8
#define LOOP_IDLE_DELAY_MS   20
#define DEBUG_FRAME_LOG_LIMIT 8
#define DRIVE_CAP_KEEP_DEFAULT (-1)
#define SLAVE_CLK_DRIVE_CAP  DRIVE_CAP_KEEP_DEFAULT
#define SLAVE_CMD_DRIVE_CAP  DRIVE_CAP_KEEP_DEFAULT
#define SLAVE_DATA_DRIVE_CAP DRIVE_CAP_KEEP_DEFAULT

static const uint8_t s_pattern[TEST_FRAME_LEN] = {
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
};

static DMA_ATTR uint8_t s_rx_buffers[RX_BUFFER_COUNT][SDIO_BUFFER_SIZE];
static DMA_ATTR uint8_t s_tx_buffers[TX_BUFFER_COUNT][TEST_FRAME_LEN];
static uint32_t s_tx_frame_counter;

static const gpio_num_t s_clk_pin = (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CLK;
static const gpio_num_t s_cmd_pin = (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_CMD;
static const gpio_num_t s_data_pins[] = {
    (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D0,
    (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D1,
    (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D2,
    (gpio_num_t)SDIO_SLAVE_SLOT0_IOMUX_PIN_NUM_D3,
};

static void fill_test_pattern(uint8_t *buffer, size_t len)
{
    for (size_t i = 0; i < len; ++i) {
        buffer[i] = s_pattern[i % TEST_FRAME_LEN];
    }
}

static bool validate_test_pattern(const uint8_t *buffer, size_t len)
{
    if (len != TEST_FRAME_LEN) {
        return false;
    }

    return memcmp(buffer, s_pattern, TEST_FRAME_LEN) == 0;
}

static int find_pattern_mismatch(const uint8_t *buffer, size_t len)
{
    size_t compare_len = (len < TEST_FRAME_LEN) ? len : TEST_FRAME_LEN;
    for (size_t i = 0; i < compare_len; ++i) {
        if (buffer[i] != s_pattern[i]) {
            return (int)i;
        }
    }
    if (len != TEST_FRAME_LEN) {
        return (int)compare_len;
    }
    return -1;
}

static void log_frame_debug(const char *label, uint32_t frame_index, const uint8_t *buffer, size_t len)
{
    ESP_LOGI(TAG, "%s frame=%" PRIu32 " len=%u", label, frame_index, (unsigned)len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, buffer, len, ESP_LOG_INFO);
}

static const char *drive_strength_name(int value)
{
    switch (value) {
    case DRIVE_CAP_KEEP_DEFAULT:
        return "default";
    case GPIO_DRIVE_CAP_0:
        return "cap0";
    case GPIO_DRIVE_CAP_1:
        return "cap1";
    case GPIO_DRIVE_CAP_2:
        return "cap2";
    case GPIO_DRIVE_CAP_3:
        return "cap3";
    default:
        return "unknown";
    }
}

static void log_gpio_drive_capability(const char *label, gpio_num_t pin)
{
    gpio_drive_cap_t capability = GPIO_DRIVE_CAP_DEFAULT;
    esp_err_t err = gpio_get_drive_capability(pin, &capability);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "%s GPIO%d drive capability: %s (%d)", label, pin, drive_strength_name(capability), capability);
    } else {
        ESP_LOGW(TAG, "Failed to read drive capability for GPIO%d: %s", pin, esp_err_to_name(err));
    }
}

static esp_err_t apply_gpio_drive_strength(gpio_num_t pin, int drive_strength)
{
    if (drive_strength == DRIVE_CAP_KEEP_DEFAULT) {
        return ESP_OK;
    }
    return gpio_set_drive_capability(pin, (gpio_drive_cap_t)drive_strength);
}

static esp_err_t apply_slave_drive_strengths(void)
{
    esp_err_t err = apply_gpio_drive_strength(s_clk_pin, SLAVE_CLK_DRIVE_CAP);
    if (err != ESP_OK) {
        return err;
    }
    err = apply_gpio_drive_strength(s_cmd_pin, SLAVE_CMD_DRIVE_CAP);
    if (err != ESP_OK) {
        return err;
    }
    for (size_t i = 0; i < sizeof(s_data_pins) / sizeof(s_data_pins[0]); ++i) {
        err = apply_gpio_drive_strength(s_data_pins[i], SLAVE_DATA_DRIVE_CAP);
        if (err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

static void log_slave_drive_strengths(const char *label)
{
    log_gpio_drive_capability(label, s_clk_pin);
    log_gpio_drive_capability(label, s_cmd_pin);
    for (size_t i = 0; i < sizeof(s_data_pins) / sizeof(s_data_pins[0]); ++i) {
        log_gpio_drive_capability(label, s_data_pins[i]);
    }
}

static void init_rx_buffers(void)
{
    for (int i = 0; i < RX_BUFFER_COUNT; ++i) {
        sdio_slave_buf_handle_t handle = sdio_slave_recv_register_buf(s_rx_buffers[i]);
        assert(handle != NULL);
        ESP_ERROR_CHECK(sdio_slave_recv_load_buf(handle));
    }
}

static void queue_tx_buffer(uint8_t *buffer)
{
    fill_test_pattern(buffer, TEST_FRAME_LEN);
    ++s_tx_frame_counter;
    if (s_tx_frame_counter <= DEBUG_FRAME_LOG_LIMIT) {
        log_frame_debug("TX queue", s_tx_frame_counter, buffer, TEST_FRAME_LEN);
    }
    ESP_ERROR_CHECK(sdio_slave_send_queue(buffer, TEST_FRAME_LEN, buffer, portMAX_DELAY));
}

static void init_tx_buffers(void)
{
    for (int i = 0; i < TX_BUFFER_COUNT; ++i) {
        queue_tx_buffer(s_tx_buffers[i]);
    }
}

static void recycle_finished_tx_buffers(void)
{
    while (true) {
        void *arg = NULL;
        esp_err_t err = sdio_slave_send_get_finished(&arg, 0);
        if (err == ESP_ERR_TIMEOUT) {
            return;
        }
        ESP_ERROR_CHECK(err);

        uint8_t *buffer = (uint8_t *)arg;
        queue_tx_buffer(buffer);
        printf("TX buffer recycled\n");
        fflush(stdout);
    }
}

static bool process_rx_packet(void)
{
    sdio_slave_buf_handle_t handle = NULL;
    uint8_t *buffer = NULL;
    size_t length = 0;
    esp_err_t err = sdio_slave_recv(&handle, &buffer, &length, 0);

    if (err == ESP_ERR_TIMEOUT) {
        return false;
    }
    ESP_ERROR_CHECK(err);

    if (validate_test_pattern(buffer, length)) {
        static uint32_t rx_frame_counter;
        ++rx_frame_counter;
        if (rx_frame_counter <= DEBUG_FRAME_LOG_LIMIT) {
            log_frame_debug("RX", rx_frame_counter, buffer, length);
        }
        printf("RX pattern validation: success\n");
    } else {
        int mismatch_index = find_pattern_mismatch(buffer, length);
        log_frame_debug("RX invalid", 0, buffer, length);
        if (mismatch_index >= 0 && mismatch_index < TEST_FRAME_LEN && length > (size_t)mismatch_index) {
            ESP_LOGE(TAG,
                     "RX mismatch at byte %d: expected 0x%02X got 0x%02X",
                     mismatch_index,
                     s_pattern[mismatch_index],
                     buffer[mismatch_index]);
        } else {
            ESP_LOGE(TAG,
                     "RX length mismatch: expected %u got %u",
                     TEST_FRAME_LEN,
                     (unsigned)length);
        }
        printf("RX pattern validation: failure\n");
    }
    fflush(stdout);

    ESP_ERROR_CHECK(sdio_slave_recv_load_buf(handle));
    return true;
}

void app_main(void)
{
    sdio_slave_config_t config = {
        .sending_mode = SDIO_SLAVE_SEND_PACKET,
        .send_queue_size = TX_BUFFER_COUNT,
        .recv_buffer_size = SDIO_BUFFER_SIZE,
        .event_cb = NULL,
    };

    ESP_ERROR_CHECK(sdio_slave_initialize(&config));
    log_slave_drive_strengths("Slave default");
    ESP_ERROR_CHECK(apply_slave_drive_strengths());
    log_slave_drive_strengths("Slave active");
    init_rx_buffers();
    init_tx_buffers();

    ESP_ERROR_CHECK(sdio_slave_start());

    printf("SDIO slave ready for channel test\n");
    fflush(stdout);

    while (true) {
        bool did_work = false;

        recycle_finished_tx_buffers();
        did_work |= process_rx_packet();

        if (!did_work) {
            vTaskDelay(pdMS_TO_TICKS(LOOP_IDLE_DELAY_MS));
        }
    }
}
