/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_ESP_COEX_GPIO_DEBUG
#include "esp_err.h"
#include "stdbool.h"
#include "soc/soc_caps.h"

#define COEX_GPIO_DEBUG_IO_INVALID  SOC_GPIO_PIN_COUNT

/* Debug signal */
#define COEX_GPIO_DEBUG_SIG_RES_US	10
typedef enum {
    COEX_GPIO_DEBUG_SIG_POSE,
    COEX_GPIO_DEBUG_SIG_NEGA,
} coex_gpio_debug_sig_t;
#define COEX_GPIO_DEBUG_SIG_TO_DURATION(sig)	((sig - COEX_GPIO_DEBUG_SIG_NEGA) * COEX_GPIO_DEBUG_SIG_RES_US)
#define COEX_GPIO_DEBUG_SIG_CHECK_US	100

/* User diagram */
#ifdef CONFIG_ESP_COEX_GPIO_DEBUG_DIAG_GENERAL
#define COEX_GPIO_DEBUG_DIAG_GENERAL	1
#elif defined(CONFIG_ESP_COEX_GPIO_DEBUG_DIAG_WIFI)
#define COEX_GPIO_DEBUG_DIAG_WIFI	1
#endif

/* User configuration validity check */
#define COEX_GPIO_DEBUG_IO_COUNT_MAX	12
#define COEX_GPIO_DEBUG_IO_COUNT	CONFIG_ESP_COEX_GPIO_DEBUG_IO_COUNT

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX0)
#define COEX_GPIO_DEBUG_IO_IDX0		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX0		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX0
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX1)
#define COEX_GPIO_DEBUG_IO_IDX1		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX1		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX1
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX2)
#define COEX_GPIO_DEBUG_IO_IDX2		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX2		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX2
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX3)
#define COEX_GPIO_DEBUG_IO_IDX3		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX3		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX3
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX4)
#define COEX_GPIO_DEBUG_IO_IDX4		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX4		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX4
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX5)
#define COEX_GPIO_DEBUG_IO_IDX5		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX5		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX5
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX6)
#define COEX_GPIO_DEBUG_IO_IDX6		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX6		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX6
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX7)
#define COEX_GPIO_DEBUG_IO_IDX7		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX7		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX7
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX8)
#define COEX_GPIO_DEBUG_IO_IDX8		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX8		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX8
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX9)
#define COEX_GPIO_DEBUG_IO_IDX9		COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX9		CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX9
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX10)
#define COEX_GPIO_DEBUG_IO_IDX10	COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX10	CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX10
#endif

#if !defined(CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX11)
#define COEX_GPIO_DEBUG_IO_IDX11	COEX_GPIO_DEBUG_IO_INVALID
#else
#define COEX_GPIO_DEBUG_IO_IDX11	CONFIG_ESP_COEX_GPIO_DEBUG_IO_IDX11
#endif

/* wifi callback -> debug */
void wifi_set_gpio_debug_cb(void (* cb)(int, coex_gpio_debug_sig_t));
int wifi_gpio_debug_max_event_get(void);

/* functions to check if in ROM */
void lmacProcessTxComplete(void);
void lmacTxFrame(void);
void pm_update_by_connectionless_status(void);
void pm_sleep(void);
void pm_dream(void);
void pm_beacon_monitor_timeout_process(void);
void pm_connectionless_wake_window_timeout_process(void);
void pm_coex_schm_process(void);
void pm_tbtt_process(void);
void pm_rx_beacon_process(void);
void ppTask(void);
void wDev_IndicateFrame(void);
void pm_check_state(void);
void pm_tx_null_data_done_process(void);
void pm_start(void);
void pm_stop(void);
void pm_disconnected_wake(void);

/* coex callback -> debug */
void coex_set_gpio_debug_cb(void (*cb)(int, coex_gpio_debug_sig_t));
int coex_gpio_debug_max_event_get(void);
esp_err_t coex_gpio_debug_matrix_init(void);

/* debug -> coex wrapper */
esp_err_t esp_coexist_debug_matrix_init(int evt, int sig, bool rev);

/* debug <-> diagram */
void wifi_bind_io_to_evt(uint8_t io_idx, uint8_t evt);
void coex_bind_io_to_evt(uint8_t io_idx, uint8_t evt);
void diagram_bind_io_to_evt(void);

/* coex -> debug
 * configure single gpio debug event */
esp_err_t coex_gpio_debug_matrix_config(int event);
/* debug -> internal use */
esp_err_t esp_coexist_gpio_debug_matrix_config(int event);

#endif
