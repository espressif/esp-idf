/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include "driver/sdmmc_defs.h"
#include "driver/uart.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_log_buffer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "sdmmc_cmd.h"
#include "soc/sdmmc_pins.h"

static const char *TAG = "example_host";

#define TEST_FRAME_LEN          16
#define SDIO_BUFFER_SIZE        128
#define LOOP_INTERVAL_MS        1000
#define INIT_RETRY_COUNT        5
#define INIT_RETRY_DELAY_MS     1000
#define IO_TIMEOUT_MS           1000
#define POLL_SLICE_MS           50
#define DEBUG_FRAME_LOG_LIMIT   8
#define SDIO_FUNC_NUM           1
#define SDIO_FUNC1_EN_MASK      BIT(1)
#define SDIO_DATA_END_ADDR      0x1f800
#define DRIVE_CAP_KEEP_DEFAULT  (-1)

#if CONFIG_EXAMPLE_ADJUSTABLE_PIN
#define PIN_CLK                 CONFIG_EXAMPLE_PIN_CLK
#define PIN_CMD                 CONFIG_EXAMPLE_PIN_CMD
#define PIN_D0                  CONFIG_EXAMPLE_PIN_D0
#define PIN_D1                  CONFIG_EXAMPLE_PIN_D1
#define PIN_D2                  CONFIG_EXAMPLE_PIN_D2
#define PIN_D3                  CONFIG_EXAMPLE_PIN_D3
#else
#define PIN_CLK                 SDMMC_SLOT1_IOMUX_PIN_NUM_CLK
#define PIN_CMD                 SDMMC_SLOT1_IOMUX_PIN_NUM_CMD
#define PIN_D0                  SDMMC_SLOT1_IOMUX_PIN_NUM_D0
#define PIN_D1                  SDMMC_SLOT1_IOMUX_PIN_NUM_D1
#define PIN_D2                  SDMMC_SLOT1_IOMUX_PIN_NUM_D2
#define PIN_D3                  SDMMC_SLOT1_IOMUX_PIN_NUM_D3
#endif

typedef enum {
    LINK_WIDTH_1BIT = 1,
    LINK_WIDTH_4BIT = 4,
} link_width_t;

typedef enum {
    LINK_SPEED_PROBING = 0,
    LINK_SPEED_DEFAULT,
    LINK_SPEED_HIGH,
} link_speed_t;

typedef struct {
    int clk_drive;
    int cmd_drive;
    int data_drive;
} drive_strength_config_t;

typedef struct {
    link_width_t width;
    link_speed_t speed;
    drive_strength_config_t drive_strength;
} link_config_t;

typedef struct {
    sdmmc_card_t *card;
    bool host_initialized;
    bool link_ready;
} host_link_t;

static const uint8_t s_pattern[TEST_FRAME_LEN] = {
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0xFF, 0x00,
};

static DRAM_DMA_ALIGNED_ATTR uint8_t s_tx_frame[TEST_FRAME_LEN];
static DRAM_DMA_ALIGNED_ATTR uint8_t s_rx_frame[TEST_FRAME_LEN];

static int read_console_key_blocking(void);

static const char *link_width_name(link_width_t width)
{
    return (width == LINK_WIDTH_4BIT) ? "4-line" : "1-line";
}

static const char *link_speed_name(link_speed_t speed)
{
    switch (speed) {
    case LINK_SPEED_PROBING:
        return "400K";
    case LINK_SPEED_HIGH:
        return "high-speed";
    case LINK_SPEED_DEFAULT:
    default:
        return "default-speed";
    }
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

static int prompt_drive_strength_selection(const char *label, int current_value)
{
    printf("\nSelect %s drive strength:\n", label);
    printf("  n - keep default\n");
    printf("  0 - GPIO_DRIVE_CAP_0\n");
    printf("  1 - GPIO_DRIVE_CAP_1\n");
    printf("  2 - GPIO_DRIVE_CAP_2\n");
    printf("  3 - GPIO_DRIVE_CAP_3\n");
    printf("Current: %s\n", drive_strength_name(current_value));
    fflush(stdout);

    switch (read_console_key_blocking()) {
    case 'n':
        return DRIVE_CAP_KEEP_DEFAULT;
    case '0':
        return GPIO_DRIVE_CAP_0;
    case '1':
        return GPIO_DRIVE_CAP_1;
    case '2':
        return GPIO_DRIVE_CAP_2;
    case '3':
        return GPIO_DRIVE_CAP_3;
    default:
        printf("Unknown selection, keeping previous value\n");
        fflush(stdout);
        return current_value;
    }
}

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

static void log_host_drive_strengths(link_width_t width, const char *label)
{
    log_gpio_drive_capability(label, PIN_CLK);
    log_gpio_drive_capability(label, PIN_CMD);
    log_gpio_drive_capability(label, PIN_D0);
    if (width == LINK_WIDTH_4BIT) {
        log_gpio_drive_capability(label, PIN_D1);
        log_gpio_drive_capability(label, PIN_D2);
        log_gpio_drive_capability(label, PIN_D3);
    }
}

static esp_err_t apply_gpio_drive_strength(gpio_num_t pin, int drive_strength)
{
    if (drive_strength == DRIVE_CAP_KEEP_DEFAULT) {
        return ESP_OK;
    }
    return gpio_set_drive_capability(pin, (gpio_drive_cap_t)drive_strength);
}

static esp_err_t apply_host_drive_strengths(const link_config_t *config)
{
    ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_CLK, config->drive_strength.clk_drive), TAG, "set CLK drive failed");
    ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_CMD, config->drive_strength.cmd_drive), TAG, "set CMD drive failed");
    ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_D0, config->drive_strength.data_drive), TAG, "set D0 drive failed");
    if (config->width == LINK_WIDTH_4BIT) {
        ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_D1, config->drive_strength.data_drive), TAG, "set D1 drive failed");
        ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_D2, config->drive_strength.data_drive), TAG, "set D2 drive failed");
        ESP_RETURN_ON_ERROR(apply_gpio_drive_strength(PIN_D3, config->drive_strength.data_drive), TAG, "set D3 drive failed");
    }
    return ESP_OK;
}

static esp_err_t enable_slave_function(sdmmc_card_t *card)
{
    uint8_t fn_enable = 0;
    ESP_RETURN_ON_ERROR(sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_ENABLE, &fn_enable), TAG, "read FN_ENABLE failed");

    fn_enable |= SDIO_FUNC1_EN_MASK;
    ESP_RETURN_ON_ERROR(sdmmc_io_write_byte(card, 0, SD_IO_CCCR_FN_ENABLE, fn_enable, NULL), TAG, "write FN_ENABLE failed");

    uint8_t fn_ready = 0;
    TickType_t deadline = xTaskGetTickCount() + pdMS_TO_TICKS(IO_TIMEOUT_MS);
    do {
        esp_err_t err = sdmmc_io_read_byte(card, 0, SD_IO_CCCR_FN_READY, &fn_ready);
        if (err != ESP_OK) {
            return err;
        }
        if ((fn_ready & SDIO_FUNC1_EN_MASK) != 0) {
            return ESP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(POLL_SLICE_MS));
    } while (xTaskGetTickCount() < deadline);

    return ESP_ERR_TIMEOUT;
}

static esp_err_t send_one_frame(host_link_t *link, const uint8_t *buffer, size_t len)
{
    uint32_t addr = SDIO_DATA_END_ADDR - len;
    return sdmmc_io_write_bytes(link->card, SDIO_FUNC_NUM, addr, buffer, len);
}

static esp_err_t receive_one_frame(host_link_t *link, uint8_t *buffer, size_t *out_len)
{
    esp_err_t err = sdmmc_io_read_bytes(link->card, SDIO_FUNC_NUM, SDIO_DATA_END_ADDR - TEST_FRAME_LEN, buffer, TEST_FRAME_LEN);
    if (err != ESP_OK) {
        return err;
    }

    *out_len = TEST_FRAME_LEN;
    return ESP_OK;
}

static void slave_power_on(void)
{
    gpio_config_t gpio_cfg = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << GPIO_NUM_18,
    };
    ESP_ERROR_CHECK(gpio_config(&gpio_cfg));
    ESP_LOGI(TAG, "power on slave");
    gpio_set_level(GPIO_NUM_18, 1);
}

static void configure_console(void)
{
    if (uart_is_driver_installed((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM)) {
        return;
    }

    setvbuf(stdin, NULL, _IONBF, 0);
    ESP_ERROR_CHECK(uart_driver_install((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM, 256, 0, 0, NULL, 0));
}

static int read_console_key_with_timeout(int timeout_ms)
{
    uint8_t byte = 0;
    int elapsed_ms = 0;

    while (elapsed_ms <= timeout_ms) {
        int wait_ms = (timeout_ms == 0) ? 0 : POLL_SLICE_MS;
        if (timeout_ms > 0 && (timeout_ms - elapsed_ms) < wait_ms) {
            wait_ms = timeout_ms - elapsed_ms;
        }

        int read_len = uart_read_bytes((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
                                       &byte,
                                       1,
                                       pdMS_TO_TICKS(wait_ms));
        if (read_len > 0) {
            if (byte == '\r' || byte == '\n') {
                continue;
            }
            return tolower(byte);
        }

        if (timeout_ms == 0) {
            break;
        }
        elapsed_ms += wait_ms;
    }

    return -1;
}

static int read_console_key_blocking(void)
{
    while (true) {
        uint8_t byte = 0;
        int read_len = uart_read_bytes((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
                                       &byte,
                                       1,
                                       portMAX_DELAY);
        if (read_len > 0) {
            if (byte == '\r' || byte == '\n') {
                continue;
            }
            return tolower(byte);
        }
    }
}

static bool wait_for_stop_key(void)
{
    for (int elapsed = 0; elapsed < LOOP_INTERVAL_MS; elapsed += POLL_SLICE_MS) {
        int key = read_console_key_with_timeout(0);
        if (key == 'z') {
            return true;
        }
        if (key >= 0) {
            ESP_LOGW(TAG, "Ignoring key '%c' while transfer loop is running. Press z to stop.", key);
        }
        vTaskDelay(pdMS_TO_TICKS(POLL_SLICE_MS));
    }
    return false;
}

static void print_setup_menu(const link_config_t *config)
{
    printf("\n=== SDIO Link Setup ===\n");
    printf("Current width: %s\n", link_width_name(config->width));
    printf("Current speed: %s\n", link_speed_name(config->speed));
    printf("CLK drive: %s\n", drive_strength_name(config->drive_strength.clk_drive));
    printf("CMD drive: %s\n", drive_strength_name(config->drive_strength.cmd_drive));
    printf("DATA drive: %s\n", drive_strength_name(config->drive_strength.data_drive));
    printf("Select bus width:\n");
    printf("  1 - 1-line\n");
    printf("  4 - 4-line\n");
    printf("Select clock mode:\n");
    printf("  p - 400K\n");
    printf("  d - default-speed\n");
    printf("  h - high-speed\n");
    printf("Drive strength:\n");
    printf("  k - configure CLK drive\n");
    printf("  m - configure CMD drive\n");
    printf("  g - configure DATA drive\n");
    printf("Press s to start link initialization\n");
    fflush(stdout);
}

static void print_traffic_menu(const link_config_t *config)
{
    printf("\n=== SDIO Traffic Menu ===\n");
    printf("Current link: %s, %s\n", link_width_name(config->width), link_speed_name(config->speed));
    printf("Press t for transmit mode\n");
    printf("Press r for receive mode\n");
    printf("Press x to close the link\n");
    fflush(stdout);
}

static void enable_bus_pullups(link_width_t width)
{
    const gpio_num_t pins_1bit[] = {PIN_CMD, PIN_CLK, PIN_D0};
    const gpio_num_t pins_4bit[] = {PIN_CMD, PIN_CLK, PIN_D0, PIN_D1, PIN_D2, PIN_D3};
    const gpio_num_t *pins = (width == LINK_WIDTH_4BIT) ? pins_4bit : pins_1bit;
    size_t pin_count = (width == LINK_WIDTH_4BIT)
                       ? sizeof(pins_4bit) / sizeof(pins_4bit[0])
                       : sizeof(pins_1bit) / sizeof(pins_1bit[0]);

    for (size_t i = 0; i < pin_count; ++i) {
        gpio_pullup_en(pins[i]);
        gpio_pulldown_dis(pins[i]);
    }
}

static void host_link_deinit(host_link_t *link)
{
    if (link == NULL) {
        return;
    }

    if (link->card) {
        sdmmc_card_deinit(link->card);
    }

    if (link->host_initialized) {
        esp_err_t err = sdmmc_host_deinit();
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "sdmmc_host_deinit failed: %s", esp_err_to_name(err));
        }
    }

    free(link->card);
    memset(link, 0, sizeof(*link));
}

static esp_err_t host_link_init(const link_config_t *config, host_link_t *link)
{
    esp_err_t err;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    host.flags = ((config->width == LINK_WIDTH_4BIT) ? SDMMC_HOST_FLAG_4BIT : SDMMC_HOST_FLAG_1BIT)
                 | SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF;
    switch (config->speed) {
    case LINK_SPEED_PROBING:
        host.max_freq_khz = SDMMC_FREQ_PROBING;
        break;
    case LINK_SPEED_HIGH:
        host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
        break;
    case LINK_SPEED_DEFAULT:
    default:
        host.max_freq_khz = SDMMC_FREQ_DEFAULT;
        break;
    }

    slot_config.width = config->width;
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = PIN_CLK;
    slot_config.cmd = PIN_CMD;
    slot_config.d0 = PIN_D0;
    slot_config.d1 = PIN_D1;
    slot_config.d2 = PIN_D2;
    slot_config.d3 = PIN_D3;
#endif

    link->card = calloc(1, sizeof(sdmmc_card_t));
    if (link->card == NULL) {
        return ESP_ERR_NO_MEM;
    }

    err = sdmmc_host_init();
    if (err != ESP_OK) {
        host_link_deinit(link);
        return err;
    }
    link->host_initialized = true;

    err = sdmmc_host_init_slot(SDMMC_HOST_SLOT_1, &slot_config);
    if (err != ESP_OK) {
        host_link_deinit(link);
        return err;
    }

    for (int attempt = 0; attempt < INIT_RETRY_COUNT; ++attempt) {
        err = sdmmc_card_init(&host, link->card);
        if (err == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "sdmmc_card_init failed (%s), retrying...", esp_err_to_name(err));
        vTaskDelay(pdMS_TO_TICKS(INIT_RETRY_DELAY_MS));
    }
    if (err != ESP_OK) {
        host_link_deinit(link);
        return err;
    }

    enable_bus_pullups(config->width);
    log_host_drive_strengths(config->width, "Host default");
    err = apply_host_drive_strengths(config);
    if (err != ESP_OK) {
        host_link_deinit(link);
        return err;
    }
    log_host_drive_strengths(config->width, "Host active");

    err = enable_slave_function(link->card);
    if (err != ESP_OK) {
        host_link_deinit(link);
        return err;
    }

    link->link_ready = true;
    return ESP_OK;
}

static void run_transmit_mode(host_link_t *link)
{
    fill_test_pattern(s_tx_frame, sizeof(s_tx_frame));
    printf("Entering transmit mode\n");
    fflush(stdout);

    while (true) {
        esp_err_t err = send_one_frame(link, s_tx_frame, sizeof(s_tx_frame));
        if (err == ESP_OK) {
            printf("TX transfer success\n");
        } else {
            printf("TX transfer error: %s\n", esp_err_to_name(err));
        }
        fflush(stdout);

        if (wait_for_stop_key()) {
            break;
        }
    }
}

static void run_receive_mode(host_link_t *link)
{
    uint32_t frame_index = 0;

    printf("Entering receive mode\n");
    fflush(stdout);

    while (true) {
        size_t received_len = 0;
        esp_err_t err = receive_one_frame(link, s_rx_frame, &received_len);
        ++frame_index;
        if (err == ESP_OK) {
            bool valid = validate_test_pattern(s_rx_frame, received_len);
            if (frame_index <= DEBUG_FRAME_LOG_LIMIT || !valid) {
                log_frame_debug("RX", frame_index, s_rx_frame, received_len);
            }
            if (valid) {
                printf("RX pattern validation: success\n");
            } else {
                int mismatch_index = find_pattern_mismatch(s_rx_frame, received_len);
                if (mismatch_index >= 0 && mismatch_index < TEST_FRAME_LEN && received_len > (size_t)mismatch_index) {
                    ESP_LOGE(TAG,
                             "RX mismatch at byte %d: expected 0x%02X got 0x%02X",
                             mismatch_index,
                             s_pattern[mismatch_index],
                             s_rx_frame[mismatch_index]);
                } else {
                    ESP_LOGE(TAG,
                             "RX length mismatch: expected %u got %u",
                             TEST_FRAME_LEN,
                             (unsigned)received_len);
                }
                printf("RX pattern validation: failure\n");
            }
        } else {
            printf("RX transfer error: %s\n", esp_err_to_name(err));
        }
        fflush(stdout);

        if (wait_for_stop_key()) {
            break;
        }
    }
}

void app_main(void)
{
    link_config_t config = {
        .width = LINK_WIDTH_1BIT,
        .speed = LINK_SPEED_DEFAULT,
        .drive_strength = {
            .clk_drive = DRIVE_CAP_KEEP_DEFAULT,
            .cmd_drive = DRIVE_CAP_KEEP_DEFAULT,
            .data_drive = DRIVE_CAP_KEEP_DEFAULT,
        },
    };

    slave_power_on();
    configure_console();

    while (true) {
        host_link_t link = {0};

        while (!link.link_ready) {
            print_setup_menu(&config);

            switch (read_console_key_blocking()) {
            case '1':
                config.width = LINK_WIDTH_1BIT;
                break;
            case '4':
                config.width = LINK_WIDTH_4BIT;
                break;
            case 'd':
                config.speed = LINK_SPEED_DEFAULT;
                break;
            case 'p':
                config.speed = LINK_SPEED_PROBING;
                break;
            case 'h':
                config.speed = LINK_SPEED_HIGH;
                break;
            case 'k':
                config.drive_strength.clk_drive = prompt_drive_strength_selection("CLK", config.drive_strength.clk_drive);
                break;
            case 'm':
                config.drive_strength.cmd_drive = prompt_drive_strength_selection("CMD", config.drive_strength.cmd_drive);
                break;
            case 'g':
                config.drive_strength.data_drive = prompt_drive_strength_selection("DATA", config.drive_strength.data_drive);
                break;
            case 's': {
                esp_err_t err = host_link_init(&config, &link);
                if (err == ESP_OK) {
                    printf("SDIO link initialized successfully\n");
                } else {
                    printf("SDIO link initialization failed: %s\n", esp_err_to_name(err));
                    host_link_deinit(&link);
                }
                fflush(stdout);
                break;
            }
            default:
                printf("Unknown selection\n");
                fflush(stdout);
                break;
            }
        }

        while (link.link_ready) {
            print_traffic_menu(&config);

            switch (read_console_key_blocking()) {
            case 't':
                run_transmit_mode(&link);
                break;
            case 'r':
                run_receive_mode(&link);
                break;
            case 'x':
                host_link_deinit(&link);
                printf("SDIO link closed\n");
                fflush(stdout);
                break;
            default:
                printf("Unknown selection\n");
                fflush(stdout);
                break;
            }
        }
    }
}
