/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The LL layer for LP I2S register operations
/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once
#include <stdbool.h>
#include <math.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/lp_i2s_struct.h"
#include "soc/lpperi_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2S_LL_GET_HW(num)                      (((num) == 0)? (&LP_I2S) : NULL)

#define LP_I2S_LL_EVENT_RX_DONE_INT             (1<<0)
#define LP_I2S_LL_EVENT_RX_HUNG_INT_INT         (1<<1)
#define LP_I2S_LL_EVENT_RX_FIFOMEM_UDF_INT      (1<<2)
#define LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT    (1<<5)

#define LP_I2S_LL_TDM_CH_MASK                   (0x03UL)

/**
 * @brief LP I2S rx stop mode enum type
 */
typedef enum {
    LP_I2S_LL_RX_STOP_MODE_START_CLEAR,
    LP_I2S_LL_RX_STOP_MODE_START_CLEAR_EOF_NUM,
} lp_i2s_ll_rx_stop_mode_t;

/**
 * @brief LP I2S rx polarity mode enum type
 */
typedef enum {
    LP_I2S_LL_RX_WS_POL_NEGATIVE,
    LP_I2S_LL_RX_WS_POL_POSITIVE,
} lp_i2s_ll_rx_ws_pol_t;

/**
 * @brief LP I2S rx bit order enum type
 */
typedef enum {
    LP_I2S_LL_RX_BIT_ORDER_BIG_ENDIAN,
    LP_I2S_LL_RX_BIT_ORDER_SMALL_ENDIAN,
} lp_i2s_ll_rx_bit_order_t;

/**
 * @brief LP I2S rx align enum type
 */
typedef enum {
    LP_I2S_LL_RX_RIGHT_ALIGN,
    LP_I2S_LL_RX_LEFT_ALIGN,
} lp_i2s_ll_rx_alignment_t;

/**
 * @brief LP I2S rx endian enum type
 */
typedef enum {
    LP_I2S_LL_RX_SMALL_ENDIAN,
    LP_I2S_LL_RX_BIG_ENDIAN,
} lp_i2s_ll_rx_endian_t;

/*---------------------------------------------------------------
                      Clock
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for LP I2S module
 *
 * @param id    Instance id
 * @param en    enable / disable
 */
static inline void lp_i2s_ll_enable_module_clock(int id, bool en)
{
    LPPERI.clk_en.ck_en_lp_i2s = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_i2s_ll_enable_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2s_ll_enable_module_clock(__VA_ARGS__)

/**
 * @brief Reset the LP I2S module
 *
 * @param hw    Hardware instance address
 */
static inline void lp_i2s_ll_reset_module_clock(int id)
{
    LPPERI.reset_en.rst_en_lp_i2s = 1;
    LPPERI.reset_en.rst_en_lp_i2s = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_i2s_ll_reset_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2s_ll_reset_module_clock(__VA_ARGS__)

/**
 * @brief Enable the bus clock for LP I2S RX module
 *
 * @param id    Instance id
 * @param en    enable / disable
 */
static inline void lp_i2s_ll_enable_rx_module_clock(int id, bool en)
{
    LPPERI.clk_en.ck_en_lp_i2s_rx = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_i2s_ll_enable_rx_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2s_ll_enable_rx_module_clock(__VA_ARGS__)

/**
 * @brief Set LP I2S clock source div num
 *
 * @note f_LP_I2S_CLK_S / (N + b / a) = f_LP_I2S_RX_CLK
 *
 * @param id    Instance id
 * @param val   value to set div num
 */
static inline void lp_i2s_ll_clk_source_div_num(int id, uint32_t val)
{
    if (val == 256) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LPPERI.lp_i2s_rxclk_div_num, lp_i2s_rx_clkm_div_num, 0);
    } else if (val == 2) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LPPERI.lp_i2s_rxclk_div_num, lp_i2s_rx_clkm_div_num, 1);
    } else {
        HAL_FORCE_MODIFY_U32_REG_FIELD(LPPERI.lp_i2s_rxclk_div_num, lp_i2s_rx_clkm_div_num, val);
    }
}

/**
 * @brief Set LP I2S rx raw clock division
 *
 * @param id    Instance id
 * @param a     div a
 * @param b     div b
 */
static inline void i2s_ll_tx_set_raw_clk_div(int id, uint32_t a, uint32_t b)
{
    if (b <= a / 2) {
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_yn1 = 0;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_x = floor(a / b) - 1;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_y = a % b;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_z = b;
    } else {
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_yn1 = 1;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_x = floor(a / (a - b)) - 1;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_y = a % (a - b);
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_z = a - b;
    }

    if (a == 0 && b == 0) {
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_x = 0;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_z = 0;
        LPPERI.lp_i2s_rxclk_div_xyz.lp_i2s_rx_clkm_div_y = 1;
    }
}

/**
 * @brief Set LP I2S rx bck div num
 *
 * @param id    Instance id
 * @param val   value to set rx bck div num
 */
static inline void lp_i2s_ll_rx_set_bck_div_num(lp_i2s_dev_t *hw, uint32_t val)
{
    hw->rx_conf1.rx_bck_div_num = val - 1;
}

/*---------------------------------------------------------------
                      Reset
---------------------------------------------------------------*/
/**
 * @brief Reset LP I2S RX module
 *
 * @param id    Instance id
 */
static inline void lp_i2s_ll_rx_reset(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_reset = 1;
    hw->rx_conf.rx_reset = 0;
}

/**
 * @brief Reset LP I2S RX FIFO
 *
 * @param id    Instance id
 */
static inline void lp_i2s_ll_rx_reset_fifo(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_fifo_reset = 1;
    hw->rx_conf.rx_fifo_reset = 0;
}

/*---------------------------------------------------------------
                      Master/Slave
---------------------------------------------------------------*/
/**
 * @brief Start LP I2S RX
 *
 * @param id    Instance id
 */
static inline void lp_i2s_ll_rx_start(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_update = 1;
    while (hw->rx_conf.rx_update);
    hw->rx_conf.rx_start = 1;
}

/**
 * @brief Set LP I2S RX stop mode
 *
 * @param id    Instance id
 */
static inline void lp_i2s_ll_set_rx_stop_mode(lp_i2s_dev_t *hw, lp_i2s_ll_rx_stop_mode_t rx_stop_mode)
{
    hw->rx_conf.rx_stop_mode = rx_stop_mode;
}

/**
 * @brief Configure the received length to trigger in_suc_eof interrupt
 *
 * @param id       Instance id
 * @param eof_num  the byte length to trigger in_suc_eof interrupt
 */
static inline void lp_i2s_ll_rx_set_eof_num(lp_i2s_dev_t *hw, int eof_num)
{
    hw->rxeof_num.rx_eof_num = eof_num;
}

/**
 * @brief Stop LP I2S RX
 *
 * @param id    Instance id
 */
static inline void lp_i2s_ll_rx_stop(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_start = 0;
}

/*---------------------------------------------------------------
                      Receive
---------------------------------------------------------------*/
/*--------------------------------------------------
                      TDM
--------------------------------------------------*/
/**
 * @brief Enable LP I2S RX TDM mode
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_enable_tdm(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_pdm_en = false;
    hw->rx_conf.rx_tdm_en = true;
}

/**
 * @brief Configure RX total chan number under TDM mode
 *
 * @param hw         LP I2S hardware instance
 * @param total_num  Total chan number
 */
static inline void lp_i2s_ll_rx_set_tdm_chan_num(lp_i2s_dev_t *hw, int total_num)
{
    hw->rx_tdm_ctrl.rx_tdm_tot_chan_num = total_num - 1;
}

/*--------------------------------------------------
                      PDM
--------------------------------------------------*/
/**
 * @brief Enable LP RX PDM mode.
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_enable_pdm(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_pdm_en = 1;
    hw->rx_conf.rx_tdm_en = 0;
}

/*---------------------------------------------------------------
                      General
---------------------------------------------------------------*/
/**
 * @brief Configure LP I2S rx channel bits and bits mode
 */
static inline void i2s_ll_rx_set_sample_bit(lp_i2s_dev_t *hw, int chan_bits, int bits_mode)
{
    hw->rx_conf1.rx_tdm_chan_bits = chan_bits - 1;
    hw->rx_conf1.rx_bits_mod = bits_mode - 1;
}

/**
 * @brief Set LP I2S rx ws polarity
 *
 * @param hw         LP I2S hardware instance
 * @param pol        ws polarity
 */
static inline void lp_i2s_ll_rx_set_ws_pol(lp_i2s_dev_t *hw, lp_i2s_ll_rx_ws_pol_t pol)
{
    hw->rx_conf.rx_ws_idle_pol = pol;
}

/**
 * @brief Set LP I2S rx ws width
 *
 * @param hw         LP I2S hardware instance
 * @param width      ws width
 */
static inline void lp_i2s_ll_rx_set_ws_width(lp_i2s_dev_t *hw, int width)
{
    hw->rx_conf1.rx_tdm_ws_width = width - 1;
}

/**
 * @brief Set LP I2S rx half sample bits
 *
 * @param hw                LP I2S hardware instance
 * @param half_sample_bits  Half sample bits
 */
static inline void lp_i2s_ll_rx_set_half_sample_bits(lp_i2s_dev_t *hw, int half_sample_bits)
{
    hw->rx_conf1.rx_half_sample_bits = half_sample_bits - 1;
}

/**
 * @brief Enable LP I2S RX set active channul
 *
 * @param hw      LP I2S hardware instance
 * @param chanid  channel id
 * @param enable  enable or disable
 */
static inline void lp_i2s_ll_rx_set_active_chan(lp_i2s_dev_t *hw, uint32_t chan_mask)
{
    uint32_t tdm_ctrl = hw->rx_tdm_ctrl.val;
    tdm_ctrl &= ~LP_I2S_LL_TDM_CH_MASK;
    tdm_ctrl |= (chan_mask & LP_I2S_LL_TDM_CH_MASK);
    hw->rx_tdm_ctrl.val = tdm_ctrl;
}

/**
 * @brief Set LP I2S rx pdm2pcm enable
 *
 * @param hw                LP I2S hardware instance
 * @param en                enable or disable
 */
static inline void lp_i2s_ll_rx_set_pdm2pcm_en(lp_i2s_dev_t *hw, bool en)
{
    hw->rx_pdm_conf.rx_pdm2pcm_en = en;
}

/**
 * @brief Set LP I2S rx bit order
 *
 * @param hw         LP I2S hardware instance
 * @param bit_order  bit order
 */
static inline void lp_i2s_ll_rx_set_bit_order(lp_i2s_dev_t *hw, lp_i2s_ll_rx_bit_order_t bit_order)
{
    hw->rx_conf.rx_bit_order = bit_order;
}

/**
 * @brief Enable LP I2S 24 fill
 *
 * @param hw         LP I2S hardware instance
 * @param en         enable or disable
 */
static inline void lp_i2s_ll_rx_enable_24_fill(lp_i2s_dev_t *hw, bool en)
{
    hw->rx_conf.rx_24_fill_en = en;
}

/**
 * @brief Set LP I2S rx alignment mode
 *
 * @param hw         LP I2S hardware instance
 * @param align      align
 */
static inline void lp_i2s_ll_rx_set_alignment_mode(lp_i2s_dev_t *hw, lp_i2s_ll_rx_alignment_t align)
{
    hw->rx_conf.rx_left_align = align;
}

/**
 * @brief Set LP I2S rx endian
 *
 * @param hw         LP I2S hardware instance
 * @param endian     endian
 */
static inline void lp_i2s_ll_rx_set_endian(lp_i2s_dev_t *hw, lp_i2s_ll_rx_endian_t endian)
{
    hw->rx_conf.rx_big_endian = endian;
}

/*---------------------------------------------------------------
                      Interrupt
---------------------------------------------------------------*/
/**
 * @brief Get LP I2S RX interrupt status word
 *
 * @param hw         LP I2S hardware instance
 * @param raw        raw or status
 */
__attribute__((always_inline))
static inline uint32_t lp_i2s_ll_rx_get_interrupt_status(lp_i2s_dev_t *hw, bool raw)
{
    if (raw) {
        return hw->int_raw.val;
    } else {
        return hw->int_st.val;
    }
}

/**
 * @brief Enable LP I2S RX channel interrupt
 *
 * @param hw         LP I2S hardware instance
 * @param mask       mask
 * @param enable     enable or disable
 */
static inline void lp_i2s_ll_rx_enable_interrupt(lp_i2s_dev_t *hw, uint32_t mask, bool enable)
{
    uint32_t int_ena = hw->int_ena.val;
    if (enable) {
        int_ena |= mask;
    } else {
        int_ena &= ~mask;
    }
    hw->int_ena.val = int_ena;
}

/**
 * @brief Clear LP I2S RX channel interrupt
 *
 * @param hw         LP I2S hardware instance
 * @param mask       mask
 */
__attribute__((always_inline))
static inline void lp_i2s_ll_rx_clear_interrupt_status(lp_i2s_dev_t *hw, uint32_t mask)
{
    hw->int_clr.val = mask;
}

#ifdef __cplusplus
}
#endif
