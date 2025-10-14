/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "esp_err.h"
#include "soc/gpio_num.h"
#include "driver/sd_types.h"
#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Configure SD Host slot
 *
 * @param[in] slot        SD Host slot handle
 * @param[in] config      SD Host slot configuration
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_configure(sd_host_slot_handle_t slot, const sd_host_slot_cfg_t *config);

/**
 * @brief Do a transaction for the slot
 *
 * @param[in] slot        SD Host slot handle
 * @param[in] cmdinfo     SD command info, see `sdmmc_command_t`
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_do_transaction(sd_host_slot_handle_t slot, sdmmc_command_t *cmdinfo);

/**
 * @brief Register SD event callbacks
 *
 * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
 *       the `cbs` structure to NULL.
 *
 * @param[in] slot         SD Host slot handle
 * @param[in] cbs          Group of callback functions
 * @param[in] user_data    User data, which will be delivered to the callback functions directly
 *
 * @return
 *        - ESP_OK:                On success
 *        - ESP_ERR_INVALID_ARG:   Invalid arguments
 *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
 */
esp_err_t sd_host_slot_register_event_callbacks(sd_host_slot_handle_t slot, const sd_host_evt_cbs_t *cbs, void *user_data);

/**
 * @brief Remove an SD Host slot
 *
 * @param[in] slot        SD Host slot handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_STATE:  Invalid state, slot is not available
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_remove_slot(sd_host_slot_handle_t slot);

/**
 * @brief Set an SD Host slot clock always on
 *
 * @param[in] slot        SD Host slot handle
 * @param[in] always_on   Always on or not
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_set_cclk_always_on(sd_host_slot_handle_t slot, bool always_on);

/**
 * @brief Enable an SD Host slot IO interrupt
 *
 * @param[in] slot        SD Host slot handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_enable_io_int(sd_host_slot_handle_t slot);

/**
 * @brief Wait for IO interrupt event
 *
 * @param[in] slot           SD Host slot handle
 * @param[in] timeout_ticks  Timeout ticks
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_TIMEOUT:        Timeout
 */
esp_err_t sd_host_slot_wait_io_int(sd_host_slot_handle_t slot, TickType_t timeout_ticks);

/**
 * @brief Get slot info
 *
 * @param[in]  slot           SD Host slot handle
 * @param[out] info           SD slot info
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 */
esp_err_t sd_host_slot_get_info(sd_host_slot_handle_t slot, sd_host_slot_info_t *info);

/**
 * @brief Delete an SD Host controller
 *
 * @param[in]  ctlr        SD Host controller handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_INVALID_STATE:  Invalid state, there's still registered slot(s)
 */
esp_err_t sd_host_del_controller(sd_host_ctlr_handle_t ctlr);

#ifdef __cplusplus
}
#endif
