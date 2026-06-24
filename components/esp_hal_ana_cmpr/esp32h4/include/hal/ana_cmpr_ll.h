/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <sys/param.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/ana_cmpr_types.h"
#include "soc/zero_det_struct.h"
#include "soc/zero_det_reg.h"
#include "soc/pcr_struct.h"
#include "soc/soc_etm_struct.h"
#include "soc/soc_etm_source.h"
#include "esp_rom_sys.h"

#ifdef __cplusplus
extern "C" {
#endif

// the analog comparator on this target is also called zero detector
typedef zero_det_dev_t analog_cmpr_dev_t;

#define ANALOG_CMPR_LL_GET(_attr) ANALOG_CMPR_LL_ ## _attr
#define ANALOG_CMPR_LL_SUPPORT(_feat) ANALOG_CMPR_LL_SUPPORT_ ## _feat

// Number of Analog Comparator instances
#define ANALOG_CMPR_LL_INST_NUM 1

// ANA_CMPR IP version
#define ANALOG_CMPR_LL_IP_VERSION 2

// Number of pads for each Analog Comparator instance
#define ANALOG_CMPR_LL_PAD_NUM 4

// Number of source channels for the comparator
#define ANALOG_CMPR_LL_SRC_CHANNEL_NUM 3

// Can detect positive/negative/any cross type
#define ANALOG_CMPR_LL_SUPPORT_EDGE_SPECIFIC_INTR_MASK  1

// Support software trigger channel scan
#define ANALOG_CMPR_LL_SUPPORT_SW_SCAN    1

#define ANALOG_CMPR_LL_GET_HW(unit)     (&ZERO_DET)

#define ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit, src_chan) (1UL << ((2 - (src_chan)) * 3 + 0))
#define ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit, src_chan) (1UL << ((2 - (src_chan)) * 3 + 1))
#define ANALOG_CMPR_LL_ANY_CROSS_INTR_MASK(unit, src_chan) (ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit, src_chan) | ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit, src_chan))
#define ANALOG_CMPR_LL_ALL_INTR_MASK(unit)                 0x1FF

#define ANALOG_CMPR_LL_ETM_SOURCE(unit, src_chan, type)    (ZERO_DET_EVT_CHANNEL_1_POS + (src_chan) + ((type) * ANALOG_CMPR_LL_SRC_CHANNEL_NUM))
#define ANALOG_CMPR_LL_ETM_TASK(unit, type)                (ZERO_DET_TASK_START)

#define ANALOG_CMPR_LL_MAX_ETM_DELAY_CYCLES                ZERO_DET_DELAY_EVENT_TIME
#define ANALOG_CMPR_LL_MAX_POLL_PERIOD_CYCLES              ZERO_DET_COMP_POLL_PERIOD

/**
 * @brief Enable the bus clock for Analog Comparator module
 *
 * @param unit_id Unit ID
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_bus_clock(int unit_id, bool enable)
{
    PCR.zero_det_conf.zero_det_clk_en = enable;
}

/**
 * @brief Reset the Analog Comparator module
 *
 * @param unit_id Unit ID
 */
static inline void analog_cmpr_ll_reset_register(int unit_id)
{
    PCR.zero_det_conf.zero_det_rst_en = 1;
    PCR.zero_det_conf.zero_det_rst_en = 0;
}

/**
 * @brief Reset the core logic of Analog Comparator module
 *
 * @param unit_id Unit ID
 */
static inline void analog_cmpr_ll_reset_core(int unit_id)
{
    (void)unit_id;
}

/**
 * @brief Set the clock source for analog comparator PAD_COMP_CLK
 *
 * @param unit_id Unit ID
 * @param clk_src Clock source, see `ana_cmpr_clk_src_t`
 */
static inline void analog_cmpr_ll_set_clk_src(int unit_id, ana_cmpr_clk_src_t clk_src)
{
    switch (clk_src) {
    case ANA_CMPR_CLK_SRC_XTAL:
        PCR.zero_det_clk_conf.zero_det_func_clk_sel = 0;
        break;
    case ANA_CMPR_CLK_SRC_RC_FAST:
        PCR.zero_det_clk_conf.zero_det_func_clk_sel = 1;
        break;
    case ANA_CMPR_CLK_SRC_PLL_F48M:
        PCR.zero_det_clk_conf.zero_det_func_clk_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * @brief Set the clock divider for analog comparator PAD_COMP_CLK
 *
 * @param unit_id Unit ID
 * @param div Clock divider value, the output clock frequency is input clock frequency divided by this value.
 *            Must be greater than 0.
 */
static inline void analog_cmpr_ll_set_clk_div(int unit_id, uint32_t div)
{
    (void)unit_id;
    (void)div;
}

/**
 * @brief Enable the function clock for Analog Comparator module
 *
 * @param unit_id Unit ID
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_function_clock(int unit_id, bool enable)
{
    PCR.zero_det_clk_conf.zero_det_func_clk_en = enable;
}

/**
 * @brief Get the interrupt mask from the given cross type
 *
 * @param unit_id Unit ID
 * @param src_chan Source channel ID
 * @param type Cross type
 * @return Interrupt mask value
 */
__attribute__((always_inline))
static inline uint32_t analog_cmpr_ll_get_intr_mask_by_type(uint32_t unit_id, uint32_t src_chan, ana_cmpr_cross_type_t type)
{
    uint32_t mask = 0;
    switch (type) {
    case ANA_CMPR_CROSS_POS:
        mask |= ANALOG_CMPR_LL_POS_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    case ANA_CMPR_CROSS_NEG:
        mask |= ANALOG_CMPR_LL_NEG_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    case ANA_CMPR_CROSS_ANY:
        mask |= ANALOG_CMPR_LL_ANY_CROSS_INTR_MASK(unit_id, src_chan);
        break;
    default:
        break;
    }
    return mask;
}

/**
 * @brief Enable (power on) analog comparator
 *
 * @param hw Analog comparator register base address
 * @param en True to enable, False to disable
 */
static inline void analog_cmpr_ll_enable(analog_cmpr_dev_t *hw, bool en)
{
    hw->det_pad_comp_cfg.det_pad_comp_xpd = en;
}

/**
 * @brief Set the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @param volt_level The voltage level of the internal reference, range [0.0V, 0.7VDD], step 0.1VDD
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_internal_ref_voltage(analog_cmpr_dev_t *hw, ana_cmpr_ref_voltage_t volt_level)
{
    hw->det_pad_comp_cfg.det_pad_comp_dref = volt_level;
}

/**
 * @brief Get the voltage of the internal reference
 *
 * @param hw Analog comparator register base address
 * @return The voltage of the internal reference
 */
static inline float analog_cmpr_ll_get_internal_ref_voltage(analog_cmpr_dev_t *hw)
{
    return hw->det_pad_comp_cfg.det_pad_comp_dref * 0.1F;
}

/**
 * @brief The reference voltage comes from internal or external
 *
 * @note Also see `analog_cmpr_ll_set_internal_ref_voltage` to use the internal reference voltage
 *
 * @param hw Analog comparator register base address
 * @param ref_src reference source, 0 for internal, 1 for external GPIO pad
 */
static inline void analog_cmpr_ll_set_ref_source(analog_cmpr_dev_t *hw, ana_cmpr_ref_source_t ref_src)
{
    hw->det_pad_comp_cfg.det_pad_comp_mode = ref_src;
}

/**
 * @brief Set the debounce cycle for the cross detection
 *
 * @note When the comparator detects a cross, it will wait for the debounce cycle to make sure the cross is stable.
 *
 * @param hw Analog comparator register base address
 * @param cycle The debounce cycle
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_cross_debounce_cycle(analog_cmpr_dev_t *hw, uint32_t cycle)
{
    // the value must be greater than or equal to 1
    hw->det_filter_cnt.det_filter_cnt = MAX(cycle, 1);
}

/**
 * @brief Enable comparator interrupt
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt mask
 * @param enable True to enable, False to disable
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_enable_intr(analog_cmpr_dev_t *hw, uint32_t mask, bool enable)
{
    uint32_t val = hw->det_int_ena.val;
    if (enable) {
        val |= mask;
    } else {
        val &= ~mask;
    }
    hw->det_int_ena.val = val;
}

/**
 * @brief Get comparator interrupt status
 *
 * @param hw Analog comparator register base address
 */
__attribute__((always_inline))
static inline uint32_t analog_cmpr_ll_get_intr_status(analog_cmpr_dev_t *hw)
{
    return hw->det_int_st.val & ANALOG_CMPR_LL_ALL_INTR_MASK(0);
}

/**
 * @brief Clear comparator interrupt status
 *
 * @param hw Analog comparator register base address
 * @param mask Interrupt status word
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_clear_intr(analog_cmpr_dev_t *hw, uint32_t mask)
{
    hw->det_int_clr.val = mask;
}

/**
 * @brief Get the interrupt status register address
 *
 * @param hw Analog comparator register base address
 * @return The interrupt status register address
 */
static inline volatile void *analog_cmpr_ll_get_intr_status_reg(analog_cmpr_dev_t *hw)
{
    return &hw->det_int_st;
}

/**
 * @brief Set external reference PAD id
 *
 * @param hw Analog comparator register base address
 * @param vref_pad_id PAD id (0..3)
 */
static inline void analog_cmpr_ll_set_ext_ref_pad(analog_cmpr_dev_t *hw, uint32_t vref_pad_id)
{
    hw->det_conf.det_vref_channel_sel = 1 << vref_pad_id;
}

/**
 * @brief Set comparator source PAD id for one source channel
 *
 * @param hw Analog comparator register base address
 * @param src_chan Source channel id (0..2)
 * @param src_pad_id PAD id (0..3)
 */
static inline void analog_cmpr_ll_set_src_pad(analog_cmpr_dev_t *hw, uint32_t src_chan, uint32_t src_pad_id)
{
    uint32_t val = 1 << src_pad_id;
    switch (src_chan) {
    case 0:
        hw->det_conf.det_comp_channel_1_sel = val;
        break;
    case 1:
        hw->det_conf.det_comp_channel_2_sel = val;
        break;
    default:
        hw->det_conf.det_comp_channel_3_sel = val;
        break;
    }
}

/**
 * @brief Set scan channel mask
 *
 * @param hw Analog comparator register base address
 * @param poll_mask Channel mask (bit0..bit2 => CH1..CH3)
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_scan_mask(analog_cmpr_dev_t *hw, uint32_t poll_mask)
{
    hw->det_conf.det_comp_poll_mask = poll_mask & 0x7;
}

/**
 * @brief Set scan mode
 *
 * @param hw Analog comparator register base address
 * @param scan_mode Scan mode, see `ana_cmpr_scan_mode_t`
 */
static inline void analog_cmpr_ll_set_scan_mode(analog_cmpr_dev_t *hw, ana_cmpr_scan_mode_t scan_mode)
{
    hw->det_conf.det_comp_poll_mode = scan_mode;
}

/**
 * @brief Start a scan to detect the cross event
 *
 * @param dev Analog comparator register base address
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_start_scan(analog_cmpr_dev_t *dev)
{
    // enable ETM register clock
    PCR.etm_conf.etm_clk_en = 1;
    while (PCR.etm_conf.etm_ready == 0) {
    }

    int trigger_times = dev->det_conf.det_limit_cnt;
    // use reg_etm_date[31] register to trigger analog comparator to start
    while (trigger_times--) {
        SOC_ETM.etm_date.val |= 1UL << 31;
        esp_rom_delay_us(10);
    }
}

/**
 * @brief Set channel switch wait cycles
 *
 * @param hw Analog comparator register base address
 * @param period_cycles Wait cycles in PAD_COMP_CLK domain
 */
static inline void analog_cmpr_ll_set_poll_period(analog_cmpr_dev_t *hw, uint32_t period_cycles)
{
    // the value must be greater than or equal to 1
    hw->det_poll_period.det_comp_poll_period = MAX(period_cycles, 1);
}

/**
 * @brief Set the resample limit for the cross detection
 *
 * @note If the current status is high, N consecutive low levels need to be sampled before switching to low, vice versa.
 *
 * @param hw Analog comparator register base address
 * @param limit_cnt The resample limit count
 */
static inline void analog_cmpr_ll_set_resample_limit(analog_cmpr_dev_t *hw, uint8_t limit_cnt)
{
    limit_cnt = MAX(limit_cnt, 1);
    hw->det_conf.det_limit_cnt = limit_cnt;
}

/**
 * @brief Set ETM delay event cycles
 *
 * @param hw Analog comparator register base address
 * @param delay_cycles Delay cycles in PAD_COMP_CLK domain
 */
static inline void analog_cmpr_ll_set_etm_delay_cycles(analog_cmpr_dev_t *hw, uint32_t delay_cycles)
{
    // the value must be greater than or equal to 1
    hw->det_delay_event_time.det_delay_event_time = MAX(delay_cycles, 1);
}

/**
 * @brief Enable per-channel ETM event
 *
 * @param hw Analog comparator register base address
 * @param src_chan Source channel id (0..2)
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_channel_etm(analog_cmpr_dev_t *hw, uint32_t src_chan, bool enable)
{
    switch (src_chan) {
    case 0:
        hw->det_conf.det_channel_1_event_timer_en = enable;
        break;
    case 1:
        hw->det_conf.det_channel_2_event_timer_en = enable;
        break;
    default:
        hw->det_conf.det_channel_3_event_timer_en = enable;
        break;
    }
}

/**
 * @brief Enable channel capture timer for cross detection time stamping
 *
 * @param hw Analog comparator register base address
 * @param enable true to enable, false to disable
 */
static inline void analog_cmpr_ll_enable_capture_timer(analog_cmpr_dev_t *hw, bool enable)
{
    hw->det_conf.det_channel_timer_en = enable;
}

/**
 * @brief Set hysteresis level
 *
 * @param hw Analog comparator register base address
 * @param hys_level Hysteresis level enum
 */
__attribute__((always_inline))
static inline void analog_cmpr_ll_set_ref_hys_level(analog_cmpr_dev_t *hw, ana_cmpr_ref_hys_t hys_level)
{
    switch (hys_level) {
    case ANA_CMPR_REF_HYS_LEVEL0:
        hw->det_pad_comp_cfg.det_pad_comp_hys = 0;
        break;
    case ANA_CMPR_REF_HYS_LEVEL1:
        hw->det_pad_comp_cfg.det_pad_comp_hys = 1;
        break;
    case ANA_CMPR_REF_HYS_LEVEL2:
        hw->det_pad_comp_cfg.det_pad_comp_hys = 2;
        break;
    case ANA_CMPR_REF_HYS_LEVEL3:
        hw->det_pad_comp_cfg.det_pad_comp_hys = 4;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    hw->det_pad_comp_cfg.det_pad_comp_hys_en = hys_level != ANA_CMPR_REF_HYS_LEVEL0;
}

/**
 * @brief Get the current compare result of one source channel
 *
 * @param hw Analog comparator register base address
 * @param src_chan Source channel id (0..2)
 * @return true if the source signal is higher than the reference signal, false otherwise
 */
static inline bool analog_cmpr_ll_get_compare_result(analog_cmpr_dev_t *hw, uint32_t src_chan)
{
    switch (src_chan) {
    case 0:
        return hw->det_channel_status.det_channel_1_pad_comp_status;
    case 1:
        return hw->det_channel_status.det_channel_2_pad_comp_status;
    default:
        return hw->det_channel_status.det_channel_3_pad_comp_status;
    }
}

/**
 * @brief Get the current capture timer value for a source channel, which can be used for time stamping the cross event
 *
 * @param hw Analog comparator register base address
 * @param src_chan Source channel id (0..2)
 * @return Current capture timer value in cycles of PAD_COMP_CLK
 */
static inline uint32_t analog_cmpr_ll_get_current_capture_time(analog_cmpr_dev_t *hw, uint32_t src_chan)
{
    return hw->det_channel_timers[src_chan][0].det_channel_timer;
}

/**
 * @brief Get the previous capture timer value for a source channel, which is the timestamp of the last cross event that triggered an interrupt
 *
 * @param hw Analog comparator register base address
 * @param src_chan Source channel id (0..2)
 * @return Previous capture timer value in cycles of PAD_COMP_CLK
 */
static inline uint32_t analog_cmpr_ll_get_previous_capture_time(analog_cmpr_dev_t *hw, uint32_t src_chan)
{
    return hw->det_channel_timers[src_chan][1].det_channel_timer;
}

#ifdef __cplusplus
}
#endif
