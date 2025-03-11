/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for LEDC.
// There is no parameter check in the hal layer, so the caller must ensure the correctness of the parameters.

#pragma once

#include "hal/ledc_ll.h"
#include "hal/ledc_types.h"
#include "soc/soc_caps.h"

/**
 * Context that should be maintained by both the driver and the HAL
 */
typedef struct {
    ledc_dev_t *dev;
    ledc_mode_t speed_mode;
} ledc_hal_context_t;

/**
 * @brief Set LEDC low speed timer clock
 *
 * @param hal Context of the HAL layer
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
#define ledc_hal_set_slow_clk_sel(hal, slow_clk_sel)  ledc_ll_set_slow_clk_sel((hal)->dev, slow_clk_sel)

/**
 * @brief Get LEDC low speed timer clock
 *
 * @param hal Context of the HAL layer
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
#define ledc_hal_get_slow_clk_sel(hal, slow_clk_sel)  ledc_ll_get_slow_clk_sel((hal)->dev, slow_clk_sel)

/**
 * @brief Update LEDC low speed timer
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
#define ledc_hal_ls_timer_update(hal, timer_sel)  ledc_ll_ls_timer_update((hal)->dev, (hal)->speed_mode, timer_sel)

/**
 * @brief Reset LEDC timer
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
#define ledc_hal_timer_rst(hal, timer_sel)  ledc_ll_timer_rst((hal)->dev, (hal)->speed_mode, timer_sel)

/**
 * @brief Pause LEDC timer
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
#define ledc_hal_timer_pause(hal, timer_sel)  ledc_ll_timer_pause((hal)->dev, (hal)->speed_mode, timer_sel)

/**
 * @brief Resume LEDC timer
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
#define ledc_hal_timer_resume(hal, timer_sel)  ledc_ll_timer_resume((hal)->dev, (hal)->speed_mode, timer_sel)

/**
 * @brief Set LEDC timer clock divider
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
#define ledc_hal_set_clock_divider(hal, timer_sel, clock_divider)  ledc_ll_set_clock_divider((hal)->dev, (hal)->speed_mode, timer_sel, clock_divider)

/**
 * @brief Get LEDC timer clock divider
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
#define ledc_hal_get_clock_divider(hal, timer_sel, clock_divider)  ledc_ll_get_clock_divider((hal)->dev, (hal)->speed_mode, timer_sel, clock_divider)

/**
 * @brief Set LEDC timer clock source
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Timer clock source
 *
 * @return None
 */
#define ledc_hal_set_clock_source(hal, timer_sel, clk_src)  ledc_ll_set_clock_source((hal)->dev, (hal)->speed_mode, timer_sel, clk_src)

/**
 * @brief Get LEDC timer clock source
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Pointer to accept the timer clock source
 *
 * @return None
 */
#define ledc_hal_get_clock_source(hal, timer_sel, clk_src)  ledc_ll_get_clock_source((hal)->dev, (hal)->speed_mode, timer_sel, clk_src)

/**
 * @brief Set LEDC duty resolution
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Resolution of duty setting in number of bits. The range of duty values is [0, (2**duty_resolution)]
 *
 * @return None
 */
#define ledc_hal_set_duty_resolution(hal, timer_sel, duty_resolution)  ledc_ll_set_duty_resolution((hal)->dev, (hal)->speed_mode, timer_sel, duty_resolution)

/**
 * @brief Get LEDC duty resolution
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Pointer to accept the resolution of duty setting in number of bits.
 *
 * @return None
 */
#define ledc_hal_get_duty_resolution(hal, timer_sel, duty_resolution)  ledc_ll_get_duty_resolution((hal)->dev, (hal)->speed_mode, timer_sel, duty_resolution)

/**
 * @brief Get LEDC max duty
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param max_duty Pointer to accept the max duty
 *
 * @return None
 */
#define ledc_hal_get_max_duty(hal, channel_num, max_duty)  ledc_ll_get_max_duty((hal)->dev, (hal)->speed_mode, channel_num, max_duty)

/**
 * @brief Get LEDC hpoint value
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param hpoint_val Pointer to accept the LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
#define ledc_hal_get_hpoint(hal, channel_num, hpoint_val)  ledc_ll_get_hpoint((hal)->dev, (hal)->speed_mode, channel_num, hpoint_val)

/**
 * @brief Set the output enable
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param sig_out_en The output enable status
 *
 * @return None
 */
#define ledc_hal_set_sig_out_en(hal, channel_num, sig_out_en)  ledc_ll_set_sig_out_en((hal)->dev, (hal)->speed_mode, channel_num, sig_out_en)

/**
 * @brief Set output idle level
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param idle_level The output idle level
 *
 * @return None
 */
#define ledc_hal_set_idle_level(hal, channel_num, idle_level)  ledc_ll_set_idle_level((hal)->dev, (hal)->speed_mode, channel_num, idle_level)

/**
 * @brief Set fade end interrupt enable
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param fade_end_intr_en The fade end interrupt enable status
 *
 * @return None
 */
#define ledc_hal_set_fade_end_intr(hal, channel_num, fade_end_intr_en)  ledc_ll_set_fade_end_intr((hal)->dev, (hal)->speed_mode, channel_num, fade_end_intr_en)

/**
 * @brief Set timer index of the specified channel
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
#define ledc_hal_bind_channel_timer(hal, channel_num, timer_sel)  ledc_ll_bind_channel_timer((hal)->dev, (hal)->speed_mode, channel_num, timer_sel)

/**
 * @brief Get timer index of the specified channel
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param timer_sel Pointer to accept the LEDC timer index
 *
 * @return None
 */
#define ledc_hal_get_channel_timer(hal, channel_num, timer_sel)  ledc_ll_get_channel_timer((hal)->dev, (hal)->speed_mode, channel_num, timer_sel)

/**
 * @brief Init the LEDC hal. This function should be called first before other hal layer function is called
 *
 * @param hal Context of the HAL layer
 * @param speed_mode speed_mode Select the LEDC speed_mode, high-speed mode and low-speed mod
 *
 * @return None
 */
void ledc_hal_init(ledc_hal_context_t *hal, ledc_mode_t speed_mode);

/**
 * @brief Update channel configure when select low speed mode
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
void ledc_hal_ls_channel_update(ledc_hal_context_t *hal, ledc_channel_t channel_num);

/**
 * @brief Set the duty start
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_start The duty start
 *
 * @return None
 */
void ledc_hal_set_duty_start(ledc_hal_context_t *hal, ledc_channel_t channel_num, bool duty_start);

/**
 * @brief Set LEDC the integer part of duty value
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val LEDC duty value, the range of duty setting is [0, (2**duty_resolution)]
 *
 * @return None
 */
void ledc_hal_set_duty_int_part(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_val);

/**
 * @brief Set LEDC hpoint value
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param hpoint_val LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
void ledc_hal_set_hpoint(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t hpoint_val);

/**
 * @brief Get LEDC duty value
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_val Pointer to accept the LEDC duty value
 *
 * @return None
 */
void ledc_hal_get_duty(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t *duty_val);

/**
 * @brief Set LEDC duty change direction
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_direction LEDC duty change direction, increase or decrease
 *
 * @return None
 */
void ledc_hal_set_duty_direction(ledc_hal_context_t *hal, ledc_channel_t channel_num, ledc_duty_direction_t duty_direction);

/**
 * @brief Set the number of increased or decreased times
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_num The number of increased or decreased times
 *
 * @return None
 */
void ledc_hal_set_duty_num(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_num);

/**
 * @brief Set the duty cycles of increase or decrease
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_cycle The duty cycles
 *
 * @return None
 */
void ledc_hal_set_duty_cycle(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_cycle);

/**
 * @brief Set the step scale of increase or decrease
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param duty_scale The step scale
 *
 * @return None
 */
void ledc_hal_set_duty_scale(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_scale);

#if SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED
/**
 * @brief Set the range number of the specified duty configurations to be written from gamma_wr register to gamma ram
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index, select from ledc_channel_t
 * @param duty_range Range index (0 - (SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX-1)), it specifies to which range in gamma ram to write
 *
 * @return None
 */
void ledc_hal_set_duty_range_wr_addr(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t duty_range);

/**
 * @brief Set the total number of ranges in one fading
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index, select from ledc_channel_t
 * @param range_num Total number of ranges (1-16) of the fading configured
 *
 * @return None
 */
void ledc_hal_set_range_number(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t range_num);

/**
 * @brief Get the total number of ranges in one fading
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index, select from ledc_channel_t
 * @param range_num Pointer to accept fade range number
 *
 * @return None
 */
void ledc_hal_get_range_number(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t *range_num);

/**
 * @brief Read the fade parameters that are stored in gamma ram for a certain fade range
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index, select from ledc_channel_t
 * @param range Range index (0 - (SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX-1)), it specifies to which range in gamma ram to read
 * @param dir Pointer to accept fade direction value
 * @param cycle Pointer to accept fade cycle value
 * @param scale Pointer to accept fade scale value
 * @param step Pointer to accept fade step value
 *
 * @return None
 */
void ledc_hal_get_fade_param(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t range, uint32_t *dir, uint32_t *cycle, uint32_t *scale, uint32_t *step);

/**
 * @brief Clear left-off range fade parameters in LEDC gamma ram
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index, select from ledc_channel_t
 * @param start_range Start of the range to clear
 */
void ledc_hal_clear_left_off_fade_param(ledc_hal_context_t *hal, ledc_channel_t channel_num, uint32_t start_range);
#endif //SOC_LEDC_GAMMA_CURVE_FADE_SUPPORTED

/**
 * @brief Get interrupt status of the specified channel
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 * @param intr_status Pointer to accept the interrupt status
 *
 * @return None
 */
void ledc_hal_get_fade_end_intr_status(ledc_hal_context_t *hal, uint32_t *intr_status);

/**
 * @brief Clear interrupt status of the specified channel
 *
 * @param hal Context of the HAL layer
 * @param channel_num LEDC channel index (0-7), select from ledc_channel_t
 *
 * @return None
 */
void ledc_hal_clear_fade_end_intr_status(ledc_hal_context_t *hal, ledc_channel_t channel_num);

/**
 * @brief Get clock config of LEDC timer
 *
 * @param hal Context of the HAL layer
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_cfg Pointer to accept clock config
 *
 * @return None
 */
void ledc_hal_get_clk_cfg(ledc_hal_context_t *hal, ledc_timer_t timer_sel, ledc_clk_cfg_t *clk_cfg);
