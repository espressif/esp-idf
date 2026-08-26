/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/ana_cmpr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Analog comparator unit configuration
 */
typedef struct {
    int                     unit;               /*!< Analog comparator unit ID, index from 0 */
    ana_cmpr_clk_src_t      clk_src;            /*!< The clock source of the analog comparator,
                                                 *   which decide the resolution of the comparator
                                                 */
    ana_cmpr_ref_source_t   ref_src;            /*!< Reference signal source of the comparator,
                                                 *   select using ANA_CMPR_REF_SRC_INTERNAL or ANA_CMPR_REF_SRC_EXTERNAL.
                                                 *   For internal reference, the reference voltage should be set to `internal_ref_volt`,
                                                 *   for external reference, the reference signal should be connect to `ANA_CMPRx_EXT_REF_GPIO`
                                                 */
    int                     intr_priority;      /*!< The interrupt priority, range 1~3.
                                                If set to 0, the driver will automatically select a relative low priority (1,2,3) */
    ana_cmpr_cross_type_t   cross_type;         /*!< The crossing type of source channel 0, that can trigger interrupt */
    gpio_num_t              src_chan0_gpio;     /*!< The GPIO number of source channel 0 signal */
    gpio_num_t              ext_ref_gpio;       /*!< The GPIO number of external reference signal, only valid when `ref_src` is set to `ANA_CMPR_REF_SRC_EXTERNAL` */
    uint8_t                 resample_limit;     /*!< Unit-wide consecutive sample count required to update channel status */
    bool                    en_capture_timer;   /*!< Enable channel capture timer at unit creation. */
} ana_cmpr_config_t;

/**
 * @brief Allocating a new analog comparator unit handle
 *
 * @param[in]  config       The config of the analog comparator unit
 * @param[out] ret_cmpr     The returned analog comparator unit handle
 * @return
 *      - ESP_OK                Allocate analog comparator unit handle success
 *      - ESP_ERR_NO_MEM        No memory for the analog comparator structure
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong unit number
 *      - ESP_ERR_INVALID_STATE The unit has been allocated or the clock source has been occupied
 */
esp_err_t ana_cmpr_new_unit(const ana_cmpr_config_t *config, ana_cmpr_handle_t *ret_cmpr);

/**
 * @brief Delete the analog comparator unit handle
 * @note Caller must ensure no active users remain before deleting the unit handle, including any
 *       ETM event/task handles created from this unit (delete them first via `esp_etm_del_event()`
 *       and `esp_etm_del_task()`).
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @return
 *      - ESP_OK                Delete analog comparator unit handle success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong unit number
 *      - ESP_ERR_INVALID_STATE The analog comparator is not disabled yet
 */
esp_err_t ana_cmpr_del_unit(ana_cmpr_handle_t cmpr);

/**
 * @brief Analog comparator internal reference configuration
 */
typedef struct {
    ana_cmpr_ref_voltage_t  ref_volt;           /*!< The internal reference voltage. It can be specified to a certain fixed percentage of
                                                 *   the VDD power supply, currently supports 0%~70% VDD with a step 10% */
    ana_cmpr_ref_hys_t      ref_hys_level;      /*!< Internal reference hysteresis level */
} ana_cmpr_internal_ref_config_t;

/**
 * @brief Set internal reference configuration
 * @note This function only need to be called when `ana_cmpr_config_t::ref_src` is set to `ANA_CMPR_REF_SRC_INTERNAL`.
 * @note This function is allowed to run within ISR context including interrupt callbacks
 * @note This function will be placed into IRAM if `CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` is on,
 *       so that it's allowed to be executed when Cache is disabled
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  ref_cfg      Internal reference configuration
 * @return
 *      - ESP_OK                Set internal reference configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_NOT_ALLOWED   Set the reference voltage for external reference channel is not allowed
 */
esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg);

/**
 * @brief Analog comparator debounce filter configuration
 */
typedef struct {
    uint32_t    wait_us;    /*!< The wait time to prevent frequent interrupts caused by signal noise or bouncing.
                                 During the specified wait_us period, no new interrupts will be triggered.
                                 Set the value according to the signal characteristics. A rapid signal requires a small wait time,
                                 otherwise the next cross event may be missed. */
} ana_cmpr_debounce_config_t;

/**
 * @brief Set debounce configuration to the analog comparator
 * @note This function is allowed to run within ISR context including interrupt callbacks
 * @note This function will be placed into IRAM if `CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` is on,
 *       so that it's allowed to be executed when Cache is disabled
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  dbc_cfg      Debounce configuration
 * @return
 *      - ESP_OK                Set debounce configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 */
esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg);

/**
 * @brief Get capture timestamps of a source channel
 *
 * @note Returned timestamp values are measured in PAD_COMP_CLK cycles
 * @note Convert ticks to time by first querying comparator clock resolution with
 *       `ana_cmpr_get_clock_resolution_hz()`.
 *       For example, `time_us = ticks * 1000000 / resolution_hz`.
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  src_chan_id  The source channel index
 * @param[out] current      Timestamp of the latest cross event, in PAD_COMP_CLK cycles.
 *                          Can be NULL if not needed.
 * @param[out] previous     Timestamp of the previous cross event, in PAD_COMP_CLK cycles.
 *                          Can be NULL if not needed.
 * @return
 *      - ESP_OK                Get capture timestamps success
 *      - ESP_ERR_INVALID_ARG   Both output pointers are NULL, or invalid channel id
 *      - ESP_ERR_NOT_SUPPORTED Hardware doesn't support capture timer
 */
esp_err_t ana_cmpr_get_capture_timestamps(ana_cmpr_handle_t cmpr, int src_chan_id, uint32_t *current, uint32_t *previous);

/**
 * @brief Get clock resolution of the analog comparator unit
 *
 * @note Returned value is in Hz and corresponds to the PAD_COMP_CLK domain.
 *       This clock is shared by features like debounce filtering and capture timestamps.
 *
 * @param[in]  cmpr             The handle of analog comparator unit
 * @param[out] resolution_hz    Comparator clock resolution in Hz
 * @return
 *      - ESP_OK                Get comparator clock resolution success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 */
esp_err_t ana_cmpr_get_clock_resolution_hz(ana_cmpr_handle_t cmpr, uint32_t *resolution_hz);

/**
 * @brief Analog comparator source channel configuration
 */
typedef struct {
    gpio_num_t              gpio_num;           /*!< Source input GPIO */
    ana_cmpr_cross_type_t   cross_type;         /*!< Crossing type that can trigger events for this source channel */
} ana_cmpr_src_chan_config_t;

/**
 * @brief Add or update a source channel
 * @note This function can only be called when the comparator unit is in init (disabled) state
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  src_chan_id  The source channel index
 * @param[in]  src_cfg      Source channel configuration
 * @return
 *      - ESP_OK                Add/update source channel success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or invalid channel/GPIO configuration
 *      - ESP_ERR_INVALID_STATE The analog comparator is not in init state
 *      - ESP_ERR_NOT_SUPPORTED Source channel index is not supported by the current target
 */
esp_err_t ana_cmpr_add_src_chan(ana_cmpr_handle_t cmpr, int src_chan_id, const ana_cmpr_src_chan_config_t *src_cfg);

/**
 * @brief Remove a source channel from scan and interrupt routing
 * @note This function can only be called when the comparator unit is in init (disabled) state
 * @note This API is idempotent: removing an already-removed channel still returns ESP_OK
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  src_chan_id  The source channel index
 * @return
 *      - ESP_OK                Remove source channel success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or invalid channel id
 *      - ESP_ERR_INVALID_STATE The analog comparator is not in init state
 *      - ESP_ERR_NOT_SUPPORTED Source channel index is not supported by the current target
 */
esp_err_t ana_cmpr_remove_src_chan(ana_cmpr_handle_t cmpr, int src_chan_id);

/**
 * @brief Set cross type for the given source channel
 * @note On targets with edge-type interrupt support, interrupt source selection is fixed when
 *       adding source channels, so runtime cross-type switching is not supported.
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  src_chan_id  The source channel index
 * @param[in]  cross_type   The source signal cross type that can trigger the interrupt
 * @return
 *      - ESP_OK                Set cross type configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or invalid channel/cross type
 *      - ESP_ERR_INVALID_STATE The target source channel isn't configured
 *      - ESP_ERR_NOT_SUPPORTED Runtime cross-type switching isn't supported by the current target
 */
esp_err_t ana_cmpr_set_src_chan_cross_type(ana_cmpr_handle_t cmpr, int src_chan_id, ana_cmpr_cross_type_t cross_type);

/**
 * @brief Analog comparator scan configuration
 */
typedef struct {
    ana_cmpr_scan_mode_t     scan_mode;      /*!< Channel scan mode */
    uint32_t                 poll_period_us; /*!< Channel switching wait time in microseconds */
} ana_cmpr_scan_config_t;

/**
 * @brief Set scan configuration
 * @note This function can only be called when the comparator unit is in init (disabled) state
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  scan_cfg     Scan configuration
 * @return
 *      - ESP_OK                Set scan configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE The analog comparator is not in init state
 *      - ESP_ERR_NOT_SUPPORTED Scan configuration is not supported by current target
 */
esp_err_t ana_cmpr_set_scan_config(ana_cmpr_handle_t cmpr, const ana_cmpr_scan_config_t *scan_cfg);

/**
 * @brief Trigger one analog comparator scan sequence
 *
 * @param[in]  cmpr             The handle of analog comparator unit
 * @return
 *      - ESP_OK                Trigger scan success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE Invalid unit state for triggering scan
 *      - ESP_ERR_NOT_SUPPORTED The hardware doesn't support software triggered scan
 */
esp_err_t ana_cmpr_trigger_scan(ana_cmpr_handle_t cmpr);

/**
 * @brief Get the output level of a source channel
 * @note The output level indicates whether the source voltage is higher than the reference voltage
 * @note This function is allowed to run within ISR context including interrupt callbacks
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  src_chan_id  The source channel index
 * @param[out] out_level   The output level of the source channel
 *                       - true:  source voltage > reference voltage
 *                       - false: source voltage < reference voltage
 * @return
 *      - ESP_OK                Get output level success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or invalid channel id
 *      - ESP_ERR_NOT_SUPPORTED Hardware can't report the output level of the source channel or the source channel index is not supported by the current target
 */
esp_err_t ana_cmpr_get_output_level(ana_cmpr_handle_t cmpr, int src_chan_id, bool *out_level);

/**
 * @brief Register analog comparator interrupt event callbacks
 * @note  This function can only be called before enabling the unit
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  cbs          Group of callback functions
 * @param[in]  user_data    The user data that will be passed to callback functions directly
 * @return
 *      - ESP_OK                Register callbacks success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE The analog comparator has been enabled
 */
esp_err_t ana_cmpr_register_event_callbacks(ana_cmpr_handle_t cmpr, const ana_cmpr_event_callbacks_t *cbs, void *user_data);

/**
 * @brief Enable the analog comparator unit
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @return
 *      - ESP_OK                Enable analog comparator unit success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE The analog comparator has been enabled
 */
esp_err_t ana_cmpr_enable(ana_cmpr_handle_t cmpr);

/**
 * @brief Disable the analog comparator unit
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @return
 *      - ESP_OK                Disable analog comparator unit success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE The analog comparator has disabled already
 */
esp_err_t ana_cmpr_disable(ana_cmpr_handle_t cmpr);

/**
 * @brief Get the GPIO number of a configured analog comparator channel
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  chan_type    The channel type of analog comparator, either source channel or external reference channel
 * @param[in]  chan_id      The source channel index when `chan_type` is `ANA_CMPR_SOURCE_CHAN`.
 *                          Must be 0 when `chan_type` is `ANA_CMPR_EXT_REF_CHAN`
 * @param[out] gpio_num     The output GPIO number of this channel
 * @return
 *      - ESP_OK                Get GPIO success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong channel type/channel id
 *      - ESP_ERR_INVALID_STATE The target channel is not configured
 *      - ESP_ERR_NOT_SUPPORTED The source channel index is not supported by the current target
 *      - ESP_ERR_NOT_FOUND     The requested channel doesn't have a GPIO mapping, e.g. the reference source is internal
 */
esp_err_t ana_cmpr_get_channel_gpio(ana_cmpr_handle_t cmpr, ana_cmpr_channel_type_t chan_type, int chan_id, gpio_num_t *gpio_num);

/**
 * @brief Set the cross type for the source channel 0 that can trigger the event
 * @note The initial cross type is configured in `ana_cmpr_new_unit`, this function can update the cross type
 * @note This function is allowed to run within ISR context including interrupt callbacks
 * @note This is a legacy API that only applies to source channel 0
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  cross_type   The source signal cross type that can trigger the interrupt
 * @return
 *      - ESP_OK                Set cross type configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 */
__attribute__((always_inline))
static inline esp_err_t ana_cmpr_set_cross_type(ana_cmpr_handle_t cmpr, ana_cmpr_cross_type_t cross_type)
{
    return ana_cmpr_set_src_chan_cross_type(cmpr, 0, cross_type);
}

/**
 * @brief Get the fixed GPIO number of the analog comparator unit
 *
 * @deprecated Please use `ana_cmpr_get_channel_gpio()` instead to query the GPIO of the configured comparator instance
 *
 * @param[in]  unit         The handle of analog comparator unit
 * @param[in]  chan_type    The channel type of analog comparator, like source channel or reference channel
 * @param[out] gpio_num     The output GPIO number of this channel
 * @return
 *      - ESP_OK                Get GPIO success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong unit number or wrong channel type
 */
esp_err_t ana_cmpr_get_gpio(ana_cmpr_unit_t unit, ana_cmpr_channel_type_t chan_type, int *gpio_num)
__attribute__((deprecated("Please use ana_cmpr_get_channel_gpio() instead")));

#ifdef __cplusplus
}
#endif
