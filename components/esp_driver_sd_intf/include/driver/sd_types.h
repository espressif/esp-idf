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
    struct {
        int freq_hz;                                 ///< Frequency in Hz
        bool override;                               ///< If set to true, frequency will be set to freq_hz; If set to false, frequency is unchanged. By default it's false
    } freq;                                          ///< Frequency settings
    struct {
        bool override;                               ///< If set to true, width will be set to width configured in `sd_host_sdmmc_slot_io_cfg_t`; If set to false, width is unchanged. By default it's false
    } width;                                         ///< Bus width settings
    struct {
        sd_sampling_mode_t mode;                     ///< Sampling mode, see `sd_sampling_mode_t`
        bool override;                               ///< If set to true, sampling mode will be set to sampling_mode; If set to false, sampling mode is unchanged. By default it's false
    } sampling_mode;                                 ///< Sampling mode settings
    struct {
        sdmmc_delay_phase_t delayphase;              ///< Delay phase, see `sdmmc_delay_phase_t`
        bool override;                               ///< If set to true, delay phase will be set to delay_phase; If set to false, delay phase is unchanged. By default it's false
    } delay_phase;                                   ///< Delay phase settings
    struct {
        sdmmc_delay_line_t delayline;                ///< Delay line, see `sdmmc_delay_line_t`
        bool override;                               ///< If set to true, delay line will be set to delay_line; If set to false, delay line is unchanged. By default it's false
    } delay_line;                                    ///< Delay line settings
} sd_host_slot_cfg_t;

/**
 * @brief Slot info
 */
typedef struct {
    int freq_hz;                                     ///< Frequency in Hz
    uint8_t width;                                   ///< Bus width
    sd_mode_t sd_mode;                               ///< SD mode, see `sd_mode_t`
    sd_sampling_mode_t sampling_mode;                ///< Sampling mode, see `sd_sampling_mode_t`
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
