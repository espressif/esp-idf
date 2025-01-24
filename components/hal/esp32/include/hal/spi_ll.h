/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32 SPI register operations

#pragma once

#include <string.h>
#include <stdlib.h> //for abs()
#include "esp_types.h"
#include "esp32/rom/lldesc.h"
#include "soc/spi_periph.h"
#include "soc/spi_struct.h"
#include "soc/dport_reg.h"
#include "hal/misc.h"
#include "hal/spi_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Registers to reset during initialization. Don't use in app.
#define SPI_LL_DMA_FIFO_RST_MASK (SPI_AHBM_RST | SPI_AHBM_FIFO_RST)
/// Interrupt not used. Don't use in app.
#define SPI_LL_UNUSED_INT_MASK  (SPI_INT_EN | SPI_SLV_WR_STA_DONE | SPI_SLV_RD_STA_DONE | SPI_SLV_WR_BUF_DONE | SPI_SLV_RD_BUF_DONE)
/// These 2 masks together will set SPI transaction to one line mode
#define SPI_LL_ONE_LINE_CTRL_MASK (SPI_FREAD_DUAL | SPI_FREAD_QUAD | SPI_FREAD_DIO | SPI_FREAD_QIO)
#define SPI_LL_ONE_LINE_USER_MASK (SPI_FWRITE_DUAL | SPI_FWRITE_QUAD | SPI_FWRITE_DIO | SPI_FWRITE_QIO)
/// Swap the bit order to its correct place to send
#define HAL_SPI_SWAP_DATA_TX(data, len) HAL_SWAP32((uint32_t)(data) << (32 - len))
#define SPI_LL_GET_HW(ID) ((ID)==0? &SPI1:((ID)==1? &SPI2 : &SPI3))

#define SPI_LL_DMA_MAX_BIT_LEN    (1 << 24)    //reg len: 24 bits
#define SPI_LL_CPU_MAX_BIT_LEN    (16 * 32)    //Fifo len: 16 words
#define SPI_LL_MOSI_FREE_LEVEL    0            //Default level after bus initialized

/**
 * The data structure holding calculated clock configuration. Since the
 * calculation needs long time, it should be calculated during initialization and
 * stored somewhere to be quickly used.
 */
typedef uint32_t spi_ll_clock_val_t;

//On ESP32-S2 and earlier chips, DMA registers are part of SPI registers. So set the registers of SPI peripheral to control DMA.
typedef spi_dev_t spi_dma_dev_t;

/*------------------------------------------------------------------------------
 * Control
 *----------------------------------------------------------------------------*/
/**
 * Enable peripheral register clock
 *
 * @param host_id   Peripheral index number, see `spi_host_device_t`
 * @param enable    Enable/Disable
 */
static inline void spi_ll_enable_bus_clock(spi_host_device_t host_id, bool enable) {
    if (enable) {
        switch (host_id)
        {
        case SPI1_HOST:
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI01_CLK_EN);
            break;
        case SPI2_HOST:
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
            break;
        case SPI3_HOST:
            DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
            break;
        default: HAL_ASSERT(false);
        }
    } else {
        switch (host_id)
        {
        case SPI1_HOST:
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI01_CLK_EN);
            break;
        case SPI2_HOST:
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI2_CLK_EN);
            break;
        case SPI3_HOST:
            DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI3_CLK_EN);
            break;
        default: HAL_ASSERT(false);
        }
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define spi_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; spi_ll_enable_bus_clock(__VA_ARGS__)

/**
 * Reset whole peripheral register to init value defined by HW design
 *
 * @param host_id   Peripheral index number, see `spi_host_device_t`
 */
static inline void spi_ll_reset_register(spi_host_device_t host_id) {
    switch (host_id)
    {
    case SPI1_HOST:
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI01_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI01_RST);
        break;
    case SPI2_HOST:
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI2_RST);
        break;
    case SPI3_HOST:
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI3_RST);
        break;
    default: HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define spi_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; spi_ll_reset_register(__VA_ARGS__)

/**
 * Enable functional output clock within peripheral
 *
 * @param host_id   Peripheral index number, see `spi_host_device_t`
 * @param enable    Enable/Disable
 */
static inline void spi_ll_enable_clock(spi_host_device_t host_id, bool enable)
{
    //empty, keep this for compatibility
}

/**
 * Select SPI peripheral clock source (master).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param clk_source clock source to select, see valid sources in type `spi_clock_source_t`
 */
__attribute__((always_inline))
static inline void spi_ll_set_clk_source(spi_dev_t *hw, spi_clock_source_t clk_source)
{
    //empty, keep this for compatibility
}

/**
 * Initialize SPI peripheral (master).
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_master_init(spi_dev_t *hw)
{
    //Reset timing
    hw->ctrl2.val = 0;

    //use all 64 bytes of the buffer
    hw->user.usr_miso_highpart = 0;
    hw->user.usr_mosi_highpart = 0;

    //Disable unneeded ints
    hw->slave.val &= ~SPI_LL_UNUSED_INT_MASK;
}

/**
 * Initialize SPI peripheral (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_init(spi_dev_t *hw)
{
    //Configure slave
    hw->clock.val = 0;
    hw->user.val = 0;
    hw->ctrl.val = 0;
    hw->slave.wr_rd_buf_en = 1; //no sure if needed
    hw->user.doutdin = 1; //we only support full duplex
    hw->user.sio = 0;
    hw->slave.slave_mode = 1;
    hw->slave.sync_reset = 1;
    hw->slave.sync_reset = 0;
    //use all 64 bytes of the buffer
    hw->user.usr_miso_highpart = 0;
    hw->user.usr_mosi_highpart = 0;

    //Disable unneeded ints
    hw->slave.val &= ~SPI_LL_UNUSED_INT_MASK;
}

/**
 * Check whether user-defined transaction is done.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return true if transaction is done, otherwise false.
 */
static inline bool spi_ll_usr_is_done(spi_dev_t *hw)
{
    return hw->slave.trans_done;
}

/**
 * Apply the register configurations and wait until it's done
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_apply_config(spi_dev_t *hw)
{
    // 32 don't need this option
}

/**
 * Trigger start of user-defined transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_user_start(spi_dev_t *hw)
{
    hw->cmd.usr = 1;
}

/**
 * Get current running command bit-mask. (Preview)
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return Bitmask of running command, see ``SPI_CMD_REG``. 0 if no in-flight command.
 */
static inline uint32_t spi_ll_get_running_cmd(spi_dev_t *hw)
{
    return hw->cmd.val;
}

/**
 * Reset SPI CPU TX FIFO
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_cpu_tx_fifo_reset(spi_dev_t *hw)
{
    //This is not used in esp32
}

/**
 * Reset SPI DMA FIFO
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_cpu_rx_fifo_reset(spi_dev_t *hw)
{
    //This is not used in esp32
}

/**
 * Reset SPI DMA TX FIFO
 *
 * On ESP32, this function is not separated
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_dma_tx_fifo_reset(spi_dev_t *hw)
{
    hw->dma_conf.val |= SPI_LL_DMA_FIFO_RST_MASK;
    hw->dma_conf.val &= ~SPI_LL_DMA_FIFO_RST_MASK;
}

/**
 * Reset SPI DMA RX FIFO
 *
 * On ESP32, this function is not separated
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_dma_rx_fifo_reset(spi_dev_t *hw)
{
    hw->dma_conf.val |= SPI_LL_DMA_FIFO_RST_MASK;
    hw->dma_conf.val &= ~SPI_LL_DMA_FIFO_RST_MASK;
}

/**
 * Clear in fifo full error
 *
 * @param hw Beginning address of the peripheral registers.
 */
__attribute__((always_inline))
static inline void spi_ll_infifo_full_clr(spi_dev_t *hw)
{
    //This is not used in esp32
}

/**
 * Clear out fifo empty error
 *
 * @param hw Beginning address of the peripheral registers.
 */
__attribute__((always_inline))
static inline void spi_ll_outfifo_empty_clr(spi_dev_t *hw)
{
    //This is not used in esp32
}

/*------------------------------------------------------------------------------
 * SPI configuration for DMA
 *----------------------------------------------------------------------------*/

/**
 * Enable/Disable RX DMA (Peripherals->DMA->RAM)
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: enable; 2: disable
 */
__attribute__((always_inline))
static inline void spi_ll_dma_rx_enable(spi_dev_t *hw, bool enable)
{
    //This is not used in esp32
}

/**
 * Enable/Disable TX DMA (RAM->DMA->Peripherals)
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: enable; 2: disable
 */
__attribute__((always_inline))
static inline void spi_ll_dma_tx_enable(spi_dev_t *hw, bool enable)
{
    //This is not used in esp32
}

/**
 * Configuration of RX DMA EOF interrupt generation way
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_done in seg-trans.
 */
static inline void spi_ll_dma_set_rx_eof_generation(spi_dev_t *hw, bool enable)
{
    //This is not used in esp32
}

/*------------------------------------------------------------------------------
 * Buffer
 *----------------------------------------------------------------------------*/
/**
 * Write to SPI buffer.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param buffer_to_send Data address to copy to the buffer.
 * @param bitlen Length to copy, in bits.
 */
static inline void spi_ll_write_buffer(spi_dev_t *hw, const uint8_t *buffer_to_send, size_t bitlen)
{
    for (size_t x = 0; x < bitlen; x += 32) {
        //Use memcpy to get around alignment issues for txdata
        uint32_t word;
        memcpy(&word, &buffer_to_send[x / 8], 4);
        hw->data_buf[(x / 32)] = word;
    }
}

/**
 * Read from SPI buffer.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param buffer_to_rcv Address to copy buffer data to.
 * @param bitlen Length to copy, in bits.
 */
static inline void spi_ll_read_buffer(spi_dev_t *hw, uint8_t *buffer_to_rcv, size_t bitlen)
{
    for (size_t x = 0; x < bitlen; x += 32) {
        //Do a memcpy to get around possible alignment issues in rx_buffer
        uint32_t word = hw->data_buf[x / 32];
        int len = bitlen - x;
        if (len > 32) {
            len = 32;
        }
        memcpy(&buffer_to_rcv[x / 8], &word, (len + 7) / 8);
    }
}

/*------------------------------------------------------------------------------
 * Configs: mode
 *----------------------------------------------------------------------------*/
/**
 * Enable/disable the postive-cs feature.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param cs One of the CS (0-2) to enable/disable the feature.
 * @param pos_cs true to enable the feature, otherwise disable (default).
 */
static inline void spi_ll_master_set_pos_cs(spi_dev_t *hw, int cs, uint32_t pos_cs)
{
    if (pos_cs) {
        hw->pin.master_cs_pol |= (1 << cs);
    } else {
        hw->pin.master_cs_pol &= ~(1 << cs);
    }
}

/**
 * Enable/disable the LSBFIRST feature for TX data.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param lsbfirst true if LSB of TX data to be sent first, otherwise MSB is sent first (default).
 */
static inline void spi_ll_set_tx_lsbfirst(spi_dev_t *hw, bool lsbfirst)
{
    hw->ctrl.wr_bit_order = lsbfirst;
}

/**
 * Enable/disable the LSBFIRST feature for RX data.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param lsbfirst true if first bit received as LSB, otherwise as MSB (default).
 */
static inline void spi_ll_set_rx_lsbfirst(spi_dev_t *hw, bool lsbfirst)
{
    hw->ctrl.rd_bit_order = lsbfirst;
}

/**
 * Set SPI mode for the peripheral as master.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param mode SPI mode to work at, 0-3.
 */
static inline void spi_ll_master_set_mode(spi_dev_t *hw, uint8_t mode)
{
    //Configure polarity
    if (mode == 0) {
        hw->pin.ck_idle_edge = 0;
        hw->user.ck_out_edge = 0;
    } else if (mode == 1) {
        hw->pin.ck_idle_edge = 0;
        hw->user.ck_out_edge = 1;
    } else if (mode == 2) {
        hw->pin.ck_idle_edge = 1;
        hw->user.ck_out_edge = 1;
    } else if (mode == 3) {
        hw->pin.ck_idle_edge = 1;
        hw->user.ck_out_edge = 0;
    }
}

/**
 * Set SPI mode for the peripheral as slave.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param mode SPI mode to work at, 0-3.
 */
static inline void spi_ll_slave_set_mode(spi_dev_t *hw, const int mode, bool dma_used)
{
    if (mode == 0) {
        //The timing needs to be fixed to meet the requirements of DMA
        hw->pin.ck_idle_edge = 1;
        hw->user.ck_i_edge = 0;
        hw->ctrl2.miso_delay_mode = 0;
        hw->ctrl2.miso_delay_num = 0;
        hw->ctrl2.mosi_delay_mode = 2;
        hw->ctrl2.mosi_delay_num = 2;
    } else if (mode == 1) {
        hw->pin.ck_idle_edge = 1;
        hw->user.ck_i_edge = 1;
        hw->ctrl2.miso_delay_mode = 2;
        hw->ctrl2.miso_delay_num = 0;
        hw->ctrl2.mosi_delay_mode = 0;
        hw->ctrl2.mosi_delay_num = 0;
    } else if (mode == 2) {
        //The timing needs to be fixed to meet the requirements of DMA
        hw->pin.ck_idle_edge = 0;
        hw->user.ck_i_edge = 1;
        hw->ctrl2.miso_delay_mode = 0;
        hw->ctrl2.miso_delay_num = 0;
        hw->ctrl2.mosi_delay_mode = 1;
        hw->ctrl2.mosi_delay_num = 2;
    } else if (mode == 3) {
        hw->pin.ck_idle_edge = 0;
        hw->user.ck_i_edge = 0;
        hw->ctrl2.miso_delay_mode = 1;
        hw->ctrl2.miso_delay_num = 0;
        hw->ctrl2.mosi_delay_mode = 0;
        hw->ctrl2.mosi_delay_num = 0;
    }

    /* Silicon issues exists in mode 0 and 2 with DMA, change clock phase to
     * avoid dma issue. This will cause slave output to appear at most half a
     * spi clock before
     */
    if (dma_used) {
        if (mode == 0) {
            hw->pin.ck_idle_edge = 0;
            hw->user.ck_i_edge = 1;
            hw->ctrl2.miso_delay_mode = 0;
            hw->ctrl2.miso_delay_num = 2;
            hw->ctrl2.mosi_delay_mode = 0;
            hw->ctrl2.mosi_delay_num = 3;
        } else if (mode == 2) {
            hw->pin.ck_idle_edge = 1;
            hw->user.ck_i_edge = 0;
            hw->ctrl2.miso_delay_mode = 0;
            hw->ctrl2.miso_delay_num = 2;
            hw->ctrl2.mosi_delay_mode = 0;
            hw->ctrl2.mosi_delay_num = 3;
        }
    }
}

/**
 * Set SPI to work in full duplex or half duplex mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param half_duplex true to work in half duplex mode, otherwise in full duplex mode.
 */
static inline void spi_ll_set_half_duplex(spi_dev_t *hw, bool half_duplex)
{
    hw->user.doutdin = !half_duplex;
}

/**
 * Set SPI to work in SIO mode or not.
 *
 * SIO is a mode which MOSI and MISO share a line. The device MUST work in half-duplexmode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param sio_mode true to work in SIO mode, otherwise false.
 */
static inline void spi_ll_set_sio_mode(spi_dev_t *hw, int sio_mode)
{
    hw->user.sio = sio_mode;
}

/**
 * Configure the SPI transaction line mode for the master to use.
 *
 * @param hw        Beginning address of the peripheral registers.
 * @param line_mode SPI transaction line mode to use, see ``spi_line_mode_t``.
 */
static inline void spi_ll_master_set_line_mode(spi_dev_t *hw, spi_line_mode_t line_mode)
{
    hw->ctrl.val &= ~SPI_LL_ONE_LINE_CTRL_MASK;
    hw->user.val &= ~SPI_LL_ONE_LINE_USER_MASK;
    if (line_mode.cmd_lines > 1) {
        HAL_ASSERT(false);
    }
    switch (line_mode.data_lines) {
    case 2:
        if (line_mode.addr_lines == 1) {
            // 1-line-cmd + 1-line-addr + 2-line-data
            hw->ctrl.fread_dual = 1;
            hw->user.fwrite_dual = 1;
        } else if (line_mode.addr_lines == 2) {
            // 1-line-cmd + 2-line-addr + 2-line-data
            hw->ctrl.fread_dio = 1;
            hw->user.fwrite_dio = 1;
        } else {
            HAL_ASSERT(false);
        }
        hw->ctrl.fastrd_mode = 1;
        break;
    case 4:
        if (line_mode.addr_lines == 1) {
            // 1-line-cmd + 1-line-addr + 4-line-data
            hw->ctrl.fread_quad = 1;
            hw->user.fwrite_quad = 1;
        } else if (line_mode.addr_lines == 4) {
            // 1-line-cmd + 4-line-addr + 4-line-data
            hw->ctrl.fread_qio = 1;
            hw->user.fwrite_qio = 1;
        } else {
            HAL_ASSERT(false);
        }
        hw->ctrl.fastrd_mode = 1;
        break;
    default:
        // 1-line-cmd + 1-line-addr + 1-line-data
        break;
    }
}

/**
 * Select one of the CS to use in current transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param cs_id The cs to use, 0-2, otherwise none of them is used.
 */
static inline void spi_ll_master_select_cs(spi_dev_t *hw, int cs_id)
{
    hw->pin.cs0_dis = (cs_id == 0) ? 0 : 1;
    hw->pin.cs1_dis = (cs_id == 1) ? 0 : 1;
    hw->pin.cs2_dis = (cs_id == 2) ? 0 : 1;
}

/**
 * Keep Chip Select activated after the current transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param keep_active if 0 don't keep CS activated, else keep CS activated
 */
static inline void spi_ll_master_keep_cs(spi_dev_t *hw, int keep_active)
{
    hw->pin.cs_keep_active = (keep_active != 0) ? 1 : 0;
}

/*------------------------------------------------------------------------------
 * Configs: parameters
 *----------------------------------------------------------------------------*/
/**
 * Set the standard clock mode for master.
 *
 * @param hw  Beginning address of the peripheral registers.
 * @param enable_std True for std timing, False for half cycle delay sampling.
 */
static inline void spi_ll_master_set_rx_timing_mode(spi_dev_t *hw, spi_sampling_point_t sample_point)
{
    //This is not supported
}

/**
 * Get if standard clock mode is supported.
 */
static inline bool spi_ll_master_is_rx_std_sample_supported(void)
{
    return false;
}

/**
 * Set the clock for master by stored value.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param val stored clock configuration calculated before (by ``spi_ll_cal_clock``).
 */
static inline void spi_ll_master_set_clock_by_reg(spi_dev_t *hw, const spi_ll_clock_val_t *val)
{
    hw->clock.val = *(uint32_t *)val;
}

/**
 * Get the frequency of given dividers. Don't use in app.
 *
 * @param fapb APB clock of the system.
 * @param pre Pre divider.
 * @param n main divider.
 *
 * @return Frequency of given dividers.
 */
static inline int spi_ll_freq_for_pre_n(int fapb, int pre, int n)
{
    return (fapb / (pre * n));
}

/**
 * Calculate the nearest frequency available for master.
 *
 * @param fapb APB clock of the system.
 * @param hz Frequency desired.
 * @param duty_cycle Duty cycle desired.
 * @param out_reg Output address to store the calculated clock configurations for the return frequency.
 *
 * @return Actual (nearest) frequency.
 */
static inline int spi_ll_master_cal_clock(int fapb, int hz, int duty_cycle, spi_ll_clock_val_t *out_reg)
{
    typeof(SPI1.clock) reg = {.val = 0};
    int eff_clk;

    //In hw, n, h and l are 1-64, pre is 1-8K. Value written to register is one lower than used value.
    if (hz > ((fapb / 4) * 3)) {
        //Using Fapb directly will give us the best result here.
        reg.clkcnt_l = 0;
        reg.clkcnt_h = 0;
        reg.clkcnt_n = 0;
        reg.clkdiv_pre = 0;
        reg.clk_equ_sysclk = 1;
        eff_clk = fapb;
    } else {
        //For best duty cycle resolution, we want n to be as close to 32 as possible, but
        //we also need a pre/n combo that gets us as close as possible to the intended freq.
        //To do this, we bruteforce n and calculate the best pre to go along with that.
        //If there's a choice between pre/n combos that give the same result, use the one
        //with the higher n.
        int pre, n, h, l;
        int bestn = -1;
        int bestpre = -1;
        int besterr = 0;
        int errval;
        for (n = 2; n <= 64; n++) { //Start at 2: we need to be able to set h/l so we have at least one high and one low pulse.
            //Effectively, this does pre=round((fapb/n)/hz).
            pre = ((fapb / n) + (hz / 2)) / hz;
            if (pre <= 0) {
                pre = 1;
            }
            if (pre > 8192) {
                pre = 8192;
            }
            errval = abs(spi_ll_freq_for_pre_n(fapb, pre, n) - hz);
            if (bestn == -1 || errval <= besterr) {
                besterr = errval;
                bestn = n;
                bestpre = pre;
            }
        }

        n = bestn;
        pre = bestpre;
        l = n;
        //This effectively does round((duty_cycle*n)/256)
        h = (duty_cycle * n + 127) / 256;
        if (h <= 0) {
            h = 1;
        }

        reg.clk_equ_sysclk = 0;
        reg.clkcnt_n = n - 1;
        reg.clkdiv_pre = pre - 1;
        reg.clkcnt_h = h - 1;
        reg.clkcnt_l = l - 1;
        eff_clk = spi_ll_freq_for_pre_n(fapb, pre, n);
    }
    if (out_reg != NULL) {
        *(uint32_t *)out_reg = reg.val;
    }
    return eff_clk;
}

/**
 * Calculate and set clock for SPI master according to desired parameters.
 *
 * This takes long, suggest to calculate the configuration during
 * initialization by ``spi_ll_master_cal_clock`` and store the result, then
 * configure the clock by stored value when used by
 * ``spi_ll_msater_set_clock_by_reg``.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param fapb APB clock of the system.
 * @param hz Frequency desired.
 * @param duty_cycle Duty cycle desired.
 *
 * @return Actual frequency that is used.
 */
static inline int spi_ll_master_set_clock(spi_dev_t *hw, int fapb, int hz, int duty_cycle)
{
    spi_ll_clock_val_t reg_val;
    int freq = spi_ll_master_cal_clock(fapb, hz, duty_cycle, &reg_val);
    spi_ll_master_set_clock_by_reg(hw, &reg_val);
    return freq;
}

/**
 * Enable/disable the CK sel feature for a CS pin.
 *
 * CK sel is a feature to toggle the CS line along with the clock.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param cs CS pin to enable/disable the feature, 0-2.
 * @param cksel true to enable the feature, otherwise false.
 */
static inline void spi_ll_master_set_cksel(spi_dev_t *hw, int cs, uint32_t cksel)
{
    if (cksel) {
        hw->pin.master_ck_sel |= (1 << cs);
    } else {
        hw->pin.master_ck_sel &= ~(1 << cs);
    }
}

/**
 * Set the mosi delay after the output edge to the signal. (Preview)
 *
 * The delay mode/num is a Espressif conception, may change in the new chips.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param delay_mode Delay mode, see TRM.
 * @param delay_num APB clocks to delay.
 */
static inline void spi_ll_set_mosi_delay(spi_dev_t *hw, int delay_mode, int delay_num)
{
    hw->ctrl2.mosi_delay_mode = delay_mode;
    hw->ctrl2.mosi_delay_num = delay_num;
}

/**
 * Set the miso delay applied to the input signal before the internal peripheral. (Preview)
 *
 * The delay mode/num is a Espressif conception, may change in the new chips.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param delay_mode Delay mode, see TRM.
 * @param delay_num APB clocks to delay.
 */
static inline void spi_ll_set_miso_delay(spi_dev_t *hw, int delay_mode, int delay_num)
{
    hw->ctrl2.miso_delay_mode = delay_mode;
    hw->ctrl2.miso_delay_num = delay_num;
}

/**
 * Set dummy clocks to output before RX phase (master), or clocks to skip
 * before the data phase and after the address phase (slave).
 *
 * Note this phase is also used to compensate RX timing in half duplex mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param dummy_n Dummy cycles used. 0 to disable the dummy phase.
 */
static inline void spi_ll_set_dummy(spi_dev_t *hw, int dummy_n)
{
    hw->user.usr_dummy = dummy_n ? 1 : 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->user1, usr_dummy_cyclelen, dummy_n - 1);
}

/**
 * Set the delay of SPI clocks before the CS inactive edge after the last SPI clock.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param hold Delay of SPI clocks after the last clock, 0 to disable the hold phase.
 */
static inline void spi_ll_master_set_cs_hold(spi_dev_t *hw, int hold)
{
    hw->ctrl2.hold_time = hold;
    hw->user.cs_hold = hold ? 1 : 0;
}

/**
 * Set the delay of SPI clocks before the first SPI clock after the CS active edge.
 *
 * Note ESP32 doesn't support to use this feature when command/address phases
 * are used in full duplex mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param setup Delay of SPI clocks after the CS active edge, 0 to disable the setup phase.
 */
static inline void spi_ll_master_set_cs_setup(spi_dev_t *hw, uint8_t setup)
{
    hw->ctrl2.setup_time = setup - 1;
    hw->user.cs_setup = setup ? 1 : 0;
}

/*------------------------------------------------------------------------------
 * Configs: data
 *----------------------------------------------------------------------------*/
/**
 * Set the input length (master).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen input length, in bits.
 */
static inline void spi_ll_set_miso_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->miso_dlen.usr_miso_dbitlen = bitlen - 1;
}

/**
 * Set the output length (master).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen output length, in bits.
 */
static inline void spi_ll_set_mosi_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->mosi_dlen.usr_mosi_dbitlen = bitlen - 1;
}

/**
 * Set the maximum input length (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen input length, in bits.
 */
static inline void spi_ll_slave_set_rx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->slv_rdbuf_dlen.bit_len = bitlen - 1;
}

/**
 * Set the maximum output length (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen output length, in bits.
 */
static inline void spi_ll_slave_set_tx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->slv_wrbuf_dlen.bit_len = bitlen - 1;
}

/**
 * Set the length of command phase.
 *
 * When in 4-bit mode, the SPI cycles of the phase will be shorter. E.g. 16-bit
 * command phases takes 4 cycles in 4-bit mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen Length of command phase, in bits. 0 to disable the command phase.
 */
static inline void spi_ll_set_command_bitlen(spi_dev_t *hw, int bitlen)
{
    hw->user2.usr_command_bitlen = bitlen - 1;
    hw->user.usr_command = bitlen ? 1 : 0;
}

/**
 * Set the length of address phase.
 *
 * When in 4-bit mode, the SPI cycles of the phase will be shorter. E.g. 16-bit
 * address phases takes 4 cycles in 4-bit mode.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen Length of address phase, in bits. 0 to disable the address phase.
 */
static inline void spi_ll_set_addr_bitlen(spi_dev_t *hw, int bitlen)
{
    hw->user1.usr_addr_bitlen = bitlen - 1;
    hw->user.usr_addr = bitlen ? 1 : 0;
}

/**
 * Set the address value in an intuitive way.
 *
 * The length and lsbfirst is required to shift and swap the address to the right place.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param address Address to set
 * @param addrlen Length of the address phase
 * @param lsbfirst whether the LSB first feature is enabled.
 */
static inline void spi_ll_set_address(spi_dev_t *hw, uint64_t addr, int addrlen, uint32_t lsbfirst)
{
    if (lsbfirst) {
        /* The output address start from the LSB of the highest byte, i.e.
         * addr[24] -> addr[31]
         * ...
         * addr[0] -> addr[7]
         * slv_wr_status[24] -> slv_wr_status[31]
         * ...
         * slv_wr_status[0] -> slv_wr_status[7]
         * So swap the byte order to let the LSB sent first.
         */
        addr = HAL_SWAP64(addr);
        hw->addr = addr >> 32;
        hw->slv_wr_status = addr;
    } else {
        // shift the address to MSB of addr (and maybe slv_wr_status) register.
        // output address will be sent from MSB to LSB of addr register, then comes the MSB to LSB of slv_wr_status register.
        if (addrlen > 32) {
            hw->addr = addr >> (addrlen - 32);
            hw->slv_wr_status = addr << (64 - addrlen);
        } else {
            hw->addr = addr << (32 - addrlen);
        }
    }
}

/**
 * Set the command value in an intuitive way.
 *
 * The length and lsbfirst is required to shift and swap the command to the right place.
 *
 * @param hw Beginning command of the peripheral registers.
 * @param command Command to set
 * @param addrlen Length of the command phase
 * @param lsbfirst whether the LSB first feature is enabled.
 */
static inline void spi_ll_set_command(spi_dev_t *hw, uint16_t cmd, int cmdlen, bool lsbfirst)
{
    if (lsbfirst) {
        // The output command start from bit0 to bit 15, kept as is.
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->user2, usr_command_value, cmd);
    } else {
        /* Output command will be sent from bit 7 to 0 of command_value, and
         * then bit 15 to 8 of the same register field. Shift and swap to send
         * more straightly.
         */
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->user2, usr_command_value, HAL_SPI_SWAP_DATA_TX(cmd, cmdlen));

    }
}

/**
 * Enable/disable the RX data phase.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable true if RX phase exist, otherwise false.
 */
static inline void spi_ll_enable_miso(spi_dev_t *hw, int enable)
{
    hw->user.usr_miso = enable;
}

/**
 * Enable/disable the TX data phase.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param enable true if TX phase exist, otherwise false.
 */
static inline void spi_ll_enable_mosi(spi_dev_t *hw, int enable)
{
    hw->user.usr_mosi = enable;
}

/**
 * Reset the slave peripheral before next transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_reset(spi_dev_t *hw)
{
    hw->slave.sync_reset = 1;
    hw->slave.sync_reset = 0;
}

/**
 * Get the received bit length of the slave.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return Received bits of the slave.
 */
static inline uint32_t spi_ll_slave_get_rcv_bitlen(spi_dev_t *hw)
{
    return hw->slv_rd_bit.slv_rdata_bit;
}

/*------------------------------------------------------------------------------
 * Interrupts
 *----------------------------------------------------------------------------*/
/**
 * Disable the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_disable_int(spi_dev_t *hw)
{
    hw->slave.trans_inten = 0;
}

/**
 * Clear the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_clear_int_stat(spi_dev_t *hw)
{
    hw->slave.trans_done = 0;
}

/**
 * Set the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_set_int_stat(spi_dev_t *hw)
{
    hw->slave.trans_done = 1;
}

/**
 * Enable the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_enable_int(spi_dev_t *hw)
{
    hw->slave.trans_inten = 1;
}

/*------------------------------------------------------------------------------
 * DMA:
 *      RX DMA (Peripherals->DMA->RAM)
 *      TX DMA (RAM->DMA->Peripherals)
 *----------------------------------------------------------------------------*/
/**
 * Enable peripheral register clock
 *
 * @param host_id   Peripheral index number, see `spi_host_device_t`
 * @param enable    Enable/Disable
 */
static inline void spi_dma_ll_enable_bus_clock(spi_host_device_t host_id, bool enable) {
    (void)host_id; // has only one spi_dma
    if (enable) {
        DPORT_SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_DMA_CLK_EN);
    } else {
        DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_SPI_DMA_CLK_EN);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define spi_dma_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; spi_dma_ll_enable_bus_clock(__VA_ARGS__)

/**
 * Reset whole peripheral register to init value defined by HW design
 *
 * @param host_id   Peripheral index number, see `spi_host_device_t`
 */
__attribute__((always_inline))
static inline void spi_dma_ll_reset_register(spi_host_device_t host_id) {
    (void)host_id; // has only one spi_dma
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_SPI_DMA_RST);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define spi_dma_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; spi_dma_ll_reset_register(__VA_ARGS__)

/**
 * Reset RX DMA which stores the data received from a peripheral into RAM.
 *
 * @param dma_in  Beginning address of the DMA peripheral registers which stores the data received from a peripheral into RAM.
 * @param channel DMA channel, for chip version compatibility, not used.
 */
__attribute__((always_inline))
static inline void spi_dma_ll_rx_reset(spi_dma_dev_t *dma_in, uint32_t channel)
{
    //Reset RX DMA peripheral
    dma_in->dma_conf.in_rst = 1;
    dma_in->dma_conf.in_rst = 0;
}

/**
 * Start RX DMA.
 *
 * @param dma_in  Beginning address of the DMA peripheral registers which stores the data received from a peripheral into RAM.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param addr    Address of the beginning DMA descriptor.
 */
__attribute__((always_inline))
static inline void spi_dma_ll_rx_start(spi_dma_dev_t *dma_in, uint32_t channel, lldesc_t *addr)
{
    dma_in->dma_in_link.addr = (int) addr & 0xFFFFF;
    dma_in->dma_in_link.start = 1;
}

/**
 * Enable DMA RX channel burst for data
 *
 * @param dma_in  Beginning address of the DMA peripheral registers which stores the data received from a peripheral into RAM.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  True to enable, false to disable
 */
static inline void spi_dma_ll_rx_enable_burst_data(spi_dma_dev_t *dma_in, uint32_t channel, bool enable)
{
    //This is not supported in esp32
}

/**
 * Enable DMA RX channel burst for descriptor
 *
 * @param dma_in  Beginning address of the DMA peripheral registers which stores the data received from a peripheral into RAM.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  True to enable, false to disable
 */
static inline void spi_dma_ll_rx_enable_burst_desc(spi_dma_dev_t *dma_in, uint32_t channel, bool enable)
{
    dma_in->dma_conf.indscr_burst_en = enable;
}

/**
 * Reset TX DMA which transmits the data from RAM to a peripheral.
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 */
__attribute__((always_inline))
static inline void spi_dma_ll_tx_reset(spi_dma_dev_t *dma_out, uint32_t channel)
{
    //Reset TX DMA peripheral
    dma_out->dma_conf.out_rst = 1;
    dma_out->dma_conf.out_rst = 0;
}

/**
 * Start TX DMA.
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param addr    Address of the beginning DMA descriptor.
 */
__attribute__((always_inline))
static inline void spi_dma_ll_tx_start(spi_dma_dev_t *dma_out, uint32_t channel, lldesc_t *addr)
{
    dma_out->dma_out_link.addr = (int) addr & 0xFFFFF;
    dma_out->dma_out_link.start = 1;
}

/**
 * Enable DMA TX channel burst for data
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  True to enable, false to disable
 */
static inline void spi_dma_ll_tx_enable_burst_data(spi_dma_dev_t *dma_out, uint32_t channel, bool enable)
{
    dma_out->dma_conf.out_data_burst_en = enable;
}

/**
 * Enable DMA TX channel burst for descriptor
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  True to enable, false to disable
 */
static inline void spi_dma_ll_tx_enable_burst_desc(spi_dma_dev_t *dma_out, uint32_t channel, bool enable)
{
    dma_out->dma_conf.outdscr_burst_en = enable;
}

/**
 * Configuration of OUT EOF flag generation way
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  1: when dma pop all data from fifo  0:when ahb push all data to fifo.
 */
static inline void spi_dma_ll_set_out_eof_generation(spi_dma_dev_t *dma_out, uint32_t channel, bool enable)
{
    dma_out->dma_conf.out_eof_mode = enable;
}

/**
 * Enable automatic outlink-writeback
 *
 * @param dma_out Beginning address of the DMA peripheral registers which transmits the data from RAM to a peripheral.
 * @param channel DMA channel, for chip version compatibility, not used.
 * @param enable  True to enable, false to disable
 */
static inline void spi_dma_ll_enable_out_auto_wrback(spi_dma_dev_t *dma_out, uint32_t channel, bool enable)
{
    //does not configure it in ESP32
}

/**
 * Get the spi communication command
 *
 * @param cmd_t           Base command value
 * @param line_mode       Line mode of SPI transaction phases: CMD, ADDR, DOUT/DIN.
 */
static inline uint16_t spi_ll_get_slave_hd_command(spi_command_t cmd_t, spi_line_mode_t line_mode)
{
    //This is not supported in esp32
    return 0;
}

/**
 * Get the dummy bits
 *
 * @param line_mode       Line mode of SPI transaction phases: CMD, ADDR, DOUT/DIN.
 */
static inline int spi_ll_get_slave_hd_dummy_bits(spi_line_mode_t line_mode)
{
    //This is not supported in esp32
    return 0;
}

#undef SPI_LL_RST_MASK
#undef SPI_LL_UNUSED_INT_MASK

#ifdef __cplusplus
}
#endif
