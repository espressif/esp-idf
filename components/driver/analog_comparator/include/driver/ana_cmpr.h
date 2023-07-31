/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/ana_cmpr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Analog comparator unit configuration
 *
 */
typedef struct {
    ana_cmpr_unit_t         unit;               /*!< Analog comparator unit */
    ana_cmpr_clk_src_t      clk_src;            /*!< The clock source of the analog comparator,
                                                 *   which decide the resolution of the comparator
                                                 */
    ana_cmpr_ref_source_t   ref_src;            /*!< Reference signal source of the comparator,
                                                 *   select using ANA_CMPR_REF_SRC_INTERNAL or ANA_CMPR_REF_SRC_EXTERNAL.
                                                 *   For internal reference, the reference voltage should be set to `internal_ref_volt`,
                                                 *   for external reference, the reference signal should be connect to `ANA_CMPRx_EXT_REF_GPIO`
                                                 */
    ana_cmpr_cross_type_t   cross_type;         /*!< The crossing types that can trigger interrupt */
    int                     intr_priority;      /*!< The interrupt priority, range 0~7, if set to 0, the driver will try to allocate an interrupt with a relative low priority (1,2,3)
                                                 *   otherwise the larger the higher, 7 is NMI */
    struct {
        uint32_t            io_loop_back:1;     /*!< Enable this field when the other signals that output on the comparision pins are supposed to be fed back.
                                                 *   Normally used for debug/test scenario */
    } flags;                                    /*!< Analog comparator driver flags */
} ana_cmpr_config_t;

/**
 * @brief Analog comparator internal reference configuration
 *
 */
typedef struct {
    ana_cmpr_ref_voltage_t  ref_volt;           /*!< The internal reference voltage. It can be specified to a certain fixed percentage of
                                                 *   the VDD power supply, currently supports 0%~70% VDD with a step 10%
                                                 */
} ana_cmpr_internal_ref_config_t;

/**
 * @brief Analog comparator debounce filter configuration
 *
 */
typedef struct {
    uint32_t                   wait_us;         /*!< The wait time of re-enabling the interrupt after the last triggering,
                                                 *   it is used to avoid the spurious triggering while the source signal crossing the reference signal.
                                                 *   The value should regarding how fast the source signal changes, e.g., a rapid signal requires
                                                 *   a small wait time, otherwise the next crosses may be missed.
                                                 *   (Unit: micro second)
                                                 */
} ana_cmpr_debounce_config_t;

/**
 * @brief Group of Analog Comparator callbacks
 * @note The callbacks are all running under ISR environment
 * @note When CONFIG_ANA_CMPR_ISR_IRAM_SAFE is enabled, the callback itself and functions called by it should be placed in IRAM.
 *       The variables used in the function should be in the SRAM as well.
 */
typedef struct {
    ana_cmpr_cross_cb_t on_cross;               /*!< The callback function on cross interrupt */
} ana_cmpr_event_callbacks_t;

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
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @return
 *      - ESP_OK                Delete analog comparator unit handle success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong unit number
 *      - ESP_ERR_INVALID_STATE The analog comparator is not disabled yet
 */
esp_err_t ana_cmpr_del_unit(ana_cmpr_handle_t cmpr);

/**
 * @brief Set internal reference configuration
 * @note  This function only need to be called when `ana_cmpr_config_t::ref_src`
 *        is ANA_CMPR_REF_SRC_INTERNAL.
 * @note This function is allowed to run within ISR context including intr callbacks
 * @note This function will be placed into IRAM if `CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` is on,
 *       so that it's allowed to be executed when Cache is disabled
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  ref_cfg      Internal reference configuration
 * @return
 *      - ESP_OK                Set denounce configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 *      - ESP_ERR_INVALID_STATE The reference source is not `ANA_CMPR_REF_SRC_INTERNAL`
 */
esp_err_t ana_cmpr_set_internal_reference(ana_cmpr_handle_t cmpr, const ana_cmpr_internal_ref_config_t *ref_cfg);

/**
 * @brief Set debounce configuration to the analog comparator
 * @note This function is allowed to run within ISR context including intr callbacks
 * @note This function will be placed into IRAM if `CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` is on,
 *       so that it's allowed to be executed when Cache is disabled
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  dbc_cfg      Debounce configuration
 * @return
 *      - ESP_OK                Set denounce configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 */
esp_err_t ana_cmpr_set_debounce(ana_cmpr_handle_t cmpr, const ana_cmpr_debounce_config_t *dbc_cfg);

/**
 * @brief Set the source signal cross type
 * @note The initial cross type is configured in `ana_cmpr_new_unit`, this function can update the cross type
 * @note This function is allowed to run within ISR context including intr callbacks
 * @note This function will be placed into IRAM if `CONFIG_ANA_CMPR_CTRL_FUNC_IN_IRAM` is on,
 *       so that it's allowed to be executed when Cache is disabled
 *
 * @param[in]  cmpr         The handle of analog comparator unit
 * @param[in]  cross_type   The source signal cross type that can trigger the interrupt
 * @return
 *      - ESP_OK                Set denounce configuration success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters
 */
esp_err_t ana_cmpr_set_cross_type(ana_cmpr_handle_t cmpr, ana_cmpr_cross_type_t cross_type);

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
 * @brief Get the specific GPIO number of the analog comparator unit
 *
 * @param[in]  unit         The handle of analog comparator unit
 * @param[in]  chan_type    The channel type of analog comparator, like source channel or reference channel
 * @param[out] gpio_num     The output GPIO number of this channel
 * @return
 *      - ESP_OK                Get GPIO success
 *      - ESP_ERR_INVALID_ARG   NULL pointer of the parameters or wrong unit number or wrong channel type
 */
esp_err_t ana_cmpr_get_gpio(ana_cmpr_unit_t unit, ana_cmpr_channel_type_t chan_type, int *gpio_num);

#ifdef __cplusplus
}
#endif
