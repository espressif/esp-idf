// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The LL layer for ESP32s2beta SPI register operations

#pragma once

#include "hal/hal_defs.h"
#include "soc/spi_periph.h"
#include "esp32/rom/lldesc.h"
#include <string.h>
#include <esp_types.h>
#include <stdlib.h> //for abs()

/// Registers to reset during initialization. Don't use in app.
#define SPI_LL_RST_MASK (SPI_OUT_RST | SPI_IN_RST | SPI_AHBM_RST | SPI_AHBM_FIFO_RST)
/// Interrupt not used. Don't use in app.
#define SPI_LL_UNUSED_INT_MASK  (SPI_INT_TRANS_DONE_EN | SPI_INT_WR_DMA_DONE_EN | SPI_INT_RD_DMA_DONE_EN | SPI_INT_WR_BUF_DONE_EN | SPI_INT_RD_BUF_DONE_EN)
/// Swap the bit order to its correct place to send
#define HAL_SPI_SWAP_DATA_TX(data, len) HAL_SWAP32((uint32_t)data<<(32-len))

#define SPI_LL_GET_HW(ID) ((ID)==0? ({abort();NULL;}):((ID)==1? &GPSPI2 : ((ID)==2? &GPSPI3: &GPSPI4)))

/**
 * The data structure holding calculated clock configuration. Since the
 * calculation needs long time, it should be calculated during initialization and
 * stored somewhere to be quickly used.
 */
typedef uint32_t spi_ll_clock_val_t;

/** IO modes supported by the master. */
typedef enum {
    SPI_LL_IO_MODE_NORMAL = 0,  ///< 1-bit mode for all phases
    SPI_LL_IO_MODE_DIO,         ///< 2-bit mode for address and data phases, 1-bit mode for command phase
    SPI_LL_IO_MODE_DUAL,        ///< 2-bit mode for data phases only, 1-bit mode for command and address phases
    SPI_LL_IO_MODE_QIO,         ///< 4-bit mode for address and data phases, 1-bit mode for command phase
    SPI_LL_IO_MODE_QUAD,        ///< 4-bit mode for data phases only, 1-bit mode for command and address phases
} spi_ll_io_mode_t;

/// Interrupt type for different working pattern
typedef enum {
    SPI_LL_INT_TYPE_NORMAL = 0, ///< Typical pattern, only wait for trans done
    SPI_LL_INT_TYPE_SEG = 1,    ///< Wait for DMA signals
} spi_ll_slave_intr_type;


/*------------------------------------------------------------------------------
 * Control
 *----------------------------------------------------------------------------*/
/**
 * Initialize SPI peripheral (master).
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_master_init(spi_dev_t *hw)
{
    //Reset DMA
    hw->dma_conf.val |= SPI_LL_RST_MASK;
    hw->dma_out_link.start = 0;
    hw->dma_in_link.start = 0;
    hw->dma_conf.val &= ~SPI_LL_RST_MASK;
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
    //it's stupid, but if something goes wrong, try to uncomment it
    //hw->slave.slave_mode = 1;
    //Configure slave
    hw->clock.val = 0;
    hw->user.val = 0;
    hw->ctrl.val = 0;
    hw->user.doutdin = 1; //we only support full duplex
    hw->user.sio = 0;
    hw->user.tx_start_bit = 7;
    hw->slave.slave_mode = 1;
    hw->dma_conf.val |= SPI_LL_RST_MASK;
    hw->dma_out_link.start = 0;
    hw->dma_in_link.start = 0;
    hw->dma_conf.val &= ~SPI_LL_RST_MASK;
    hw->slave.sync_reset = 1;
    hw->slave.sync_reset = 0;
    //use all 64 bytes of the buffer
    hw->user.usr_miso_highpart = 0;
    hw->user.usr_mosi_highpart = 0;
    //by default seg mode is disabled
    hw->dma_conf.dma_continue = 0;

    //Disable unneeded ints
    hw->slave.val &= ~SPI_LL_UNUSED_INT_MASK;
    hw->dma_int_ena.val = 0;
}

/**
 * Reset TX and RX DMAs.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_reset_dma(spi_dev_t *hw)
{
    //Reset DMA peripheral
    hw->dma_conf.val |= SPI_LL_RST_MASK;
    hw->dma_out_link.start = 0;
    hw->dma_in_link.start = 0;
    hw->dma_conf.val &= ~SPI_LL_RST_MASK;
    hw->dma_conf.out_data_burst_en = 1;
    hw->dma_conf.indscr_burst_en = 1;
    hw->dma_conf.outdscr_burst_en = 1;
    hw->dma_in_link.dma_rx_ena = 0;
    assert(hw->dma_in_link.dma_rx_ena==0);
}

/**
 * Start RX DMA.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param addr Address of the beginning DMA descriptor.
 */
static inline void spi_ll_rxdma_start(spi_dev_t *hw, lldesc_t *addr)
{
    //if something breaks, uncomment this line
    //hw->dma_in_link.restart = 1;
    hw->dma_in_link.addr = (int) addr & 0xFFFFF;
    hw->dma_in_link.start = 1;
}

/**
 * Start TX DMA.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param addr Address of the beginning DMA descriptor.
 */
static inline void spi_ll_txdma_start(spi_dev_t *hw, lldesc_t *addr)
{
    //if something breaks, uncomment this line
    hw->dma_out_link.restart = 1;
    hw->dma_out_link.addr = (int) addr & 0xFFFFF;
    hw->dma_out_link.start = 1;
}

/**
 * Write to SPI buffer.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param buffer_to_send Data address to copy to the buffer.
 * @param bitlen Length to copy, in bits.
 */
static inline void spi_ll_write_buffer(spi_dev_t *hw, const uint8_t *buffer_to_send, size_t bitlen)
{
    for (int x = 0; x < bitlen; x += 32) {
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
    for (int x = 0; x < bitlen; x += 32) {
        //Do a memcpy to get around possible alignment issues in rx_buffer
        uint32_t word = hw->data_buf[x / 32];
        int len = bitlen - x;
        if (len > 32) {
            len = 32;
        }
        memcpy(&buffer_to_rcv[x / 8], &word, (len + 7) / 8);
    }
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
 * Disable the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_disable_int(spi_dev_t *hw)
{
    hw->slave.int_trans_done_en = 0;
}

/**
 * Clear the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_clear_int_stat(spi_dev_t *hw)
{
    hw->slave.trans_done = 0;
    hw->dma_int_clr.val = UINT32_MAX;
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
    hw->slave.int_trans_done_en = 1;
}

/**
 * Set different interrupt types for the slave.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param int_type Interrupt type
 */
static inline void spi_ll_slave_set_int_type(spi_dev_t *hw, spi_ll_slave_intr_type int_type)
{
    switch (int_type)
    {
    case SPI_LL_INT_TYPE_SEG:
        hw->dma_int_ena.in_suc_eof = 1;
        hw->dma_int_ena.out_total_eof = 1;
        hw->slave.int_trans_done_en = 0;
        break;
    default:
        hw->dma_int_ena.in_suc_eof = 0;
        hw->dma_int_ena.out_total_eof = 0;
        hw->slave.int_trans_done_en = 1;
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
        hw->misc.master_cs_pol |= (1 << cs);
    } else {
        hw->misc.master_cs_pol &= (1 << cs);
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
        hw->misc.ck_idle_edge = 0;
        hw->user.ck_out_edge = 0;
    } else if (mode == 1) {
        hw->misc.ck_idle_edge = 0;
        hw->user.ck_out_edge = 1;
    } else if (mode == 2) {
        hw->misc.ck_idle_edge = 1;
        hw->user.ck_out_edge = 1;
    } else if (mode == 3) {
        hw->misc.ck_idle_edge = 1;
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
        hw->misc.ck_idle_edge = 0;
        hw->user.rsck_i_edge = 0;
        hw->user.tsck_i_edge = 0;
        hw->ctrl1.rsck_data_out = 0;
        hw->ctrl1.clk_mode_13 = 0;
    } else if (mode == 1) {
        hw->misc.ck_idle_edge = 0;
        hw->user.rsck_i_edge = 1;
        hw->user.tsck_i_edge = 1;
        hw->ctrl1.rsck_data_out = 0;
        hw->ctrl1.clk_mode_13 = 1;
    } else if (mode == 2) {
        hw->misc.ck_idle_edge = 1;
        hw->user.rsck_i_edge = 1;
        hw->user.tsck_i_edge = 1;
        hw->ctrl1.rsck_data_out = 0;
        hw->ctrl1.clk_mode_13 = 0;
    } else if (mode == 3) {
        hw->misc.ck_idle_edge = 1;
        hw->user.rsck_i_edge = 0;
        hw->user.tsck_i_edge = 0;
        hw->ctrl1.rsck_data_out = 0;
        hw->ctrl1.clk_mode_13 = 1;
    }
    //hw->ctrl1.rsck_data_out = 1;
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
 * Configure the io mode for the master to work at.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param io_mode IO mode to work at, see ``spi_ll_io_mode_t``.
 */
static inline void spi_ll_master_set_io_mode(spi_dev_t *hw, spi_ll_io_mode_t io_mode)
{
    hw->ctrl.val &= ~(SPI_FREAD_DUAL | SPI_FREAD_QUAD | SPI_FREAD_DIO | SPI_FREAD_QIO);
    hw->user.val &= ~(SPI_FWRITE_DUAL | SPI_FWRITE_QUAD | SPI_FWRITE_DIO | SPI_FWRITE_QIO);
    switch (io_mode) {
    case SPI_LL_IO_MODE_DIO:
        // hw->ctrl.fread_dio = 1;
        // hw->user.fwrite_dio = 1;
        break;
    case SPI_LL_IO_MODE_DUAL:
        hw->ctrl.fread_dual = 1;
        hw->user.fwrite_dual = 1;
        break;
    case SPI_LL_IO_MODE_QIO:
        // hw->ctrl.fread_qio = 1;
        // hw->user.fwrite_qio = 1;
        break;
    case SPI_LL_IO_MODE_QUAD:
        hw->ctrl.fread_quad = 1;
        hw->user.fwrite_quad = 1;
        break;
    default:
        break;
    };
    // if (io_mode != SPI_LL_IO_MODE_NORMAL) {
    //     hw->ctrl.fastrd_mode = 1;
    // }
}

/**
 * Select one of the CS to use in current transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param cs_id The cs to use, 0-2, otherwise none of them is used.
 */
static inline void spi_ll_master_select_cs(spi_dev_t *hw, int cs_id)
{
    hw->misc.cs0_dis = (cs_id == 0) ? 0 : 1;
    hw->misc.cs1_dis = (cs_id == 1) ? 0 : 1;
    hw->misc.cs2_dis = (cs_id == 2) ? 0 : 1;
}

/*------------------------------------------------------------------------------
 * Configs: parameters
 *----------------------------------------------------------------------------*/
/**
 * Set the clock for master by stored value.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param val stored clock configuration calculated before (by ``spi_ll_cal_clock``).
 */
static inline void spi_ll_master_set_clock_by_reg(spi_dev_t *hw, spi_ll_clock_val_t *val)
{
    hw->clock.val = *(uint32_t *)val;
}

/**
 * Get the frequency of given dividers. Don't use in app.
 *
 * @param fapb APB clock of the system.
 * @param pre Pre devider.
 * @param n main divider.
 *
 * @return Frequency of given dividers.
 */
static inline int spi_ll_freq_for_pre_n(int fapb, int pre, int n)
{
    return (fapb / (pre * n));
}

/**
 * Calculate the nearest frequency avaliable for master.
 *
 * @param fapb APB clock of the system.
 * @param hz Frequncy desired.
 * @param duty_cycle Duty cycle desired.
 * @param out_reg Output address to store the calculated clock configurations for the return frequency.
 *
 * @return Actual (nearest) frequency.
 */
static inline int spi_ll_master_cal_clock(int fapb, int hz, int duty_cycle, spi_ll_clock_val_t *out_reg)
{
    typeof(GPSPI2.clock) reg;
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
 * @param hz Frequncy desired.
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
    //TODO: this doesn't make sense
    hw->dout_num.dout0_num = 0;
    hw->dout_num.dout1_num = 0;
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
    //TODO: this doesn't make sense
    hw->din_num.din0_num = 1;
    hw->din_num.din1_num = 1;
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
    hw->user1.usr_dummy_cyclelen = dummy_n - 1;
}

/**
 * Set the delay of SPI clocks before the CS inactive edge after the last SPI clock.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param hold Delay of SPI clocks after the last clock, 0 to disable the hold phase.
 */
static inline void spi_ll_master_set_cs_hold(spi_dev_t *hw, int hold)
{
    hw->ctrl2.cs_hold_time = hold - 1;
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
    hw->ctrl2.cs_setup_time = setup - 1;
    hw->user.cs_setup = setup ? 1 : 0;
}

/**
 * Enable/disable the segment transfer feature for the slave.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param en true to enable, false to disable.
 */
static inline void spi_ll_slave_set_seg_en(spi_dev_t *hw, bool en)
{
    hw->dma_conf.slv_rx_seg_trans_en = en;
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
    hw->miso_dlen.usr_miso_bit_len = bitlen - 1;
}

/**
 * Set the output length (master).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen output length, in bits.
 */
static inline void spi_ll_set_mosi_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->mosi_dlen.usr_mosi_bit_len = bitlen - 1;
}

/**
 * Set the maximum input length (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen input length, in bits.
 */
static inline void spi_ll_slave_set_rx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->slv_wrbuf_dlen.bit_len = bitlen - 1;
}

/**
 * Set the maximum output length (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 * @param bitlen output length, in bits.
 */
static inline void spi_ll_slave_set_tx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    hw->slv_rdbuf_dlen.bit_len = bitlen - 1;
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
        hw->user2.usr_command_value = cmd;
    } else {
        /* Output command will be sent from bit 7 to 0 of command_value, and
         * then bit 15 to 8 of the same register field. Shift and swap to send
         * more straightly.
         */
        hw->user2.usr_command_value = HAL_SPI_SWAP_DATA_TX(cmd, cmdlen);

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
    return hw->slv_rd_byte.slv_rdata_bit * 8;
}


#undef SPI_LL_RST_MASK
#undef SPI_LL_UNUSED_INT_MASK
