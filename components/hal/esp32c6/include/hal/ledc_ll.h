/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for LEDC register operations.
// Note that most of the register operations in this layer are non-atomic operations.

#pragma once

#include "hal/ledc_types.h"
#include "soc/ledc_struct.h"
#include "soc/ledc_reg.h"
#include "soc/pcr_struct.h"
#include "soc/clk_tree_defs.h"
#include "hal/assert.h"
#include "esp_rom_sys.h"    //for sync issue workaround
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LEDC_LL_GET_HW()           &LEDC

#define LEDC_LL_DUTY_NUM_MAX       (LEDC_CH0_GAMMA_DUTY_NUM_V)
#define LEDC_LL_DUTY_CYCLE_MAX     (LEDC_CH0_GAMMA_DUTY_CYCLE_V)
#define LEDC_LL_DUTY_SCALE_MAX     (LEDC_CH0_GAMMA_SCALE_V)
#define LEDC_LL_HPOINT_VAL_MAX     (LEDC_HPOINT_CH0_V)
#define LEDC_LL_FRACTIONAL_BITS    (8)
#define LEDC_LL_FRACTIONAL_MAX     ((1 << LEDC_LL_FRACTIONAL_BITS) - 1)
#define LEDC_LL_GLOBAL_CLOCKS      SOC_LEDC_CLKS

#define LEDC_LL_GLOBAL_CLK_DEFAULT LEDC_SLOW_CLK_RC_FAST

/**
 * @brief Enable peripheral register clock
 *
 * @param enable    Enable/Disable
 */
static inline void ledc_ll_enable_bus_clock(bool enable)
{
    PCR.ledc_conf.ledc_clk_en = enable;
}

/**
 * @brief Reset whole peripheral register to init value defined by HW design
 */
static inline void ledc_ll_enable_reset_reg(bool enable)
{
    PCR.ledc_conf.ledc_rst_en = enable;
}

/**
 * @brief Enable the power for LEDC memory block
 */
static inline void ledc_ll_enable_mem_power(bool enable)
{
    // No register to control the power for LEDC memory block on C6
}

/**
 * @brief Enable LEDC function clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param en True to enable, false to disable
 *
 * @return None
 */
static inline void ledc_ll_enable_clock(ledc_dev_t *hw, bool en)
{
    (void)hw;
    PCR.ledc_sclk_conf.ledc_sclk_en = en;
}

/**
 * @brief Set LEDC low speed timer clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
static inline void ledc_ll_set_slow_clk_sel(ledc_dev_t *hw, ledc_slow_clk_sel_t slow_clk_sel)
{
    (void)hw;
    uint32_t clk_sel_val = 0;
    if (slow_clk_sel == LEDC_SLOW_CLK_PLL_DIV) {
        clk_sel_val = 1;
    } else if (slow_clk_sel == LEDC_SLOW_CLK_RC_FAST) {
        clk_sel_val = 2;
    } else if (slow_clk_sel == LEDC_SLOW_CLK_XTAL) {
        clk_sel_val = 3;
    }
    PCR.ledc_sclk_conf.ledc_sclk_sel = clk_sel_val;
}

/**
 * @brief Get LEDC low speed timer clock
 *
 * @param hw Beginning address of the peripheral registers
 * @param slow_clk_sel LEDC low speed timer clock source
 *
 * @return None
 */
static inline void ledc_ll_get_slow_clk_sel(ledc_dev_t *hw, ledc_slow_clk_sel_t *slow_clk_sel)
{
    (void)hw;
    uint32_t clk_sel_val = PCR.ledc_sclk_conf.ledc_sclk_sel;
    if (clk_sel_val == 1) {
        *slow_clk_sel = LEDC_SLOW_CLK_PLL_DIV;
    } else if (clk_sel_val == 2) {
        *slow_clk_sel = LEDC_SLOW_CLK_RC_FAST;
    } else if (clk_sel_val == 3) {
        *slow_clk_sel = LEDC_SLOW_CLK_XTAL;
    } else {
        abort();
    }
}

/**
 * @brief Update LEDC low speed timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_ls_timer_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.para_up = 1;
    // Here, we don't wait for the bit gets cleared since it can take quite long depends on the pwm frequency
}

/**
 * @brief Reset LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_rst(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.rst = 1;
    hw->timer_group[speed_mode].timer[timer_sel].conf.rst = 0;
}

/**
 * @brief Pause LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_pause(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.pause = 1;
}

/**
 * @brief Resume LEDC timer
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_timer_resume(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.pause = 0;
}

/**
 * @brief Set LEDC timer clock divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_set_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t clock_divider)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.clk_div = clock_divider;
}

/**
 * @brief Get LEDC timer clock divider
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clock_divider Timer clock divide value, the timer clock is divided from the selected clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_divider(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *clock_divider)
{
    *clock_divider = hw->timer_group[speed_mode].timer[timer_sel].conf.clk_div;
}

/**
 * @brief Get LEDC timer clock source
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param clk_src Pointer to accept the timer clock source
 *
 * @return None
 */
static inline void ledc_ll_get_clock_source(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, ledc_clk_src_t *clk_src)
{
    // The target has no timer-specific clock source option
    HAL_ASSERT(hw->timer_group[speed_mode].timer[timer_sel].conf.tick_sel == 0);
    *clk_src = LEDC_SCLK;
}

/**
 * @brief Set LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Resolution of duty setting in number of bits. The range of duty values is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t duty_resolution)
{
    hw->timer_group[speed_mode].timer[timer_sel].conf.duty_res = duty_resolution;
}

/**
 * @brief Get LEDC duty resolution
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param duty_resolution Pointer to accept the resolution of duty setting in number of bits.
 *
 * @return None
 */
static inline void ledc_ll_get_duty_resolution(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *duty_resolution)
{
    *duty_resolution = hw->timer_group[speed_mode].timer[timer_sel].conf.duty_res;
}

/**
 * @brief Get LEDC max duty
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 * @param max_duty Pointer to accept the max duty
 *
 * @return None
 */
static inline void ledc_ll_get_max_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_timer_t timer_sel, uint32_t *max_duty)
{
    *max_duty = (1 << (LEDC.timer_group[speed_mode].timer[timer_sel].conf.duty_res));
}

/**
 * @brief Update channel configure when select low speed mode
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_ls_channel_update(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.para_up = 1;
}

/**
 * @brief Set LEDC hpoint value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param hpoint_val LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
static inline void ledc_ll_set_hpoint(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t hpoint_val)
{
    hw->channel_group[speed_mode].channel[channel_num].hpoint.hpoint = hpoint_val;
}

/**
 * @brief Get LEDC hpoint value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param hpoint_val Pointer to accept the LEDC hpoint value(max: 0xfffff)
 *
 * @return None
 */
static inline void ledc_ll_get_hpoint(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *hpoint_val)
{
    *hpoint_val = hw->channel_group[speed_mode].channel[channel_num].hpoint.hpoint;
}

/**
 * @brief Set LEDC the integer part of duty value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_val LEDC duty value, the range of duty setting is [0, (2**duty_resolution)]
 *
 * @return None
 */
static inline void ledc_ll_set_duty_int_part(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_val)
{
    hw->channel_group[speed_mode].channel[channel_num].duty.duty = duty_val << 4;
}

/**
 * @brief Get LEDC duty value
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_val Pointer to accept the LEDC duty value
 *
 * @return None
 */
static inline void ledc_ll_get_duty(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *duty_val)
{
    *duty_val = (hw->channel_group[speed_mode].channel[channel_num].duty_rd.duty_r >> 4);
}

/**
 * @brief Set LEDC duty change direction
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_direction LEDC duty change direction, increase or decrease
 *
 * @return None
 */
static inline void ledc_ll_set_duty_direction(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_duty_direction_t duty_direction)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr.gamma_duty_inc = duty_direction;
}

/**
 * @brief Set the number of increased or decreased times
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_num The number of increased or decreased times
 *
 * @return None
 */
static inline void ledc_ll_set_duty_num(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_num)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr.gamma_duty_num = duty_num;
}

/**
 * @brief Set the duty cycles of increase or decrease
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_cycle The duty cycles
 *
 * @return None
 */
static inline void ledc_ll_set_duty_cycle(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_cycle)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr.gamma_duty_cycle = duty_cycle;
}

/**
 * @brief Set the step scale of increase or decrease
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_scale The step scale
 *
 * @return None
 */
static inline void ledc_ll_set_duty_scale(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_scale)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr.gamma_scale = duty_scale;
}

/**
 * @brief Function to set fade parameters all-in-one
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param dir LEDC duty change direction, increase or decrease
 * @param cycle The duty cycles
 * @param scale The step scale
 * @param step The number of increased or decreased times
 *
 * @return None
 */
static inline void ledc_ll_set_fade_param(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t dir, uint32_t cycle, uint32_t scale, uint32_t step)
{
    uint32_t val = (dir << LEDC_CH0_GAMMA_DUTY_INC_S) | (cycle << LEDC_CH0_GAMMA_DUTY_CYCLE_S) | (scale << LEDC_CH0_GAMMA_SCALE_S) | (step << LEDC_CH0_GAMMA_DUTY_NUM_S);
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr.val = val;
}

/**
 * @brief Set the range number of the specified duty configurations to be written from gamma_wr register to gamma ram
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_range Range index (0 - (SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX-1)), it specifies to which range in gamma ram to write
 *
 * @return None
 */
static inline void ledc_ll_set_duty_range_wr_addr(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_range)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].wr_addr.gamma_wr_addr = duty_range;
}

/**
 * @brief Function to set fade parameters all-in-one for one range
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param range Gamma fade range index, 0 ~ SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX
 * @param dir LEDC duty change direction, increase or decrease
 * @param cycle The duty cycles
 * @param scale The step scale
 * @param step The number of increased or decreased times
 *
 * @return None
 */
static inline void ledc_ll_set_fade_param_range(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint8_t range, uint32_t dir, uint32_t cycle, uint32_t scale, uint32_t step)
{
    HAL_ASSERT(range < SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX);
    // To workaround sync issue
    // This is to ensure the fade param write to the gamma_wr register would not mess up the last wr_addr
    ledc_ll_set_duty_range_wr_addr(hw, speed_mode, channel_num, range);
    esp_rom_delay_us(5);

    ledc_ll_set_fade_param(hw, speed_mode, channel_num, dir, cycle, scale, step);
    ledc_ll_set_duty_range_wr_addr(hw, speed_mode, channel_num, range);

    // To workaround sync issue
    // This is to ensure the fade param in gamma_wr register can be written to the correct wr_addr
    esp_rom_delay_us(5);
}

/**
 * @brief Set the total number of ranges in one fading
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param range_num Total number of ranges (1 - SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX) of the fading configured
 *
 * @return None
 */
static inline void ledc_ll_set_range_number(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t range_num)
{
    hw->channel_gamma_conf_group[speed_mode].gamma_conf[channel_num].gamma_entry_num = range_num;
}

/**
 * @brief Get the total number of ranges in one fading
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param range_num Pointer to accept fade range number
 *
 * @return None
 */
static inline void ledc_ll_get_range_number(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *range_num)
{
    *range_num = hw->channel_gamma_conf_group[speed_mode].gamma_conf[channel_num].gamma_entry_num;
}

/**
 * @brief Set the range number of the specified duty configurations to be read from gamma ram to gamma_rd register
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_range Range index (0 - (SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX-1)), it specifies to which range in gamma ram to read
 *
 * @return None
 */
static inline void ledc_ll_set_duty_range_rd_addr(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t duty_range)
{
    hw->channel_gamma_group[speed_mode].channel[channel_num].rd_addr.gamma_rd_addr = duty_range;
}

/**
 * @brief Get fade configurations in gamma_rd register
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param dir Pointer to accept fade direction value
 * @param cycle Pointer to accept fade cycle value
 * @param scale Pointer to accept fade scale value
 * @param step Pointer to accept fade step value
 *
 * @return None
 */
static inline void ledc_ll_get_fade_param(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t *dir, uint32_t *cycle, uint32_t *scale, uint32_t *step)
{
    uint32_t val = hw->channel_gamma_group[speed_mode].channel[channel_num].rd_data.gamma_rd_data;
    *dir = (val & LEDC_CH0_GAMMA_DUTY_INC_M) >> LEDC_CH0_GAMMA_DUTY_INC_S;
    *cycle = (val & LEDC_CH0_GAMMA_DUTY_CYCLE_M) >> LEDC_CH0_GAMMA_DUTY_CYCLE_S;
    *scale = (val & LEDC_CH0_GAMMA_SCALE_M) >> LEDC_CH0_GAMMA_SCALE_S;
    *step = (val & LEDC_CH0_GAMMA_DUTY_NUM_M) >> LEDC_CH0_GAMMA_DUTY_NUM_S;
}

/**
 * @brief Get fade configurations for one range
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param range Gamma fade range index to get, 0 ~ SOC_LEDC_GAMMA_CURVE_FADE_RANGE_MAX
 * @param dir Pointer to accept fade direction value
 * @param cycle Pointer to accept fade cycle value
 * @param scale Pointer to accept fade scale value
 * @param step Pointer to accept fade step value
 *
 * @return None
 */
static inline void ledc_ll_get_fade_param_range(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint8_t range, uint32_t *dir, uint32_t *cycle, uint32_t *scale, uint32_t *step)
{
    // On ESP32C6/H2, gamma ram read/write has the APB and LEDC clock domain sync issue
    // To make sure the parameter read is from the correct gamma ram addr, add a delay in between to ensure synchronization
    ledc_ll_set_duty_range_rd_addr(hw, speed_mode, channel_num, range);
    esp_rom_delay_us(5);
    ledc_ll_get_fade_param(hw, speed_mode, channel_num, dir, cycle, scale, step);
}

/**
 * @brief Set the output enable
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param sig_out_en The output enable status
 *
 * @return None
 */
__attribute__((always_inline))
static inline void ledc_ll_set_sig_out_en(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool sig_out_en)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.sig_out_en = sig_out_en;
}

/**
 * @brief Set the duty start
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param duty_start The duty start
 *
 * @return None
 */
static inline void ledc_ll_set_duty_start(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool duty_start)
{
    hw->channel_group[speed_mode].channel[channel_num].conf1.duty_start = duty_start;
}

/**
 * @brief Set output idle level
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param idle_level The output idle level
 *
 * @return None
 */
__attribute__((always_inline))
static inline void ledc_ll_set_idle_level(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, uint32_t idle_level)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.idle_lv = idle_level & 0x1;
}

/**
 * @brief Set fade end interrupt enable
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param fade_end_intr_en The fade end interrupt enable status
 *
 * @return None
 */
static inline void ledc_ll_set_fade_end_intr(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, bool fade_end_intr_en)
{
    uint32_t value = hw->int_ena.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    hw->int_ena.val = fade_end_intr_en ? (value | BIT(int_en_base + channel_num)) : (value & (~(BIT(int_en_base + channel_num))));
}

/**
 * @brief Get fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param intr_status The fade end interrupt status
 *
 * @return None
 */
static inline void ledc_ll_get_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, uint32_t *intr_status)
{
    uint32_t value = hw->int_st.val;
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    *intr_status = (value >> int_en_base) & 0xff;
}

/**
 * @brief Clear fade end interrupt status
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 *
 * @return None
 */
static inline void ledc_ll_clear_fade_end_intr_status(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num)
{
    uint32_t int_en_base = LEDC_DUTY_CHNG_END_CH0_INT_ENA_S;
    hw->int_clr.val = BIT(int_en_base + channel_num);
}

/**
 * @brief Set timer index of the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param timer_sel LEDC timer index (0-3), select from ledc_timer_t
 *
 * @return None
 */
static inline void ledc_ll_bind_channel_timer(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_timer_t timer_sel)
{
    hw->channel_group[speed_mode].channel[channel_num].conf0.timer_sel = timer_sel;
}

/**
 * @brief Get timer index of the specified channel
 *
 * @param hw Beginning address of the peripheral registers
 * @param speed_mode LEDC speed_mode, low-speed mode only
 * @param channel_num LEDC channel index (0-5), select from ledc_channel_t
 * @param timer_sel Pointer to accept the LEDC timer index
 *
 * @return None
 */
static inline void ledc_ll_get_channel_timer(ledc_dev_t *hw, ledc_mode_t speed_mode, ledc_channel_t channel_num, ledc_timer_t *timer_sel)
{
    *timer_sel = (ledc_timer_t)(hw->channel_group[speed_mode].channel[channel_num].conf0.timer_sel);
}

#ifdef __cplusplus
}
#endif
