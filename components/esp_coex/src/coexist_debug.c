/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "private/esp_coexist_debug.h"

#include <stdbool.h>
#include "esp_err.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_log.h"

#include "rom/gpio.h"
#include "rom/ets_sys.h"
#include "driver/gpio.h"
#include "soc/gpio_sig_map.h"
#include "esp_rom_gpio.h"
#include "soc/soc.h"
#include "esp_private/periph_ctrl.h"

#if CONFIG_ESP_COEX_GPIO_DEBUG
static char* TAG = "coexist debug";

__attribute__((weak)) void wifi_set_gpio_debug_cb(void (* cb)(int, coex_gpio_debug_sig_t))
{
    ESP_LOGW(TAG, "Not support: %s", __FUNCTION__);
}
__attribute__((weak)) int wifi_gpio_debug_max_event_get(void)
{
    ESP_LOGW(TAG, "Not support: %s", __FUNCTION__);
    return 0;
}

__attribute__((weak)) void coex_set_gpio_debug_cb(void (*cb)(int, coex_gpio_debug_sig_t))
{
    ESP_LOGW(TAG, "Not support: %s", __FUNCTION__);
}

__attribute__((weak)) int coex_gpio_debug_max_event_get(void)
{
    ESP_LOGW(TAG, "Not support: %s", __FUNCTION__);
    return 0;
}

__attribute__((weak)) esp_err_t coex_gpio_debug_matrix_init(void)
{
    ESP_LOGW(TAG, "Not support: %s", __FUNCTION__);
    return ESP_OK;
}

/* Check if functions in ROM */
static const void* rom_funcs[] = {
#if CONFIG_ESP_WIFI_ENABLED
    lmacProcessTxComplete,
    lmacTxFrame,
    pm_update_by_connectionless_status,
    pm_sleep,
    pm_dream,
    pm_beacon_monitor_timeout_process,
    pm_connectionless_wake_window_timeout_process,
    pm_coex_schm_process,
    pm_tbtt_process,
    pm_rx_beacon_process,
    ppTask,
    wDev_IndicateFrame,
    pm_check_state,
    pm_tx_null_data_done_process,
    pm_start,
    pm_stop,
    pm_disconnected_wake,
#endif
};
static const char* rom_funcs_name[] = {
#if CONFIG_ESP_WIFI_ENABLED
    "lmacProcessTxComplete",
    "lmacTxframe",
    "pm_update_by_connectionless_status",
    "pm_sleep",
    "pm_dream",
    "pm_beacon_monitor_timeout_process",
    "pm_connectionless_wake_window_timeout_process",
    "pm_coex_schm_process",
    "pm_tbtt_process",
    "pm_rx_beacon_process",
    "ppTask",
    "wDev_IndicateFrame",
    "pm_check_state",
    "pm_tx_null_data_done_process",
    "pm_start",
    "pm_stop",
    "pm_disconnected_wake",
#endif
};

static bool check_funcs_in_rom(void)
{
    bool in_rom = false;
    for (uint8_t i = 0; i < sizeof(rom_funcs) / sizeof(void*); i++) {
        if ((uint32_t)rom_funcs[i] >= SOC_IROM_MASK_LOW && (uint32_t)rom_funcs[i] <= SOC_IROM_MASK_HIGH) {
            ESP_LOGE(TAG, "remove function from ROM: %s", rom_funcs_name[i]);
            in_rom = true;
        }
    }
    return in_rom;
}

/* Define used IO nums */
static const DRAM_ATTR gpio_num_t s_io_nums[COEX_GPIO_DEBUG_IO_COUNT_MAX] = {
    COEX_GPIO_DEBUG_IO_IDX0,
    COEX_GPIO_DEBUG_IO_IDX1,
    COEX_GPIO_DEBUG_IO_IDX2,
    COEX_GPIO_DEBUG_IO_IDX3,
    COEX_GPIO_DEBUG_IO_IDX4,
    COEX_GPIO_DEBUG_IO_IDX5,
    COEX_GPIO_DEBUG_IO_IDX6,
    COEX_GPIO_DEBUG_IO_IDX7,
    COEX_GPIO_DEBUG_IO_IDX8,
    COEX_GPIO_DEBUG_IO_IDX9,
    COEX_GPIO_DEBUG_IO_IDX10,
    COEX_GPIO_DEBUG_IO_IDX11,
};

/* Mapping from evt to IO */
static DRAM_ATTR gpio_num_t *s_evt_io_map, *s_wifi_evt_io_map, *s_coex_evt_io_map;
static DRAM_ATTR uint8_t s_wifi_evt_max, s_coex_evt_max;

inline static void bind_io_to_evt(gpio_num_t *ptrmap, uint8_t io, uint8_t evt)
{
    ptrmap[evt] = io;
}

inline static void evt_set_signal(gpio_num_t io, coex_gpio_debug_sig_t sig)
{
    if (sig == COEX_GPIO_DEBUG_SIG_POSE) {
        gpio_set_level(io, true);
    } else if (sig == COEX_GPIO_DEBUG_SIG_NEGA) {
        gpio_set_level(io, false);
    } else {
        gpio_set_level(io, true);
        esp_rom_delay_us(COEX_GPIO_DEBUG_SIG_TO_DURATION(sig));
        gpio_set_level(io, false);
    }
}

void wifi_bind_io_to_evt(uint8_t io_idx, uint8_t evt)
{
    if (!s_wifi_evt_io_map || evt >= s_wifi_evt_max || io_idx >= COEX_GPIO_DEBUG_IO_COUNT) {
        return;
    }
    ESP_LOGI(TAG, "Bind IO %u to Wi-Fi evt %u", s_io_nums[io_idx], evt);
    bind_io_to_evt(s_wifi_evt_io_map, s_io_nums[io_idx], evt);
}

void coex_bind_io_to_evt(uint8_t io_idx, uint8_t evt)
{
    if (!s_coex_evt_io_map || evt >= s_coex_evt_max || io_idx >= COEX_GPIO_DEBUG_IO_COUNT) {
        return;
    }
    ESP_LOGI(TAG, "Bind IO %u to coexist evt %u", s_io_nums[io_idx], evt);
    bind_io_to_evt(s_coex_evt_io_map, s_io_nums[io_idx], evt);
}

IRAM_ATTR void wifi_set_gpio_debug(int evt, coex_gpio_debug_sig_t sig)
{
    if (evt >= s_wifi_evt_max || s_wifi_evt_io_map[evt] == COEX_GPIO_DEBUG_IO_INVALID) {
        return;
    }
    evt_set_signal(s_wifi_evt_io_map[evt], sig);
}

IRAM_ATTR void coex_set_gpio_debug(int evt, coex_gpio_debug_sig_t sig)
{
    if (evt >= s_coex_evt_max || s_coex_evt_io_map[evt] == COEX_GPIO_DEBUG_IO_INVALID) {
        return;
    }
    evt_set_signal(s_coex_evt_io_map[evt], sig);
}

esp_err_t esp_coexist_debug_matrix_init(int evt, int sig, bool rev)
{
    if (evt >= s_coex_evt_max || s_coex_evt_io_map[evt] == COEX_GPIO_DEBUG_IO_INVALID) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_rom_gpio_connect_out_signal(s_coex_evt_io_map[evt], sig, rev, false);
    return ESP_OK;
}

esp_err_t esp_coexist_gpio_debug_matrix_config(int event)
{
    coex_module_enable();
    esp_err_t ret = coex_gpio_debug_matrix_config(event);
    coex_module_disable();
    return ret;
}

esp_err_t esp_coexist_debug_init(void)
{
    if (check_funcs_in_rom()) {
        return ESP_ERR_INVALID_STATE;
    }

    s_wifi_evt_max = wifi_gpio_debug_max_event_get();
    s_coex_evt_max = coex_gpio_debug_max_event_get();
    uint8_t evt_max = s_wifi_evt_max + s_coex_evt_max;
    if (evt_max == 0) {
        return ESP_ERR_NOT_SUPPORTED;
    }

    /* Allocate binding map */
    s_evt_io_map = malloc(sizeof(gpio_num_t) * evt_max);
    if (!s_evt_io_map) {
        return ESP_ERR_NO_MEM;
    }
    /* Init to invalid IO num */
    for (uint8_t i = 0; i < evt_max; i++) {
        s_evt_io_map[i] = COEX_GPIO_DEBUG_IO_INVALID;
    }
    s_wifi_evt_io_map = s_evt_io_map;
    s_coex_evt_io_map = s_evt_io_map + s_wifi_evt_max;

    /* binding map configuration */
    diagram_bind_io_to_evt();

    /* Register callback for Wi-Fi evt */
    wifi_set_gpio_debug_cb(wifi_set_gpio_debug);

    /* Register callback for coexist evt */
    coex_set_gpio_debug_cb(coex_set_gpio_debug);

    /* IO init and validity check */
    gpio_config_t io_conf = {
        //disable interrupt
        .intr_type = GPIO_INTR_DISABLE,
        //set as output mode
        .mode = GPIO_MODE_OUTPUT,
        //bit mask of the pins that you want to set,e.g.GPIO18/19
        .pin_bit_mask = 0,
        //disable pull-down mode
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        //enable pull-up mode
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };

    for (uint8_t i = 0; i < COEX_GPIO_DEBUG_IO_COUNT; i++) {
        gpio_num_t io = s_io_nums[i];
        io_conf.pin_bit_mask = (1ULL << io);
        gpio_config(&io_conf);
        gpio_set_level(io, 0);
    }
    esp_rom_delay_us(COEX_GPIO_DEBUG_SIG_CHECK_US);
    for (uint8_t i = 0; i < COEX_GPIO_DEBUG_IO_COUNT; i++) {
        gpio_set_level(s_io_nums[i], true);
    }
    esp_rom_delay_us(COEX_GPIO_DEBUG_SIG_CHECK_US);
    for (uint8_t i = 0; i < COEX_GPIO_DEBUG_IO_COUNT; i++) {
        gpio_set_level(s_io_nums[i], false);
    }

    coex_module_enable();
    /* Init coexist hardware signal */
    ESP_ERROR_CHECK(coex_gpio_debug_matrix_init());
    coex_module_disable();

    return ESP_OK;
}

#endif
