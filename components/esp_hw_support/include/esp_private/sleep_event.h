/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {
    /**
     * Using SLEEP_EVENT to determine the execution of specific
     * code at a particular point in the sleep flow.
    */
    SLEEP_EVENT_HW_EXIT_SLEEP,            // CPU wake up and start to work
    SLEEP_EVENT_SW_CLK_READY,             // CPU frequency restore
    SLEEP_EVENT_SW_EXIT_SLEEP,            // End of esp_light_sleep_start
    SLEEP_EVENT_SW_GOTO_SLEEP,            // Beginning of esp_light_sleep_start
    SLEEP_EVENT_HW_TIME_START,            // Start timing the sleep time
    SLEEP_EVENT_HW_GOTO_SLEEP,            // Hardware is about to power off
    SLEEP_EVENT_SW_CPU_TO_MEM_START,      // CPU registers are starting to be saved
    SLEEP_EVENT_SW_CPU_TO_MEM_END,        // CPU registers have finished saving
#if CONFIG_IDF_TARGET_ESP32H2
    SLEEP_EVENT_HW_FLASH_BBPLL_EN_START,  // Beginning of rtc_clk_bbpll_enable when using FLASH_PLL
    SLEEP_EVENT_HW_FLASH_BBPLL_EN_STOP,   // End of rtc_clk_bbpll_enable when using FLASH_PLL
#endif
    SLEEP_EVENT_HW_PLL_EN_START,          // Beginning of rtc_clk_pll_enable
    SLEEP_EVENT_HW_PLL_EN_STOP,           // End of rtc_clk_pll_enable
    SLEEP_EVENT_CB_INDEX_NUM,
} esp_sleep_event_cb_index_t;

/**
 * @brief Function prototype for light sleep event callback functions (if CONFIG_FREERTOS_USE_TICKLESS_IDLE).
 * @param user_arg is the user provided argument while registering callbacks.
 * @param ext_arg is an externally provided parameter that is used when the callback is executed.
 * @return None
 */

typedef esp_err_t (*esp_sleep_event_cb_t)(void *user_arg, void *ext_arg);

/**
 * @brief Function entry parameter types for light sleep event callback functions (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 */
struct _esp_sleep_event_cb_config_t {
    /**
     * Callback function defined by internal developers.
     */
    esp_sleep_event_cb_t cb;
    /**
     * Input parameters of callback function defined by internal developers.
     */
    void *user_arg;
    /**
     * Execution priority of callback function defined by internal developers.
     * The smaller the priority, the earlier it executes when call esp_sleep_execute_event_callbacks.
     * If functions have the same priority, the function registered first will be executed first.
     */
    uint32_t prior;
    /**
     * Next callback configuration defined by internal developer.
     */
    struct _esp_sleep_event_cb_config_t *next;
};

typedef struct _esp_sleep_event_cb_config_t esp_sleep_event_cb_config_t;

struct _esp_sleep_event_cbs_config_t {
    /**
     * Callback configurations defined by internal developers.
     */
    esp_sleep_event_cb_config_t *sleep_event_cb_config[SLEEP_EVENT_CB_INDEX_NUM];
};

typedef struct _esp_sleep_event_cbs_config_t esp_sleep_event_cbs_config_t;

/**
 * @brief Register event callbacks for light sleep internal events (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 * @param event_id      Designed to register the corresponding event_cb in g_sleep_event_cbs_config
 * @param event_cb_conf Config struct containing event callback function and corresponding argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the input parameter event_cb_conf is NULL or event_id is out of range
 *      - ESP_ERR_NO_MEM if the remaining memory is insufficient to support malloc
 *      - ESP_FAIL if register the same function repeatedly
 *
 * @note Some of these callback functions are called from IDLE task context hence they cannot call any blocking functions
 * @note Passing NULL value will not deregister the callbacks, it will silently ignore and return ESP_OK
 */
esp_err_t esp_sleep_register_event_callback(esp_sleep_event_cb_index_t event_id, const esp_sleep_event_cb_config_t *event_cb_conf);

/**
 * @brief Unregister event callbacks for light sleep internal events (if CONFIG_FREERTOS_USE_TICKLESS_IDLE)
 * @param event_id      Designed to unregister the corresponding event_cb in g_sleep_event_cbs_config
 * @param event_cb_conf Config struct containing event callback function and corresponding argument
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if the input parameter cb is NULL or event_id is out of range
 */
esp_err_t esp_sleep_unregister_event_callback(esp_sleep_event_cb_index_t event_id, esp_sleep_event_cb_t cb);

/**
 * @brief Designed to execute functions in the esp_sleep_event_cb_config_t linked list
 *
 * @param event_id   Designed to annotate the corresponding event_cb in g_sleep_event_cbs_config
 * @param ext_arg    Designed to pass external parameters
 * @return None
 */
void esp_sleep_execute_event_callbacks(esp_sleep_event_cb_index_t event_id, void *ext_arg);

#ifdef __cplusplus
}
#endif
