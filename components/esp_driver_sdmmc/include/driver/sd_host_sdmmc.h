/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
*
* SPDX-License-Identifier: Apache-2.0
*/

#pragma once

#include "esp_err.h"
#include "soc/gpio_num.h"
#include "driver/sd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SD_HOST_SLOT_WIDTH_DEFAULT    0    ///< use the maximum possible width for the slot

/**
 * @brief SD Host SDMMC controller configuration
 */
typedef struct {
    uint32_t event_queue_items;    ///< Event queue items. If 0, fallback to default queue item number (4)
    uint32_t dma_desc_num;         ///< Number of DMA descriptor, fallback to default dma descriptor number (4)
} sd_host_sdmmc_cfg_t;

/**
 * @brief SD Host slot IO configuration
 */
typedef struct {
    sd_bus_width_t width;                            ///< Slot bus width
    gpio_num_t clk_io;                               ///< Clock
    gpio_num_t cmd_io;                               ///< Command
    gpio_num_t cd_io;                                ///< Card detect
    gpio_num_t wp_io;                                ///< Write protect
    gpio_num_t d0_io;                                ///< Data0
    gpio_num_t d1_io;                                ///< Data1
    gpio_num_t d2_io;                                ///< Data2
    gpio_num_t d3_io;                                ///< Data3
    gpio_num_t d4_io;                                ///< Data4
    gpio_num_t d5_io;                                ///< Data5
    gpio_num_t d6_io;                                ///< Data6
    gpio_num_t d7_io;                                ///< Data7
} sd_host_sdmmc_slot_io_cfg_t;

/**
 * @brief SD Host slot init configuration
 */
typedef struct {
    int slot_id;                                     ///< Slot ID
    sd_mode_t sd_mode;                               ///< SD mode, see `sd_mode_t`
    sd_host_sdmmc_slot_io_cfg_t io_config;           ///< IO configuration

    struct {
        uint32_t internal_pullup: 1;                 ///< Enable internal pullup or not
        uint32_t wp_active_high: 1;                  ///< WP signal is active high or not
    } slot_flags;                                    ///< Slot flags
} sd_host_slot_sdmmc_init_cfg_t;

/**
 * @brief Create an SDMMC Host controller
 *
 * @param[in]  config      SDMMC Host controller configuration
 * @param[out] ret_handle  Returned SDMMC Host controller handle
 *
 * @return
 *        - ESP_OK:               On success
 *        - ESP_ERR_NO_MEM:       Out of memory
 *        - ESP_ERR_NOT_FOUND:    Controller not found
 *        - ESP_ERR_INVALID_ARG:  Invalid argument
 */
esp_err_t sd_host_create_sdmmc_controller(const sd_host_sdmmc_cfg_t *config, sd_host_ctlr_handle_t *ret_handle);

/**
 * @brief Add an SD slot to the SDMMC Host controller
 *
 * @param[in]  ctlr        SD Host controller handle
 * @param[in]  config      SD Host slot init configuration
 * @param[out] ret_handle  Returned SD Host slot handle
 *
 * @return
 *        - ESP_OK:                 On success
 *        - ESP_ERR_INVALID_STATE:  Invalid state, slot is not available
 *        - ESP_ERR_INVALID_ARG:    Invalid argument
 *        - ESP_ERR_NO_MEM:         Out of memory
 */
esp_err_t sd_host_sdmmc_controller_add_slot(sd_host_ctlr_handle_t ctlr, const sd_host_slot_sdmmc_init_cfg_t *config, sd_host_slot_handle_t *ret_handle);

#ifdef __cplusplus
}
#endif
