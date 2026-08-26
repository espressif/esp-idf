/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "hal/sd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SD Host controller handle
 */
typedef struct sd_host_driver_t *sd_host_ctlr_handle_t;

/**
 * @brief SD Host slot handle
 */
typedef struct sd_slot_driver_t *sd_host_slot_handle_t;

/**
 * @brief SD Host slot configuration
 */
typedef struct {
    int freq_hz;                                 ///< Frequency in Hz
    sd_bus_width_t width;                        ///< Bus width
    sd_sampling_mode_t sampling_mode;            ///< Sampling mode, see `sd_sampling_mode_t`
    sdmmc_delay_phase_t delayphase;              ///< Delay phase, see `sdmmc_delay_phase_t`
    sdmmc_delay_line_t delayline;                ///< Delay line, see `sdmmc_delay_line_t`
} sd_host_slot_cfg_t;

/**
 * @brief Slot info
 */
typedef struct {
    int freq_hz;                                 ///< Frequency in Hz
    sd_bus_width_t width;                        ///< Bus width
    sd_mode_t sd_mode;                           ///< SD mode, see `sd_mode_t`
    sd_sampling_mode_t sampling_mode;            ///< Sampling mode, see `sd_sampling_mode_t`
} sd_host_slot_info_t;

/*---------------------------------------------
                Event Callbacks
----------------------------------------------*/
/**
 * @brief SD event data structure
 */
typedef struct {
    //leave empty for future-proof
} sd_host_evt_data_t;

/**
 * @brief Prototype of SD event callback
 *
 * @param[in] slot      Slot handle
 * @param[in] edata     SD event data
 * @param[in] user_data User registered context, registered when in `esp_isp_register_event_callbacks()`
 *
 * @return Whether a high priority task is woken up by this function
 */
typedef bool (*sd_host_callback_t)(sd_host_slot_handle_t slot, const sd_host_evt_data_t *edata, void *user_data);

/**
 * @brief SD event callbacks
 */
typedef struct {
    sd_host_callback_t on_trans_done;      ///< Event callback, invoked when one transaction done
    sd_host_callback_t on_io_interrupt;    ///< Event callback, invoked when IO interrupts
} sd_host_evt_cbs_t;

#ifdef __cplusplus
}
#endif
