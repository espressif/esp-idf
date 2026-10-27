/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "ble_log_sync.h"
#include "ble_log_util.h"
#include "esp_timer.h"
#include "freertos/task.h"
#if CONFIG_BLE_LOG_LL_ENABLED
#include "esp_bt.h"
#endif

#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
#include "soc/soc_caps.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_rom_sys.h"

#define SYNC_GPIO                   CONFIG_BLE_LOG_SYNC_IO_NUM
#define SYNC_UART                   UART_NUM_1
#define SYNC_IDLE_US                64
#define SYNC_IDLE_BITS              (SYNC_IDLE_US * BLE_LOG_SYNC_BAUDRATE / 1000000)
#define SYNC_BREAK_US               12
#define SYNC_SEPARATOR_US           2
#define TAG                         "BLE-Log-Sync"

static int64_t s_initial_idle_until;
static bool s_uart_installed;
static bool s_gpio_owned;

bool ble_log_sync_init(void)
{
    if (!GPIO_IS_VALID_OUTPUT_GPIO(SYNC_GPIO)) {
        ESP_LOGE(TAG, "Invalid sync output GPIO %d; check CONFIG_BLE_LOG_SYNC_IO_NUM", SYNC_GPIO);
        return false;
    }
    if (CONFIG_ESP_CONSOLE_UART_NUM == SYNC_UART) {
        ESP_LOGE(TAG, "UART1 is used by console; select another console UART to enable sync");
        return false;
    }
#if CONFIG_BLE_LOG_PRPH_UART_DMA && CONFIG_BLE_LOG_PRPH_UART_DMA_PORT == 1
    ESP_LOGE(TAG, "UART1 is used by BLE Log transport; change CONFIG_BLE_LOG_PRPH_UART_DMA_PORT to enable sync");
    return false;
#endif
    if (uart_is_driver_installed(SYNC_UART)) {
        ESP_LOGE(TAG, "UART1 driver is already installed; reserve UART1 for sync or disable sync output");
        return false;
    }
    esp_err_t ret = uart_driver_install(SYNC_UART, 256, 0, 0, NULL, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "UART1 driver installation failed: %s", esp_err_to_name(ret));
        return false;
    }
    s_uart_installed = true;
    const uart_config_t config = {
        .baud_rate = BLE_LOG_SYNC_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#if SOC_UART_SUPPORT_XTAL_CLK
        .source_clk = UART_SCLK_XTAL,
#else
        .source_clk = UART_SCLK_REF_TICK,
#endif
    };
    if ((ret = uart_param_config(SYNC_UART, &config)) != ESP_OK) {
        ESP_LOGE(TAG, "UART1 configuration failed: %s", esp_err_to_name(ret));
        goto failed;
    }
    if ((ret = uart_set_pin(SYNC_UART, SYNC_GPIO, -1, -1, -1)) != ESP_OK) {
        ESP_LOGE(TAG, "UART1 sync GPIO %d routing failed: %s", SYNC_GPIO, esp_err_to_name(ret));
        goto failed;
    }
    s_gpio_owned = true;
    if ((ret = uart_disable_rx_intr(SYNC_UART)) != ESP_OK) {
        ESP_LOGE(TAG, "UART1 RX interrupt disable failed: %s", esp_err_to_name(ret));
        goto failed;
    }
    if ((ret = uart_set_tx_idle_num(SYNC_UART, SYNC_IDLE_BITS)) != ESP_OK) {
        ESP_LOGE(TAG, "UART1 idle configuration failed: %s", esp_err_to_name(ret));
        goto failed;
    }
    if ((ret = uart_set_line_inverse(SYNC_UART, 0)) != ESP_OK) {
        ESP_LOGE(TAG, "UART1 TX polarity configuration failed: %s", esp_err_to_name(ret));
        goto failed;
    }
    if ((ret = gpio_input_enable(SYNC_GPIO)) != ESP_OK) {
        ESP_LOGE(TAG, "Sync GPIO %d input enable failed: %s", SYNC_GPIO, esp_err_to_name(ret));
        goto failed;
    }
    s_initial_idle_until = esp_timer_get_time() + SYNC_IDLE_US;
    return true;

failed:
    ble_log_sync_deinit();
    return false;
}

static int ble_log_sync_get_level(void)
{
    return s_gpio_owned ? gpio_get_level(SYNC_GPIO) : 0;
}

static bool ble_log_sync_send(uint32_t anchor_count, ble_log_ts_info_t *info)
{
    /* The public status check takes driver locks, so keep it outside sampling. */
    if (!s_gpio_owned || esp_timer_get_time() < s_initial_idle_until ||
            uart_wait_tx_done(SYNC_UART, 0) != ESP_OK) {
        return false;
    }
    uint8_t data[BLE_LOG_SYNC_BYTES];
    for (unsigned i = 0; i < 3; i++) {
        data[i] = anchor_count >> (8 * i);
    }
    BLE_LOG_ENTER_CRITICAL();
    /* The public inversion call nests a UART lock and synchronizes registers. */
    (void)uart_set_line_inverse(SYNC_UART, UART_SIGNAL_TXD_INV);
    /* The sampling helper nests the same BLE Log critical section. */
    ble_log_sync_sample(info);
    BLE_LOG_EXIT_CRITICAL();
    for (unsigned i = 0; i < 4; i++) {
        data[3 + i] = info->esp_ts >> (8 * i);
    }
    /* The sample follows T0; add 1 us for esp_timer's integer rounding. */
    uint32_t elapsed = (uint32_t)esp_timer_get_time() - info->esp_ts;
    if (elapsed < SYNC_BREAK_US + 1) {
        esp_rom_delay_us(SYNC_BREAK_US + 1 - elapsed);
    }
    (void)uart_set_line_inverse(SYNC_UART, 0);
    esp_rom_delay_us(SYNC_SEPARATOR_US);
    int sent = uart_tx_chars(SYNC_UART, (const char *)data, sizeof(data));
    if (sent != BLE_LOG_SYNC_BYTES) {
        /* A zero-byte write has no hardware trailing idle interval. */
        s_initial_idle_until = esp_timer_get_time() + SYNC_IDLE_US;
        ESP_LOGE(TAG, "Sync label send failed: %d", sent);
    }
    return true;
}

void ble_log_sync_deinit(void)
{
    if (s_uart_installed) {
        (void)uart_wait_tx_done(SYNC_UART, pdMS_TO_TICKS(20));
        uart_driver_delete(SYNC_UART);
        s_uart_installed = false;
    }
    if (s_gpio_owned) {
        gpio_reset_pin(SYNC_GPIO);
        s_gpio_owned = false;
    }
}
#endif

/* Link-layer clock sample; 0 when the controller exports no accessor. */
#if CONFIG_BLE_LOG_LL_ENABLED
#if CONFIG_BT_DUAL_MODE_ARCH
/* Temporary fallback until the dual-mode controller libraries export the
 * link-layer timer accessor. A strong library definition overrides it. */
uint32_t r_sched_timer_getCurrentTimeU32(void) __attribute__((weak));
uint32_t r_sched_timer_getCurrentTimeU32(void)
{
    return 0;
}
#define BLE_LOG_GET_LC_TS r_sched_timer_getCurrentTimeU32()
/* ESP BLE Controller Gen 2 */
#elif defined(CONFIG_IDF_TARGET_ESP32H2) || defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32C5) ||\
    defined(CONFIG_IDF_TARGET_ESP32C61) || defined(CONFIG_IDF_TARGET_ESP32H21)
extern uint32_t r_ble_lll_timer_current_tick_get(void);
#define BLE_LOG_GET_LC_TS r_ble_lll_timer_current_tick_get()
/* ESP BLE Controller Gen 1 */
#elif defined(CONFIG_IDF_TARGET_ESP32C2)
extern uint32_t r_os_cputime_get32(void);
#define BLE_LOG_GET_LC_TS r_os_cputime_get32()
/* Legacy BLE Controller */
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
extern uint32_t lld_read_clock_us(void);
#define BLE_LOG_GET_LC_TS lld_read_clock_us()
#else /* Other targets */
#define BLE_LOG_GET_LC_TS 0
#endif /* BLE targets */
#else /* !CONFIG_BLE_LOG_LL_ENABLED */
#define BLE_LOG_GET_LC_TS 0
#endif /* CONFIG_BLE_LOG_LL_ENABLED */

BLE_LOG_STATIC uint32_t ble_log_sync_lc_ts_get(void)
{
#if CONFIG_BLE_LOG_LL_ENABLED
    /* Legacy accessors dereference controller state. INIT is emitted before
     * controller initialization completes, and standalone users may keep the
     * controller idle for the entire BLE Log epoch. */
    if (esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_IDLE) {
        return 0;
    }
#endif
    return BLE_LOG_GET_LC_TS;
}

/* Captures the link-layer, ESP and OS clocks in one short sampling window. */
void ble_log_sync_sample(ble_log_ts_info_t *info)
{
    info->int_src_code = BLE_LOG_INT_SRC_TS;
    BLE_LOG_ENTER_CRITICAL();
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    info->io_level = ble_log_sync_get_level();
#else
    info->io_level = 0;
#endif
    info->lc_ts = ble_log_sync_lc_ts_get();
    info->esp_ts = esp_timer_get_time();
    info->os_ts = pdTICKS_TO_MS(xTaskGetTickCountFromISR());
    BLE_LOG_EXIT_CRITICAL();
}

void ble_log_sync_sample_anchor(ble_log_ts_info_t *info, uint32_t anchor_count)
{
#if CONFIG_BLE_LOG_TS_SYNC_TOGGLE_IO_ENABLED
    /* Keep the GPIO T0 sample even if the UART label fails. */
    if (ble_log_sync_send(anchor_count, info)) {
        return;
    }
#else
    (void)anchor_count;
#endif
    ble_log_sync_sample(info);
}
