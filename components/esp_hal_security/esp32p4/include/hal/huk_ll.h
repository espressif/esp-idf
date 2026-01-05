/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use it in application code.
 ******************************************************************************/

#pragma once

#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hal/huk_types.h"
#include "soc/huk_reg.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void huk_ll_power_up(void)
{

}

/* @brief Configure the HUK mode */
static inline void huk_ll_configure_mode(const esp_huk_mode_t huk_mode)
{
    REG_SET_FIELD(HUK_CONF_REG, HUK_MODE, huk_mode);
}

static inline void huk_ll_write_info(const uint8_t *buffer, const size_t size)
{
    memcpy((uint8_t *)HUK_INFO_MEM, buffer, size);
}

static inline void huk_ll_read_info(uint8_t *buffer, const size_t size)
{
    memcpy(buffer, (uint8_t *)HUK_INFO_MEM, size);
}

/* @brief Start the HUK at IDLE state */
static inline void huk_ll_start(void)
{
    REG_SET_FIELD(HUK_START_REG, HUK_START, 1);
}

/* @brief Continue HUK operation at LOAD/GAIN state */
static inline void huk_ll_continue(void)
{
    REG_SET_FIELD(HUK_START_REG, HUK_CONTINUE, 1);
}

/* @bried Enable or Disable the HUK interrupts */
static inline void huk_ll_configure_interrupt(const esp_huk_interrupt_type_t intr, const bool en)
{
    switch (intr) {
    case ESP_HUK_INT_PREP_DONE:
        REG_SET_FIELD(HUK_INT_ENA_REG, HUK_PREP_DONE_INT_ENA, en);
        break;
    case ESP_HUK_INT_PROC_DONE:
        REG_SET_FIELD(HUK_INT_ENA_REG, HUK_PROC_DONE_INT_ENA, en);
        break;
    case ESP_HUK_INT_POST_DONE:
        REG_SET_FIELD(HUK_INT_ENA_REG, HUK_POST_DONE_INT_ENA, en);
        break;
    default:
        return;
    }
}

/* @bried Clear the HUK interrupts */
static inline void huk_ll_clear_int(const esp_huk_interrupt_type_t intr)
{
    switch (intr) {
    case ESP_HUK_INT_PREP_DONE:
        REG_SET_FIELD(HUK_INT_CLR_REG, HUK_PREP_DONE_INT_CLR, 1);
        break;
    case ESP_HUK_INT_PROC_DONE:
        REG_SET_FIELD(HUK_INT_CLR_REG, HUK_PROC_DONE_INT_CLR, 1);
        break;
    case ESP_HUK_INT_POST_DONE:
        REG_SET_FIELD(HUK_INT_CLR_REG, HUK_POST_DONE_INT_CLR, 1);
        break;
    default:
        return;
    }
}

/**
 * @brief Read state of Hardware Unique Key Generator
 *
 * @return esp_huk_state_t
 */
static inline esp_huk_state_t huk_ll_get_state(void)
{
    return (esp_huk_state_t) REG_GET_FIELD(HUK_STATE_REG, HUK_STATE);
}

/**
 * @brief Get the HUK generation status
 */
static inline esp_huk_gen_status_t huk_ll_get_gen_status(void)
{
    return (esp_huk_gen_status_t) REG_GET_FIELD(HUK_STATUS_REG, HUK_STATUS);
}

/**
 * @brief Read the HUK date information
 */
static inline uint32_t huk_ll_get_date_info(void)
{
    // Only the least significant 28 bits have desired information
    return (uint32_t)(0x0FFFFFFF & REG_READ(HUK_DATE_REG));
}

#ifdef __cplusplus
}
#endif
#endif
