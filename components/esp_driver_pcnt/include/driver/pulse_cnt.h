/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/pcnt_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of PCNT unit handle
 */
typedef struct pcnt_unit_t *pcnt_unit_handle_t;

/**
 * @brief Type of PCNT channel handle
 */
typedef struct pcnt_chan_t *pcnt_channel_handle_t;

/**
 * @brief PCNT watch event data
 */
typedef struct {
    int watch_point_value;                       /*!< Watch point value that triggered the event */
    pcnt_unit_zero_cross_mode_t zero_cross_mode; /*!< Zero cross mode */
} pcnt_watch_event_data_t;

/**
 * @brief PCNT watch event callback prototype
 *
 * @note The callback function is invoked from an ISR context, so it should meet the restrictions of not calling any blocking APIs when implementing the callback.
 *       e.g. must use ISR version of FreeRTOS APIs.
 *
 * @param[in] unit PCNT unit handle
 * @param[in] edata PCNT event data, fed by the driver
 * @param[in] user_ctx User data, passed from `pcnt_unit_register_event_callbacks()`
 * @return Whether a high priority task has been woken up by this function
 */
typedef bool (*pcnt_watch_cb_t)(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx);

/**
 * @brief Group of supported PCNT callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_PCNT_ISR_IRAM_SAFE is enabled, the callback itself and functions callbed by it should be placed in IRAM.
 */
typedef struct {
    pcnt_watch_cb_t on_reach; /*!< Called when PCNT unit counter reaches any watch point */
} pcnt_event_callbacks_t;

/**
 * @brief PCNT unit configuration
 */
typedef struct {
    int low_limit;      /*!< Low limitation of the count unit, should be lower than 0 */
    int high_limit;     /*!< High limitation of the count unit, should be higher than 0 */
    int intr_priority;  /*!< PCNT interrupt priority,
                            if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3) */
    struct {
        uint32_t accum_count: 1; /*!< Whether to accumulate the count value when overflows at the high/low limit */
    } flags;       /*!< Extra flags */
} pcnt_unit_config_t;

/**
 * @brief PCNT channel configuration
 */
typedef struct {
    int edge_gpio_num;  /*!< GPIO number used by the edge signal, input mode with pull up enabled. Set to -1 if unused */
    int level_gpio_num; /*!< GPIO number used by the level signal, input mode with pull up enabled. Set to -1 if unused */
    struct {
        uint32_t invert_edge_input: 1;   /*!< Invert the input edge signal */
        uint32_t invert_level_input: 1;  /*!< Invert the input level signal */
        uint32_t virt_edge_io_level: 1;  /*!< Virtual edge IO level, 0: low, 1: high. Only valid when edge_gpio_num is set to -1 */
        uint32_t virt_level_io_level: 1; /*!< Virtual level IO level, 0: low, 1: high. Only valid when level_gpio_num is set to -1 */
        uint32_t io_loop_back: 1;        /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
    } flags;                             /*!< Channel config flags */
} pcnt_chan_config_t;

/**
 * @brief PCNT glitch filter configuration
 */
typedef struct {
    uint32_t max_glitch_ns; /*!< Pulse width smaller than this threshold will be treated as glitch and ignored, in the unit of ns */
} pcnt_glitch_filter_config_t;

/**
 * @brief Create a new PCNT unit, and return the handle
 *
 * @note The newly created PCNT unit is put in the init state.
 *
 * @param[in] config PCNT unit configuration
 * @param[out] ret_unit Returned PCNT unit handle
 * @return
 *      - ESP_OK: Create PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Create PCNT unit failed because of invalid argument (e.g. high/low limit value out of the range)
 *      - ESP_ERR_NO_MEM: Create PCNT unit failed because out of memory
 *      - ESP_ERR_NOT_FOUND: Create PCNT unit failed because all PCNT units are used up and no more free one
 *      - ESP_FAIL: Create PCNT unit failed because of other error
 */
esp_err_t pcnt_new_unit(const pcnt_unit_config_t *config, pcnt_unit_handle_t *ret_unit);

/**
 * @brief Delete the PCNT unit handle
 *
 * @note A PCNT unit can't be in the enable state when this function is invoked.
 *       See also `pcnt_unit_disable()` for how to disable a unit.
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Delete the PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Delete the PCNT unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Delete the PCNT unit failed because the unit is not in init state or some PCNT channel is still in working
 *      - ESP_FAIL: Delete the PCNT unit failed because of other error
 */
esp_err_t pcnt_del_unit(pcnt_unit_handle_t unit);

/**
 * @brief Set glitch filter for PCNT unit
 *
 * @note The glitch filter module is clocked from APB, and APB frequency can be changed during DFS, which in return make the filter out of action.
 *       So this function will lazy-install a PM lock internally when the power management is enabled. With this lock, the APB frequency won't be changed.
 *       The PM lock can be uninstalled in `pcnt_del_unit()`.
 * @note This function should be called when the PCNT unit is in the init state (i.e. before calling `pcnt_unit_enable()`)
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] config PCNT filter configuration, set config to NULL means disabling the filter function
 * @return
 *      - ESP_OK: Set glitch filter successfully
 *      - ESP_ERR_INVALID_ARG: Set glitch filter failed because of invalid argument (e.g. glitch width is too big)
 *      - ESP_ERR_INVALID_STATE: Set glitch filter failed because the unit is not in the init state
 *      - ESP_FAIL: Set glitch filter failed because of other error
 */
esp_err_t pcnt_unit_set_glitch_filter(pcnt_unit_handle_t unit, const pcnt_glitch_filter_config_t *config);

#if SOC_PCNT_SUPPORT_CLEAR_SIGNAL
/**
 * @brief PCNT clear signal configuration
 */
typedef struct {
    int clear_signal_gpio_num;  /*!< GPIO number used by the clear signal, the default active level is high, input mode with pull down enabled */
    struct {
        uint32_t invert_clear_signal: 1;   /*!< Invert the clear input signal and set input mode with pull up */
        uint32_t io_loop_back: 1;         /*!< For debug/test, the signal output from the GPIO will be fed to the input path as well */
    } flags;                              /*!< clear signal config flags */
} pcnt_clear_signal_config_t;

/**
 * @brief Set clear signal for PCNT unit
 *
 * @note The function of clear signal is the same as `pcnt_unit_clear_count()`. High-level Active
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] config PCNT clear signal configuration, set config to NULL means disabling the clear signal
 * @return
 *      - ESP_OK: Set clear signal successfully
 *      - ESP_ERR_INVALID_ARG: Set clear signal failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set clear signal failed because set clear signal repeatedly or disable clear signal before set it
 *      - ESP_FAIL: Set clear signal failed because of other error
 */
esp_err_t pcnt_unit_set_clear_signal(pcnt_unit_handle_t unit, const pcnt_clear_signal_config_t *config);
#endif

/**
 * @brief Enable the PCNT unit
 *
 * @note This function will transit the unit state from init to enable.
 * @note This function will enable the interrupt service, if it's lazy installed in `pcnt_unit_register_event_callbacks()`.
 * @note This function will acquire the PM lock if it's lazy installed in `pcnt_unit_set_glitch_filter()`.
 * @note Enable a PCNT unit doesn't mean to start it. See also `pcnt_unit_start()` for how to start the PCNT counter.
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Enable PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Enable PCNT unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Enable PCNT unit failed because the unit is already enabled
 *      - ESP_FAIL: Enable PCNT unit failed because of other error
 */
esp_err_t pcnt_unit_enable(pcnt_unit_handle_t unit);

/**
 * @brief Disable the PCNT unit
 *
 * @note This function will do the opposite work to the `pcnt_unit_enable()`
 * @note Disable a PCNT unit doesn't mean to stop it. See also `pcnt_unit_stop()` for how to stop the PCNT counter.
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Disable PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Disable PCNT unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Disable PCNT unit failed because the unit is not enabled yet
 *      - ESP_FAIL: Disable PCNT unit failed because of other error
 */
esp_err_t pcnt_unit_disable(pcnt_unit_handle_t unit);

/**
 * @brief Start the PCNT unit, the counter will start to count according to the edge and/or level input signals
 *
 * @note This function should be called when the unit is in the enable state (i.e. after calling `pcnt_unit_enable()`)
 * @note This function is allowed to run within ISR context
 * @note This function will be placed into IRAM if `CONFIG_PCNT_CTRL_FUNC_IN_IRAM` is on, so that it's allowed to be executed when Cache is disabled
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Start PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Start PCNT unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Start PCNT unit failed because the unit is not enabled yet
 *      - ESP_FAIL: Start PCNT unit failed because of other error
 */
esp_err_t pcnt_unit_start(pcnt_unit_handle_t unit);

/**
 * @brief Stop PCNT from counting
 *
 * @note This function should be called when the unit is in the enable state (i.e. after calling `pcnt_unit_enable()`)
 * @note The stop operation won't clear the counter. Also see `pcnt_unit_clear_count()` for how to clear pulse count value.
 * @note This function is allowed to run within ISR context
 * @note This function will be placed into IRAM if `CONFIG_PCNT_CTRL_FUNC_IN_IRAM`, so that it is allowed to be executed when Cache is disabled
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Stop PCNT unit successfully
 *      - ESP_ERR_INVALID_ARG: Stop PCNT unit failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Stop PCNT unit failed because the unit is not enabled yet
 *      - ESP_FAIL: Stop PCNT unit failed because of other error
 */
esp_err_t pcnt_unit_stop(pcnt_unit_handle_t unit);

/**
 * @brief Clear PCNT pulse count value to zero
 *
 * @note It's recommended to call this function after adding a watch point by `pcnt_unit_add_watch_point()`, so that the newly added watch point is effective immediately.
 * @note This function is allowed to run within ISR context
 * @note This function will be placed into IRAM if `CONFIG_PCNT_CTRL_FUNC_IN_IRAM`, so that it's allowed to be executed when Cache is disabled
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @return
 *      - ESP_OK: Clear PCNT pulse count successfully
 *      - ESP_ERR_INVALID_ARG: Clear PCNT pulse count failed because of invalid argument
 *      - ESP_FAIL: Clear PCNT pulse count failed because of other error
 */
esp_err_t pcnt_unit_clear_count(pcnt_unit_handle_t unit);

/**
 * @brief Get PCNT count value
 *
 * @note This function is allowed to run within ISR context
 * @note This function will be placed into IRAM if `CONFIG_PCNT_CTRL_FUNC_IN_IRAM`, so that it's allowed to be executed when Cache is disabled
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[out] value Returned count value
 * @return
 *      - ESP_OK: Get PCNT pulse count successfully
 *      - ESP_ERR_INVALID_ARG: Get PCNT pulse count failed because of invalid argument
 *      - ESP_FAIL: Get PCNT pulse count failed because of other error
 */
esp_err_t pcnt_unit_get_count(pcnt_unit_handle_t unit, int *value);

/**
 * @brief Set event callbacks for PCNT unit
 *
 * @note User registered callbacks are expected to be runnable within ISR context
 * @note The first call to this function needs to be before the call to `pcnt_unit_enable`
 * @note User can deregister a previously registered callback by calling this function and setting the callback member in the `cbs` structure to NULL.
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] cbs Group of callback functions
 * @param[in] user_data User data, which will be passed to callback functions directly
 * @return
 *      - ESP_OK: Set event callbacks successfully
 *      - ESP_ERR_INVALID_ARG: Set event callbacks failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Set event callbacks failed because the unit is not in init state
 *      - ESP_FAIL: Set event callbacks failed because of other error
 */
esp_err_t pcnt_unit_register_event_callbacks(pcnt_unit_handle_t unit, const pcnt_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Add a watch point for PCNT unit, PCNT will generate an event when the counter value reaches the watch point value
 *
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] watch_point Value to be watched
 * @return
 *      - ESP_OK: Add watch point successfully
 *      - ESP_ERR_INVALID_ARG: Add watch point failed because of invalid argument (e.g. the value to be watched is out of the limitation set in `pcnt_unit_config_t`)
 *      - ESP_ERR_INVALID_STATE: Add watch point failed because the same watch point has already been added
 *      - ESP_ERR_NOT_FOUND: Add watch point failed because no more hardware watch point can be configured
 *      - ESP_FAIL: Add watch point failed because of other error
 */
esp_err_t pcnt_unit_add_watch_point(pcnt_unit_handle_t unit, int watch_point);

/**
 * @brief Remove a watch point for PCNT unit
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] watch_point Watch point value
 * @return
 *      - ESP_OK: Remove watch point successfully
 *      - ESP_ERR_INVALID_ARG: Remove watch point failed because of invalid argument
 *      - ESP_ERR_INVALID_STATE: Remove watch point failed because the watch point was not added by `pcnt_unit_add_watch_point()` yet
 *      - ESP_FAIL: Remove watch point failed because of other error
 */
esp_err_t pcnt_unit_remove_watch_point(pcnt_unit_handle_t unit, int watch_point);

/**
 * @brief Create PCNT channel for specific unit, each PCNT has several channels associated with it
 *
 * @note This function should be called when the unit is in init state (i.e. before calling `pcnt_unit_enable()`)
 *
 * @param[in] unit PCNT unit handle created by `pcnt_new_unit()`
 * @param[in] config PCNT channel configuration
 * @param[out] ret_chan Returned channel handle
 * @return
 *      - ESP_OK: Create PCNT channel successfully
 *      - ESP_ERR_INVALID_ARG: Create PCNT channel failed because of invalid argument
 *      - ESP_ERR_NO_MEM: Create PCNT channel failed because of insufficient memory
 *      - ESP_ERR_NOT_FOUND: Create PCNT channel failed because all PCNT channels are used up and no more free one
 *      - ESP_ERR_INVALID_STATE: Create PCNT channel failed because the unit is not in the init state
 *      - ESP_FAIL: Create PCNT channel failed because of other error
 */
esp_err_t pcnt_new_channel(pcnt_unit_handle_t unit, const pcnt_chan_config_t *config, pcnt_channel_handle_t *ret_chan);

/**
 * @brief Delete the PCNT channel
 *
 * @param[in] chan PCNT channel handle created by `pcnt_new_channel()`
 * @return
 *      - ESP_OK: Delete the PCNT channel successfully
 *      - ESP_ERR_INVALID_ARG: Delete the PCNT channel failed because of invalid argument
 *      - ESP_FAIL: Delete the PCNT channel failed because of other error
 */
esp_err_t pcnt_del_channel(pcnt_channel_handle_t chan);

/**
 * @brief Set channel actions when edge signal changes (e.g. falling or rising edge occurred).
 *        The edge signal is input from the `edge_gpio_num` configured in `pcnt_chan_config_t`.
 *        We use these actions to control when and how to change the counter value.
 *
 * @param[in] chan PCNT channel handle created by `pcnt_new_channel()`
 * @param[in] pos_act Action on posedge signal
 * @param[in] neg_act Action on negedge signal
 * @return
 *      - ESP_OK: Set edge action for PCNT channel successfully
 *      - ESP_ERR_INVALID_ARG: Set edge action for PCNT channel failed because of invalid argument
 *      - ESP_FAIL: Set edge action for PCNT channel failed because of other error
 */
esp_err_t pcnt_channel_set_edge_action(pcnt_channel_handle_t chan, pcnt_channel_edge_action_t pos_act, pcnt_channel_edge_action_t neg_act);

/**
 * @brief Set channel actions when level signal changes (e.g. signal level goes from high to low).
 *        The level signal is input from the `level_gpio_num` configured in `pcnt_chan_config_t`.
 *        We use these actions to control when and how to change the counting mode.
 *
 * @param[in] chan PCNT channel handle created by `pcnt_new_channel()`
 * @param[in] high_act Action on high level signal
 * @param[in] low_act Action on low level signal
 * @return
 *      - ESP_OK: Set level action for PCNT channel successfully
 *      - ESP_ERR_INVALID_ARG: Set level action for PCNT channel failed because of invalid argument
 *      - ESP_FAIL: Set level action for PCNT channel failed because of other error
 */
esp_err_t pcnt_channel_set_level_action(pcnt_channel_handle_t chan, pcnt_channel_level_action_t high_act, pcnt_channel_level_action_t low_act);

#ifdef __cplusplus
}
#endif
