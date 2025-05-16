/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "driver/sd_types.h"
#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sd_host_driver_t sd_host_driver_t;    /*!< Type of SD driver host context */
typedef struct sd_slot_driver_t sd_slot_driver_t;    /*!< Type of SD driver slot context */

/**
 * @brief SD driver host driver context
 */
struct sd_host_driver_t {
    /**
     * @brief Delete an SD Host controller
     *
     * @param[in]  ctlr_ctx    SD driver host controller context
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     *        - ESP_ERR_INVALID_STATE:  Invalid state, there's still registered slot(s)
     */
    esp_err_t (*del_ctlr)(sd_host_driver_t *ctlr_ctx);
};

/**
 * @brief SD driver API definition
 */
struct sd_slot_driver_t {
    /**
     * @brief Configure SD Host slot
     *
     * @param[in] slot_drv    SD Host slot driver handle
     * @param[in] config      SD Host slot configuration
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     */
    esp_err_t (*configure)(sd_slot_driver_t *slot_drv, const sd_host_slot_cfg_t *config);

    /**
     * @brief Do a transaction for the slot
     *
     * @param[in] slot_drv    SD Host slot driver handle
     * @param[in] cmdinfo     SD command info, see `sdmmc_command_t`
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     */
    esp_err_t (*do_transaction)(sd_slot_driver_t *slot_drv, sdmmc_command_t *cmdinfo);

    /**
     * @brief Register SD event callbacks
     *
     * @note User can deregister a previously registered callback by calling this function and setting the to-be-deregistered callback member in
     *       the `cbs` structure to NULL.
     *
     * @param[in] slot_drv     SD Host slot driver handle
     * @param[in] cbs          Group of callback functions
     * @param[in] user_data    User data, which will be delivered to the callback functions directly
     *
     * @return
     *        - ESP_OK:                On success
     *        - ESP_ERR_INVALID_ARG:   Invalid arguments
     *        - ESP_ERR_INVALID_STATE: Driver state is invalid, you shouldn't call this API at this moment
     */
    esp_err_t (*register_cbs)(sd_slot_driver_t *slot_drv, const sd_host_evt_cbs_t *cbs, void *user_data);

    /**
     * @brief Remove an SD Host slot
     *
     * @param[in] slot_drv    SD Host slot driver handle
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_STATE:  Invalid state, slot is not available
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     */
    esp_err_t (*remove_slot)(sd_slot_driver_t *slot_drv);

    /**
     * @brief Set an SD Host slot clock always on
     *
     * @param[in] slot_drv    SD Host slot driver handle
     * @param[in] always_on   Always on or not
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     */
    esp_err_t (*set_cclk_always_on)(sd_slot_driver_t *slot_drv, bool always_on);

    /**
     * @brief Enable an SD Host slot IO interrupt
     *
     * @param[in] slot_drv    SD Host slot driver handle
     *
     * @return
     *        - ESP_OK:                 On success
     *        - ESP_ERR_INVALID_ARG:    Invalid argument
     */
    esp_err_t (*enable_io_int)(sd_slot_driver_t *slot_drv);

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
    esp_err_t (*wait_io_int)(sd_slot_driver_t *slot_drv, TickType_t timeout_ticks);

    /**
     * @brief Get slot info
     *
     * @param[in]  slot           SD Host slot handle
     * @param[out] info           SD slot info
     */
    esp_err_t (*get_info)(sd_slot_driver_t *slot_drv, sd_host_slot_info_t *info);
};

#ifdef __cplusplus
}
#endif
