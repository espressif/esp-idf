/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The LL layer for SPI register operations

#pragma once

#include <stdlib.h> //for abs()
#include <string.h>
#include "esp_attr.h"
#include "esp_types.h"
#include "soc/spi_periph.h"
#include "soc/spi_struct.h"
#include "soc/lldesc.h"
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/spi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Interrupt not used. Don't use in app.
#define SPI_LL_UNUSED_INT_MASK  (SPI_TRANS_DONE_INT_ENA | SPI_SLV_WR_DMA_DONE_INT_ENA | SPI_SLV_RD_DMA_DONE_INT_ENA | SPI_SLV_WR_BUF_DONE_INT_ENA | SPI_SLV_RD_BUF_DONE_INT_ENA)
/// These 2 masks together will set SPI transaction to one line mode
#define SPI_LL_ONE_LINE_CTRL_MASK (SPI_FREAD_QUAD | SPI_FREAD_DUAL | SPI_FCMD_QUAD | SPI_FCMD_DUAL | SPI_FADDR_QUAD | SPI_FADDR_DUAL)
#define SPI_LL_ONE_LINE_USER_MASK (SPI_FWRITE_QUAD | SPI_FWRITE_DUAL)
/// Swap the bit order to its correct place to send
#define HAL_SPI_SWAP_DATA_TX(data, len) HAL_SWAP32((uint32_t)(data) << (32 - len))
/// This is the expected clock frequency
#define SPI_LL_PERIPH_CLK_FREQ (80 * 1000000)
#define SPI_LL_GET_HW(ID) ((ID)==0? ({abort();NULL;}):&GPSPI2)

/**
 * The data structure holding calculated clock configuration. Since the
 * calculation needs long time, it should be calculated during initialization and
 * stored somewhere to be quickly used.
 */
typedef uint32_t spi_ll_clock_val_t;
typedef spi_dev_t spi_dma_dev_t;

// Type definition of all supported interrupts
typedef enum {
    SPI_LL_INTR_TRANS_DONE =    BIT(0),     ///< A transaction has done
    SPI_LL_INTR_RDBUF =         BIT(6),     ///< Has received RDBUF command. Only available in slave HD.
    SPI_LL_INTR_WRBUF =         BIT(7),     ///< Has received WRBUF command. Only available in slave HD.
    SPI_LL_INTR_RDDMA =         BIT(8),     ///< Has received RDDMA command. Only available in slave HD.
    SPI_LL_INTR_WRDMA =         BIT(9),     ///< Has received WRDMA command. Only available in slave HD.
    SPI_LL_INTR_CMD7  =         BIT(10),    ///< Has received CMD7 command. Only available in slave HD.
    SPI_LL_INTR_CMD8 =          BIT(11),    ///< Has received CMD8 command. Only available in slave HD.
    SPI_LL_INTR_CMD9 =          BIT(12),    ///< Has received CMD9 command. Only available in slave HD.
    SPI_LL_INTR_CMDA =          BIT(13),    ///< Has received CMDA command. Only available in slave HD.
    SPI_LL_INTR_SEG_DONE =      BIT(14),
} spi_ll_intr_t;
FLAG_ATTR(spi_ll_intr_t)

// Flags for conditions under which the transaction length should be recorded
typedef enum {
    SPI_LL_TRANS_LEN_COND_WRBUF =   BIT(0), ///< WRBUF length will be recorded
    SPI_LL_TRANS_LEN_COND_RDBUF =   BIT(1), ///< RDBUF length will be recorded
    SPI_LL_TRANS_LEN_COND_WRDMA =   BIT(2), ///< WRDMA length will be recorded
    SPI_LL_TRANS_LEN_COND_RDDMA =   BIT(3), ///< RDDMA length will be recorded
} spi_ll_trans_len_cond_t;
FLAG_ATTR(spi_ll_trans_len_cond_t)

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
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Initialize SPI peripheral (slave).
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_init(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Initialize SPI peripheral (slave half duplex mode)
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_hd_init(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Check whether user-defined transaction is done.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return   True if transaction is done, otherwise false.
 */
static inline bool spi_ll_usr_is_done(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Trigger start of user-defined transaction for master.
 * The synchronization between two clock domains is required in ESP32-S3
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_master_user_start(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Trigger start of user-defined transaction for slave.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_user_start(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Get current running command bit-mask. (Preview)
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return   Bitmask of running command, see ``SPI_CMD_REG``. 0 if no in-flight command.
 */
static inline uint32_t spi_ll_get_running_cmd(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Reset the slave peripheral before next transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_slave_reset(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Reset SPI CPU TX FIFO
 *
 * On ESP32C3, this function is not seperated
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_cpu_tx_fifo_reset(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Reset SPI CPU RX FIFO
 *
 * On ESP32C3, this function is not seperated
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_cpu_rx_fifo_reset(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Reset SPI DMA TX FIFO
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_dma_tx_fifo_reset(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Reset SPI DMA RX FIFO
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_dma_rx_fifo_reset(spi_dev_t *hw)
{
   abort(); // TODO: SPI support IDF-4024
}

/**
 * Clear in fifo full error
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_infifo_full_clr(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Clear out fifo empty error
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_outfifo_empty_clr(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * DMA
 *----------------------------------------------------------------------------*/
/**
 * Enable/Disable RX DMA (Peripherals->DMA->RAM)
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: enable; 2: disable
 */
static inline void spi_ll_dma_rx_enable(spi_dev_t *hw, bool enable)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Enable/Disable TX DMA (RAM->DMA->Peripherals)
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: enable; 2: disable
 */
static inline void spi_ll_dma_tx_enable(spi_dev_t *hw, bool enable)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Configuration of RX DMA EOF interrupt generation way
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable 1: spi_dma_inlink_eof is set when the number of dma pushed data bytes is equal to the value of spi_slv/mst_dma_rd_bytelen[19:0] in spi dma transition.  0: spi_dma_inlink_eof is set by spi_trans_done in non-seg-trans or spi_dma_seg_trans_done in seg-trans.
 */
static inline void spi_ll_dma_set_rx_eof_generation(spi_dev_t *hw, bool enable)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * Buffer
 *----------------------------------------------------------------------------*/
/**
 * Write to SPI hardware data buffer.
 *
 * @param hw             Beginning address of the peripheral registers.
 * @param buffer_to_send Address of the data to be written to the hardware data buffer.
 * @param bitlen         Length to write, in bits.
 */
static inline void spi_ll_write_buffer(spi_dev_t *hw, const uint8_t *buffer_to_send, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Write to SPI hardware data buffer by buffer ID (address)
 *
 * @param hw      Beginning address of the peripheral registers
 * @param byte_id Start ID (address) of the hardware buffer to be written
 * @param data    Address of the data to be written to the hardware data buffer.
 * @param len     Length to write, in bytes.
 */
static inline void spi_ll_write_buffer_byte(spi_dev_t *hw, int byte_id, uint8_t *data, int len)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Read from SPI hardware data buffer.
 *
 * @param hw            Beginning address of the peripheral registers.
 * @param buffer_to_rcv Address of a buffer to read data from hardware data buffer
 * @param bitlen        Length to read, in bits.
 */
static inline void spi_ll_read_buffer(spi_dev_t *hw, uint8_t *buffer_to_rcv, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Read from SPI hardware data buffer by buffer ID (address)
 *
 * @param hw      Beginning address of the peripheral registers
 * @param byte_id Start ID (address) of the hardware buffer to be read
 * @param data    Address of a buffer to read data from hardware data buffer
 * @param len     Length to read, in bytes.
 */
static inline void spi_ll_read_buffer_byte(spi_dev_t *hw, int byte_id, uint8_t *out_data, int len)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * Configs: mode
 *----------------------------------------------------------------------------*/
/**
 * Enable/disable the postive-cs feature.
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param cs     One of the CS (0-2) to enable/disable the feature.
 * @param pos_cs True to enable the feature, otherwise disable (default).
 */
static inline void spi_ll_master_set_pos_cs(spi_dev_t *hw, int cs, uint32_t pos_cs)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Enable/disable the LSBFIRST feature for TX data.
 *
 * @param hw       Beginning address of the peripheral registers.
 * @param lsbfirst True if LSB of TX data to be sent first, otherwise MSB is sent first (default).
 */
static inline void spi_ll_set_tx_lsbfirst(spi_dev_t *hw, bool lsbfirst)
{
    hw->ctrl.wr_bit_order = lsbfirst;
}

/**
 * Enable/disable the LSBFIRST feature for RX data.
 *
 * @param hw       Beginning address of the peripheral registers.
 * @param lsbfirst True if first bit received as LSB, otherwise as MSB (default).
 */
static inline void spi_ll_set_rx_lsbfirst(spi_dev_t *hw, bool lsbfirst)
{
    hw->ctrl.rd_bit_order = lsbfirst;
}

/**
 * Set SPI mode for the peripheral as master.
 *
 * @param hw   Beginning address of the peripheral registers.
 * @param mode SPI mode to work at, 0-3.
 */
static inline void spi_ll_master_set_mode(spi_dev_t *hw, uint8_t mode)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set SPI mode for the peripheral as slave.
 *
 * @param hw   Beginning address of the peripheral registers.
 * @param mode SPI mode to work at, 0-3.
 */
static inline void spi_ll_slave_set_mode(spi_dev_t *hw, const int mode, bool dma_used)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set SPI to work in full duplex or half duplex mode.
 *
 * @param hw          Beginning address of the peripheral registers.
 * @param half_duplex True to work in half duplex mode, otherwise in full duplex mode.
 */
static inline void spi_ll_set_half_duplex(spi_dev_t *hw, bool half_duplex)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set SPI to work in SIO mode or not.
 *
 * SIO is a mode which MOSI and MISO share a line. The device MUST work in half-duplexmode.
 *
 * @param hw       Beginning address of the peripheral registers.
 * @param sio_mode True to work in SIO mode, otherwise false.
 */
static inline void spi_ll_set_sio_mode(spi_dev_t *hw, int sio_mode)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Configure the SPI transaction line mode for the master to use.
 *
 * @param hw        Beginning address of the peripheral registers.
 * @param line_mode SPI transaction line mode to use, see ``spi_line_mode_t``.
 */
static inline void spi_ll_master_set_line_mode(spi_dev_t *hw, spi_line_mode_t line_mode)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the SPI slave to work in segment transaction mode
 *
 * @param hw        Beginning address of the peripheral registers.
 * @param seg_trans True to work in seg mode, otherwise false.
 */
static inline void spi_ll_slave_set_seg_mode(spi_dev_t *hw, bool seg_trans)
{
    hw->dma_conf.dma_seg_trans_en = seg_trans;
}

/**
 * Select one of the CS to use in current transaction.
 *
 * @param hw    Beginning address of the peripheral registers.
 * @param cs_id The cs to use, 0-2, otherwise none of them is used.
 */
static inline void spi_ll_master_select_cs(spi_dev_t *hw, int cs_id)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Keep Chip Select activated after the current transaction.
 *
 * @param hw Beginning address of the peripheral registers.
 * @param keep_active if 0 don't keep CS activated, else keep CS activated
 */
static inline void spi_ll_master_keep_cs(spi_dev_t *hw, int keep_active)
{
    hw->misc.cs_keep_active = (keep_active != 0) ? 1 : 0;
}

/*------------------------------------------------------------------------------
 * Configs: parameters
 *----------------------------------------------------------------------------*/
/**
 * Set the clock for master by stored value.
 *
 * @param hw  Beginning address of the peripheral registers.
 * @param val Stored clock configuration calculated before (by ``spi_ll_cal_clock``).
 */
static inline void spi_ll_master_set_clock_by_reg(spi_dev_t *hw, const spi_ll_clock_val_t *val)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Get the frequency of given dividers. Don't use in app.
 *
 * @param fapb APB clock of the system.
 * @param pre  Pre devider.
 * @param n    Main divider.
 *
 * @return     Frequency of given dividers.
 */
static inline int spi_ll_freq_for_pre_n(int fapb, int pre, int n)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Calculate the nearest frequency avaliable for master.
 *
 * @param fapb       APB clock of the system.
 * @param hz         Frequncy desired.
 * @param duty_cycle Duty cycle desired.
 * @param out_reg    Output address to store the calculated clock configurations for the return frequency.
 *
 * @return           Actual (nearest) frequency.
 */
static inline int spi_ll_master_cal_clock(int fapb, int hz, int duty_cycle, spi_ll_clock_val_t *out_reg)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Calculate and set clock for SPI master according to desired parameters.
 *
 * This takes long, suggest to calculate the configuration during
 * initialization by ``spi_ll_master_cal_clock`` and store the result, then
 * configure the clock by stored value when used by
 * ``spi_ll_msater_set_clock_by_reg``.
 *
 * @param hw         Beginning address of the peripheral registers.
 * @param fapb       APB clock of the system.
 * @param hz         Frequncy desired.
 * @param duty_cycle Duty cycle desired.
 *
 * @return           Actual frequency that is used.
 */
static inline int spi_ll_master_set_clock(spi_dev_t *hw, int fapb, int hz, int duty_cycle)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the mosi delay after the output edge to the signal. (Preview)
 *
 * The delay mode/num is a Espressif conception, may change in the new chips.
 *
 * @param hw         Beginning address of the peripheral registers.
 * @param delay_mode Delay mode, see TRM.
 * @param delay_num  APB clocks to delay.
 */
static inline void spi_ll_set_mosi_delay(spi_dev_t *hw, int delay_mode, int delay_num)
{
}

/**
 * Set the miso delay applied to the input signal before the internal peripheral. (Preview)
 *
 * The delay mode/num is a Espressif conception, may change in the new chips.
 *
 * @param hw         Beginning address of the peripheral registers.
 * @param delay_mode Delay mode, see TRM.
 * @param delay_num  APB clocks to delay.
 */
static inline void spi_ll_set_miso_delay(spi_dev_t *hw, int delay_mode, int delay_num)
{
}

/**
 * Set the delay of SPI clocks before the CS inactive edge after the last SPI clock.
 *
 * @param hw   Beginning address of the peripheral registers.
 * @param hold Delay of SPI clocks after the last clock, 0 to disable the hold phase.
 */
static inline void spi_ll_master_set_cs_hold(spi_dev_t *hw, int hold)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the delay of SPI clocks before the first SPI clock after the CS active edge.
 *
 * Note ESP32 doesn't support to use this feature when command/address phases
 * are used in full duplex mode.
 *
 * @param hw    Beginning address of the peripheral registers.
 * @param setup Delay of SPI clocks after the CS active edge, 0 to disable the setup phase.
 */
static inline void spi_ll_master_set_cs_setup(spi_dev_t *hw, uint8_t setup)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * Configs: data
 *----------------------------------------------------------------------------*/
/**
 * Set the output length (master).
 * This should be called before master setting MISO(input) length
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen output length, in bits.
 */
static inline void spi_ll_set_mosi_bitlen(spi_dev_t *hw, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the input length (master).
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen input length, in bits.
 */
static inline void spi_ll_set_miso_bitlen(spi_dev_t *hw, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the maximum input length (slave).
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen Input length, in bits.
 */
static inline void spi_ll_slave_set_rx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the maximum output length (slave).
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen Output length, in bits.
 */
static inline void spi_ll_slave_set_tx_bitlen(spi_dev_t *hw, size_t bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the length of command phase.
 *
 * When in 4-bit mode, the SPI cycles of the phase will be shorter. E.g. 16-bit
 * command phases takes 4 cycles in 4-bit mode.
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen Length of command phase, in bits. 0 to disable the command phase.
 */
static inline void spi_ll_set_command_bitlen(spi_dev_t *hw, int bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the length of address phase.
 *
 * When in 4-bit mode, the SPI cycles of the phase will be shorter. E.g. 16-bit
 * address phases takes 4 cycles in 4-bit mode.
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param bitlen Length of address phase, in bits. 0 to disable the address phase.
 */
static inline void spi_ll_set_addr_bitlen(spi_dev_t *hw, int bitlen)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the address value in an intuitive way.
 *
 * The length and lsbfirst is required to shift and swap the address to the right place.
 *
 * @param hw       Beginning address of the peripheral registers.
 * @param address  Address to set
 * @param addrlen  Length of the address phase
 * @param lsbfirst Whether the LSB first feature is enabled.
 */
static inline void spi_ll_set_address(spi_dev_t *hw, uint64_t addr, int addrlen, uint32_t lsbfirst)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the command value in an intuitive way.
 *
 * The length and lsbfirst is required to shift and swap the command to the right place.
 *
 * @param hw       Beginning command of the peripheral registers.
 * @param command  Command to set
 * @param addrlen  Length of the command phase
 * @param lsbfirst Whether the LSB first feature is enabled.
 */
static inline void spi_ll_set_command(spi_dev_t *hw, uint16_t cmd, int cmdlen, bool lsbfirst)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set dummy clocks to output before RX phase (master), or clocks to skip
 * before the data phase and after the address phase (slave).
 *
 * Note this phase is also used to compensate RX timing in half duplex mode.
 *
 * @param hw      Beginning address of the peripheral registers.
 * @param dummy_n Dummy cycles used. 0 to disable the dummy phase.
 */
static inline void spi_ll_set_dummy(spi_dev_t *hw, int dummy_n)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Enable/disable the RX data phase.
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable True if RX phase exist, otherwise false.
 */
static inline void spi_ll_enable_miso(spi_dev_t *hw, int enable)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Enable/disable the TX data phase.
 *
 * @param hw     Beginning address of the peripheral registers.
 * @param enable True if TX phase exist, otherwise false.
 */
static inline void spi_ll_enable_mosi(spi_dev_t *hw, int enable)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Get the received bit length of the slave.
 *
 * @param hw Beginning address of the peripheral registers.
 *
 * @return   Received bits of the slave.
 */
static inline uint32_t spi_ll_slave_get_rcv_bitlen(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * Interrupts
 *----------------------------------------------------------------------------*/
//helper macros to generate code for each interrupts
#define FOR_EACH_ITEM(op, list) do { list(op) } while(0)
#define INTR_LIST(item)    \
    item(SPI_LL_INTR_TRANS_DONE,    dma_int_ena.trans_done,         dma_int_raw.trans_done,         dma_int_clr.trans_done=1) \
    item(SPI_LL_INTR_RDBUF,         dma_int_ena.rd_buf_done,        dma_int_raw.rd_buf_done,        dma_int_clr.rd_buf_done=1) \
    item(SPI_LL_INTR_WRBUF,         dma_int_ena.wr_buf_done,        dma_int_raw.wr_buf_done,        dma_int_clr.wr_buf_done=1) \
    item(SPI_LL_INTR_RDDMA,         dma_int_ena.rd_dma_done,        dma_int_raw.rd_dma_done,        dma_int_clr.rd_dma_done=1) \
    item(SPI_LL_INTR_WRDMA,         dma_int_ena.wr_dma_done,        dma_int_raw.wr_dma_done,        dma_int_clr.wr_dma_done=1) \
    item(SPI_LL_INTR_SEG_DONE,      dma_int_ena.dma_seg_trans_done, dma_int_raw.dma_seg_trans_done, dma_int_clr.dma_seg_trans_done=1) \
    item(SPI_LL_INTR_CMD7,          dma_int_ena.cmd7,               dma_int_raw.cmd7,               dma_int_clr.cmd7=1) \
    item(SPI_LL_INTR_CMD8,          dma_int_ena.cmd8,               dma_int_raw.cmd8,               dma_int_clr.cmd8=1) \
    item(SPI_LL_INTR_CMD9,          dma_int_ena.cmd9,               dma_int_raw.cmd9,               dma_int_clr.cmd9=1) \
    item(SPI_LL_INTR_CMDA,          dma_int_ena.cmda,               dma_int_raw.cmda,               dma_int_clr.cmda=1)


static inline void spi_ll_enable_intr(spi_dev_t *hw, spi_ll_intr_t intr_mask)
{
#define ENA_INTR(intr_bit, en_reg, ...) if (intr_mask & (intr_bit)) hw->en_reg = 1;
    FOR_EACH_ITEM(ENA_INTR, INTR_LIST);
#undef ENA_INTR
}

static inline void spi_ll_disable_intr(spi_dev_t *hw, spi_ll_intr_t intr_mask)
{
#define DIS_INTR(intr_bit, en_reg, ...) if (intr_mask & (intr_bit)) hw->en_reg = 0;
    FOR_EACH_ITEM(DIS_INTR, INTR_LIST);
#undef DIS_INTR
}

static inline void spi_ll_set_intr(spi_dev_t *hw, spi_ll_intr_t intr_mask)
{
#define SET_INTR(intr_bit, _, st_reg, ...) if (intr_mask & (intr_bit)) hw->st_reg = 1;
    FOR_EACH_ITEM(SET_INTR, INTR_LIST);
#undef SET_INTR
}

static inline void spi_ll_clear_intr(spi_dev_t *hw, spi_ll_intr_t intr_mask)
{
#define CLR_INTR(intr_bit, _, __, clr_reg) if (intr_mask & (intr_bit)) hw->clr_reg;
    FOR_EACH_ITEM(CLR_INTR, INTR_LIST);
#undef CLR_INTR
}

static inline bool spi_ll_get_intr(spi_dev_t *hw, spi_ll_intr_t intr_mask)
{
#define GET_INTR(intr_bit, _, st_reg, ...) if (intr_mask & (intr_bit) && hw->st_reg) return true;
    FOR_EACH_ITEM(GET_INTR, INTR_LIST);
    return false;
#undef GET_INTR
}

#undef FOR_EACH_ITEM
#undef INTR_LIST

/**
 * Disable the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_disable_int(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Clear the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_clear_int_stat(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Set the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_set_int_stat(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/**
 * Enable the trans_done interrupt.
 *
 * @param hw Beginning address of the peripheral registers.
 */
static inline void spi_ll_enable_int(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

/*------------------------------------------------------------------------------
 * Slave HD
 *----------------------------------------------------------------------------*/
static inline void spi_ll_slave_hd_set_len_cond(spi_dev_t *hw, spi_ll_trans_len_cond_t cond_mask)
{
    abort(); // TODO: SPI support IDF-4024
}

static inline int spi_ll_slave_get_rx_byte_len(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

static inline uint32_t spi_ll_slave_hd_get_last_addr(spi_dev_t *hw)
{
    abort(); // TODO: SPI support IDF-4024
}

#undef SPI_LL_RST_MASK
#undef SPI_LL_UNUSED_INT_MASK

#ifdef __cplusplus
}
#endif
