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
#include "hal/i2s_types.h"
#include "soc/lp_i2s_struct.h"
#include "soc/lpperi_struct.h"
#include "soc/lp_system_struct.h"
#include "soc/clk_tree_defs.h"
#include "soc/reg_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LP_I2S_LL_GET_HW(num)                   (((num) == 0)? (&LP_I2S) : NULL)

#define LP_I2S_LL_EVENT_RX_DONE_INT             (1<<0)
#define LP_I2S_LL_EVENT_RX_HUNG_INT_INT         (1<<1)
#define LP_I2S_LL_EVENT_RX_FIFOMEM_UDF_INT      (1<<2)
#define LP_I2S_LL_EVENT_RX_MEM_THRESHOLD_INT    (1<<5)
#define LP_I2S_LL_TDM_CH_MASK                   (0x03UL)

#define LP_I2S_LL_RX_SUPPORTED                  1

#define LP_I2S_LL_TDM_MAX_DATA_BIT_WIDTH        16
#define LP_I2S_LL_TDM_MAX_CHAN_BIT_WIDTH        16
#define LP_I2S_LL_RX_MEM_THRESH_BYTES_MAX       1020
#define LP_I2S_LL_RX_MEM_POP_BYTES              4

#define LP_I2S_LL_CUT_OFF_COEF_X_NUM            21
#define LP_I2S_LL_CUT_OFF_COEF_Y_NUM            3
/* PDM high pass filter cut-off frequency and coefficients list
 * [0]: cut-off frequency * 10; [1]: param0; [2]: param5
 * NOTE: the cut-off frequency was timed 10 to use integer type */
#define LP_I2S_LL_CUT_OFF_COEF                  {{1850, 0, 0}, {1720,  0, 1}, {1600,  1, 1}, \
                                                {1500, 1, 2}, {1370,  2, 2}, {1260,  2, 3}, \
                                                {1200, 0, 3}, {1150,  3, 3}, {1060,  1, 7}, \
                                                {1040, 2, 4}, {920,   4, 4}, {915, 2, 7}, \
                                                {810,  4, 5}, {772, 3, 7}, {690,   5, 5}, \
                                                {630,  4, 7}, {580,   5, 6}, {490,   5, 7}, \
                                                {460,  6, 6}, {355, 6, 7}, {233, 7, 7}}

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
                      Mem
---------------------------------------------------------------*/
/**
 * @brief Enable the internal memory for LP I2S module
 *
 * @param id    Instance id
 * @param en    enable / disable
 */
static inline void lp_i2s_ll_enable_mem(int id, bool en)
{
    LP_SYS.lp_mem_aux_ctrl.lp_mem_aux_ctrl = !en;
}

static inline void lp_i2s_ll_set_rx_mem_threshold(lp_i2s_dev_t *hw, uint32_t words)
{
    LP_I2S.rx_mem_conf.rx_mem_threshold = words;
}

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
 * @brief Select ISP clock source
 *
 * @param id    Instance id
 * @param clk_src  clock source, see valid sources in type `soc_periph_lp_i2s_clk_src_t`
 */
static inline void lp_i2s_ll_select_rx_clk_source(int id, soc_periph_lp_i2s_clk_src_t clk_src)
{
    uint8_t clk_val = 0;
    switch (clk_src) {
    case LP_I2S_CLK_SRC_LP_PERI:
        clk_val = 0;
        break;
    case LP_I2S_CLK_SRC_XTAL_D2:
        clk_val = 1;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    LPPERI.core_clk_sel.lp_i2s_rx_clk_sel = clk_val;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define lp_i2s_ll_select_rx_clk_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; lp_i2s_ll_select_rx_clk_source(__VA_ARGS__)

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
static inline void lp_i2s_ll_rx_set_raw_clk_div(int id, uint32_t a, uint32_t b)
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
 * @param hw    LP I2S hardware instance
 * @param val   value to set rx bck div num
 */
static inline void lp_i2s_ll_rx_set_bck_div_num(lp_i2s_dev_t *hw, uint32_t val)
{
    hw->rx_conf1.rx_bck_div_num = val - 1;
}

/**
 * @brief Enable/Disable clock gate
 *
 * @param hw     LP I2S hardware instance
 * @param enable enable or disable
 */
static inline void lp_i2s_ll_clk_gate_en(lp_i2s_dev_t *hw, bool enable)
{
    hw->clk_gate.clk_en = enable;
}

/**
 * @brief Enable/Disable rx mem clock gate
 *
 * @param hw     LP I2S hardware instance
 * @param enable enable or disable
 */
static inline void lp_i2s_ll_rx_mem_clk_gate_en(lp_i2s_dev_t *hw, bool enable)
{
    hw->clk_gate.rx_mem_cg_force_on = enable;
}

/**
 * @brief Enable/Disable rx reg clock gate
 *
 * @param hw     LP I2S hardware instance
 * @param enable enable or disable
 */
static inline void lp_i2s_ll_rx_reg_clk_gate_en(lp_i2s_dev_t *hw, bool enable)
{
    hw->clk_gate.rx_reg_cg_force_on = enable;
}

/*---------------------------------------------------------------
                      Reset
---------------------------------------------------------------*/
/**
 * @brief Reset LP I2S RX module
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_reset(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_reset = 1;
    hw->rx_conf.rx_reset = 0;
}

/**
 * @brief Reset LP I2S RX FIFO
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_reset_fifo(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_fifo_reset = 1;
    hw->rx_conf.rx_fifo_reset = 0;
}

/**
 * @brief Reset LP I2S RX FIFO mem
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_reset_fifo_mem(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_fifomem_reset = 1;
    hw->rx_conf.rx_fifomem_reset = 0;
}

/*---------------------------------------------------------------
                      Master/Slave
---------------------------------------------------------------*/
/**
 * @brief Start LP I2S RX
 *
 * @param hw         LP I2S hardware instance
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
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_set_rx_stop_mode(lp_i2s_dev_t *hw, lp_i2s_ll_rx_stop_mode_t rx_stop_mode)
{
    hw->rx_conf.rx_stop_mode = rx_stop_mode;
}

/**
 * @brief Configure the received length to trigger in_suc_eof interrupt
 *
 * @param hw         LP I2S hardware instance
 * @param eof_num  the byte length to trigger in_suc_eof interrupt
 */
static inline void lp_i2s_ll_rx_set_eof_num(lp_i2s_dev_t *hw, int eof_num)
{
    hw->rxeof_num.rx_eof_num = eof_num;
}

/**
 * @brief Stop LP I2S RX
 *
 * @param hw         LP I2S hardware instance
 */
static inline void lp_i2s_ll_rx_stop(lp_i2s_dev_t *hw)
{
    hw->rx_conf.rx_start = 0;
}

/**
 * @brief Stop LP I2S RX
 *
 * @param hw         LP I2S hardware instance
 * @param is_slave   Is slave or master
 */
static inline void lp_i2s_ll_set_rx_master_slave_mode(lp_i2s_dev_t *hw, bool is_slave)
{
    hw->rx_conf.rx_slave_mod = is_slave;
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

/**
 * @brief Enable LP I2S RX PDM high pass filter
 *
 * @param hw      LP I2S hardware instance
 * @param enable  Set true to enable I2S RX PDM high pass filter, set false to bypass it
 */
static inline void lp_i2s_ll_rx_enable_pdm_hp_filter(lp_i2s_dev_t *hw, bool enable)
{
    hw->rx_pdm_conf.rx_pdm_hp_bypass = !enable;
}

/**
 * @brief Set LP_I2S RX PDM high pass filter param0
 *
 * @param hw     Peripheral LP_I2S hardware instance address.
 * @param param  The fourth parameter of PDM RX IIR_HP filter stage 1 is (504 + LP_I2S_RX_IIR_HP_MULT12_0[2:0])
 */
static inline void lp_i2s_ll_rx_set_pdm_hp_filter_param0(lp_i2s_dev_t *hw, uint32_t param)
{
    hw->rx_pdm_conf.rx_iir_hp_mult12_0 = param;
}

/**
 * @brief Set LP I2S RX PDM high pass filter param5
 *
 * @param hw     Peripheral LP_I2S hardware instance address.
 * @param param  The fourth parameter of PDM RX IIR_HP filter stage 2 is (504 + LP_I2S_RX_IIR_HP_MULT12_5[2:0])
 */
static inline void lp_i2s_ll_rx_set_pdm_hp_filter_param5(lp_i2s_dev_t *hw, uint32_t param)
{
    hw->rx_pdm_conf.rx_iir_hp_mult12_5 = param;
}

/**
 * @brief Configure RX PDM amplify number
 * @note  This is the amplification number of the digital amplifier,
 *        which is added after the PDM to PCM conversion result and mainly used for
 *        amplify the small PDM signal under the VAD scenario
 *        pcm_result = pdm_input * amplify_num
 *        pcm_result = 0 if amplify_num = 0
 *
 * @param hw       Peripheral LP_I2S hardware instance address.
 * @param amp_num  PDM RX amplify number
 */
static inline void lp_i2s_ll_rx_set_pdm_amplify_num(lp_i2s_dev_t *hw, uint32_t amp_num)
{
    hw->rx_pdm_conf.rx_pdm2pcm_amplify_num = amp_num;
}

/*---------------------------------------------------------------
                      General
---------------------------------------------------------------*/
/**
 * @brief Configure LP I2S rx channel bits and bits mode
 *
 * @param hw         LP I2S hardware instance
 * @param chan_bits  Chan bits
 * @param bits_mode  Bits mode
 */
static inline void lp_i2s_ll_rx_set_sample_bit(lp_i2s_dev_t *hw, int chan_bits, int bits_mode)
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
 * @brief Set LP I2S rx std channels
 *
 * @param hw         LP I2S hardware instance
 * @param chan_mask  select chan to receive data
 */
static inline void lp_i2s_ll_rx_select_std_chan(lp_i2s_dev_t *hw, i2s_std_slot_mask_t chan_mask)
{
    hw->rx_tdm_ctrl.rx_tdm_tot_chan_num = 1;
    uint32_t mask = 0;
    switch (chan_mask)
    {
    case I2S_STD_SLOT_LEFT:
        mask = 0x01;
        break;
    case I2S_STD_SLOT_RIGHT:
        mask = 0x02;
        break;
    case I2S_STD_SLOT_BOTH:
        mask = 0x03;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
    lp_i2s_ll_rx_set_active_chan(hw, mask);
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

/**
 * @brief Enable RX mono mode
 *
 * @param hw        LP I2S hardware instance
 * @param mono_ena  Set true to enable mono mde.
 */
static inline void lp_i2s_ll_rx_enable_mono_mode(lp_i2s_dev_t *hw, bool mono_ena)
{
    hw->rx_conf.rx_mono = mono_ena;
    hw->rx_conf.rx_mono_fst_vld = mono_ena;
}

/**
 * @brief Enable RX MSB shift, the data will be launch at the first BCK clock
 *
 * @param hw                LP I2S hardware instance
 * @param msb_shift_enable  Set true to enable MSB shift
 */
static inline void lp_i2s_ll_rx_enable_msb_shift(lp_i2s_dev_t *hw, bool msb_shift_enable)
{
    hw->rx_conf1.rx_msb_shift = msb_shift_enable;
}

/**
 * Read from LP I2S hardware data buffer.
 *
 * @param hw            Beginning address of the peripheral registers.
 * @param buffer_to_rcv Address of a buffer to read data from hardware data buffer
 * @param byte_len      Length to read, in bytes.
 */
static inline void lp_i2s_ll_read_buffer(lp_i2s_dev_t *hw, void *buffer_to_rcv, size_t byte_len)
{
    for (int i = 0; i < byte_len; i += 4) {
        *(uint32_t *)(buffer_to_rcv + i) = *((uint32_t *)LP_I2S_RAM_BASE);
    }
}

/**
 * Write from LP I2S hardware data buffer.
 *
 * @param hw                    Beginning address of the peripheral registers.
 * @param buffer_to_rcv         Address of a buffer to write data from hardware data buffer
 * @param bitlbyte_lenen        Length to write, in bytes.
 */
static inline void lp_i2s_ll_write_buffer(lp_i2s_dev_t *hw, const void *buffer_to_send, size_t byte_len)
{
    for (int i = 0; i < byte_len; i += 4) {
        //Use memcpy to get around alignment issues for txdata
        *((uint32_t *)LP_I2S_RAM_BASE) = *(uint32_t *)(buffer_to_send + i);
    }
}

/**
 * Get RX mem fifo cnt, in bytes
 *
 * @param hw            Beginning address of the peripheral registers.
 */
static inline uint32_t lp_i2s_ll_get_rx_mem_fifo_cnt(lp_i2s_dev_t *hw)
{
    return hw->rx_mem_conf.rx_mem_fifo_cnt * 4;
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
 * @brief Get interrupt status reg address
 *
 * @param[in] hw  LP I2S hardware instance
 *
 * @return Interrupt status reg address
 */
static inline uint32_t lp_i2s_ll_get_intr_status_reg_addr(lp_i2s_dev_t *hw)
{
    return (uint32_t)&(hw->int_st);
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
