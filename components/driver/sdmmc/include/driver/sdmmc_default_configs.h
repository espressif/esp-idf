/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "sdmmc_types.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDMMC_HOST_SLOT_0     0     ///< SDMMC slot 0
#define SDMMC_HOST_SLOT_1     1     ///< SDMMC slot 1

/**
 * @brief Default sdmmc_host_t structure initializer for SDMMC peripheral
 *
 * Uses SDMMC peripheral, with 4-bit mode enabled, and max frequency set to 20MHz
 */
#define SDMMC_HOST_DEFAULT() {\
    .flags = SDMMC_HOST_FLAG_8BIT | \
             SDMMC_HOST_FLAG_4BIT | \
             SDMMC_HOST_FLAG_1BIT | \
             SDMMC_HOST_FLAG_DDR, \
    .slot = SDMMC_HOST_SLOT_1, \
    .max_freq_khz = SDMMC_FREQ_DEFAULT, \
    .io_voltage = 3.3f, \
    .init = &sdmmc_host_init, \
    .set_bus_width = &sdmmc_host_set_bus_width, \
    .get_bus_width = &sdmmc_host_get_slot_width, \
    .set_bus_ddr_mode = &sdmmc_host_set_bus_ddr_mode, \
    .set_card_clk = &sdmmc_host_set_card_clk, \
    .set_cclk_always_on = &sdmmc_host_set_cclk_always_on, \
    .do_transaction = &sdmmc_host_do_transaction, \
    .deinit = &sdmmc_host_deinit, \
    .io_int_enable = sdmmc_host_io_int_enable, \
    .io_int_wait = sdmmc_host_io_int_wait, \
    .command_timeout_ms = 0, \
    .get_real_freq = &sdmmc_host_get_real_freq, \
    .input_delay_phase = SDMMC_DELAY_PHASE_0, \
    .set_input_delay = &sdmmc_host_set_input_delay \
}

#define SDMMC_SLOT_NO_CD      GPIO_NUM_NC     ///< indicates that card detect line is not used
#define SDMMC_SLOT_NO_WP      GPIO_NUM_NC     ///< indicates that write protect line is not used
#define SDMMC_SLOT_WIDTH_DEFAULT 0 ///< use the maximum possible width for the slot



#if SOC_SDMMC_USE_IOMUX && !SOC_SDMMC_USE_GPIO_MATRIX
/**
 * Macro defining default configuration of SDMMC host slot
 */
#define SDMMC_SLOT_CONFIG_DEFAULT() {\
    .cd = SDMMC_SLOT_NO_CD, \
    .wp = SDMMC_SLOT_NO_WP, \
    .width   = SDMMC_SLOT_WIDTH_DEFAULT, \
    .flags = 0, \
}

#else

/**
 * Macro defining default configuration of SDMMC host slot
 */
#if CONFIG_IDF_TARGET_ESP32P4
#define SDMMC_SLOT_CONFIG_DEFAULT() {\
    .clk = GPIO_NUM_43, \
    .cmd = GPIO_NUM_44, \
    .d0 = GPIO_NUM_39, \
    .d1 = GPIO_NUM_40, \
    .d2 = GPIO_NUM_41, \
    .d3 = GPIO_NUM_42, \
    .d4 = GPIO_NUM_45, \
    .d5 = GPIO_NUM_46, \
    .d6 = GPIO_NUM_47, \
    .d7 = GPIO_NUM_48, \
    .cd = SDMMC_SLOT_NO_CD, \
    .wp = SDMMC_SLOT_NO_WP, \
    .width   = SDMMC_SLOT_WIDTH_DEFAULT, \
    .flags = 0, \
}

#elif CONFIG_IDF_TARGET_ESP32S3
#define SDMMC_SLOT_CONFIG_DEFAULT() {\
    .clk = GPIO_NUM_14, \
    .cmd = GPIO_NUM_15, \
    .d0 = GPIO_NUM_2, \
    .d1 = GPIO_NUM_4, \
    .d2 = GPIO_NUM_12, \
    .d3 = GPIO_NUM_13, \
    .d4 = GPIO_NUM_33, \
    .d5 = GPIO_NUM_34, \
    .d6 = GPIO_NUM_35, \
    .d7 = GPIO_NUM_36, \
    .cd = SDMMC_SLOT_NO_CD, \
    .wp = SDMMC_SLOT_NO_WP, \
    .width   = SDMMC_SLOT_WIDTH_DEFAULT, \
    .flags = 0, \
}
#endif  // GPIO Matrix chips

#endif

#ifdef __cplusplus
}
#endif
