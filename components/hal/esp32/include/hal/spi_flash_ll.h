/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash

#pragma once

#include <stdlib.h>
#include "soc/spi_periph.h"
#include "soc/spi_struct.h"
#include "hal/spi_types.h"
#include "hal/spi_flash_types.h"
#include <sys/param.h> // For MIN/MAX
#include <stdbool.h>
#include <string.h>
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_FLASH_LL_CLOCK_FREQUENCY_MHZ (80)

/// Get the start address of SPI peripheral registers by the host ID
#define spi_flash_ll_get_hw(host_id) ( ((host_id)==SPI1_HOST) ? &SPI1 :(\
                                       ((host_id)==SPI2_HOST) ? &SPI2 :(\
                                       ((host_id)==SPI3_HOST) ? &SPI3 :(\
                                       {abort();(spi_dev_t*)0;}\
                                     ))) )
#define spi_flash_ll_hw_get_id(dev) ( ((dev) == &SPI1) ? SPI1_HOST :(\
                                      ((dev) == &SPI2) ? SPI2_HOST :(\
                                      ((dev) == &SPI3) ? SPI3_HOST :(\
                                      -1\
                                    ))) )

/// Empty function to be compatible with new version chips.
#define spi_flash_ll_set_dummy_out(dev, out_en, out_lev)

// On ESP32, we extent 4 bits to occupy `Continuous Read Mode` bits. (same to origin code.)
#define SPI_FLASH_LL_CONTINUOUS_MODE_BIT_NUMS (4)

/// type to store pre-calculated register value in above layers
typedef typeof(SPI1.clock.val) spi_flash_ll_clock_reg_t;

/*------------------------------------------------------------------------------
 * Control
 *----------------------------------------------------------------------------*/
/**
 * Reset peripheral registers before configuration and starting control
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spi_flash_ll_reset(spi_dev_t *dev)
{
    dev->user.val = 0;
    dev->ctrl.val = 0;
}

/**
 * Check whether the previous operation is done.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 * @return true if last command is done, otherwise false.
 */
static inline bool spi_flash_ll_cmd_is_done(const spi_dev_t *dev)
{
    return (dev->cmd.val == 0);
}

/**
 * Erase the flash chip.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spi_flash_ll_erase_chip(spi_dev_t *dev)
{
    dev->cmd.flash_ce = 1;
}

/**
 * Erase the sector, the address should be set by spi_flash_ll_set_address.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spi_flash_ll_erase_sector(spi_dev_t *dev)
{
    dev->ctrl.val = 0;
    dev->cmd.flash_se = 1;
}

/**
 * Erase the block, the address should be set by spi_flash_ll_set_address.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spi_flash_ll_erase_block(spi_dev_t *dev)
{
    dev->cmd.flash_be = 1;
}

/**
 * Enable/disable write protection for the flash chip.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param wp true to enable the protection, false to disable (write enable).
 */
static inline void spi_flash_ll_set_write_protect(spi_dev_t *dev, bool wp)
{
    if (wp) {
        dev->cmd.flash_wrdi = 1;
    } else {
        dev->cmd.flash_wren = 1;
    }
}

/**
 * Get the read data from the buffer after ``spi_flash_ll_read`` is done.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer to hold the output data
 * @param read_len Length to get out of the buffer
 */
static inline void spi_flash_ll_get_buffer_data(spi_dev_t *dev, void *buffer, uint32_t read_len)
{
    if (((intptr_t)buffer % 4 == 0) && (read_len % 4 == 0)) {
        // If everything is word-aligned, do a faster memcpy
        memcpy(buffer, (void *)dev->data_buf, read_len);
    } else {
        // Otherwise, slow(er) path copies word by word
        int copy_len = read_len;
        for (size_t i = 0; i < (read_len + 3) / 4; i++) {
            int word_len = MIN(sizeof(uint32_t), copy_len);
            uint32_t word = dev->data_buf[i];
            memcpy(buffer, &word, word_len);
            buffer = (void *)((intptr_t)buffer + word_len);
            copy_len -= word_len;
        }
    }
}

/**
 * Write a word to the data buffer.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param word Data to write at address 0.
 */
static inline void spi_flash_ll_write_word(spi_dev_t *dev, uint32_t word)
{
    dev->data_buf[0] = word;
}

/**
 * Set the data to be written in the data buffer.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer holding the data
 * @param length Length of data in bytes.
 */
static inline void spi_flash_ll_set_buffer_data(spi_dev_t *dev, const void *buffer, uint32_t length)
{
    // Load data registers, word at a time
    int num_words = (length + 3) >> 2;
    for (int i = 0; i < num_words; i++) {
        uint32_t word = 0;
        uint32_t word_len = MIN(length, sizeof(word));
        memcpy(&word, buffer, word_len);
        dev->data_buf[i] = word;
        length -= word_len;
        buffer = (void *)((intptr_t)buffer + word_len);
    }
}

/**
 * Program a page of the flash chip. Call ``spi_flash_ll_set_address`` before
 * this to set the address to program.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer holding the data to program
 * @param length Length to program.
 */
static inline void spi_flash_ll_program_page(spi_dev_t *dev, const void *buffer, uint32_t length)
{
    dev->user.usr_dummy = 0;
    spi_flash_ll_set_buffer_data(dev, buffer, length);
    dev->cmd.flash_pp = 1;
}

/**
 * Trigger a user defined transaction. All phases, including command, address, dummy, and the data phases,
 * should be configured before this is called.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spi_flash_ll_user_start(spi_dev_t *dev)
{
    dev->cmd.usr = 1;
}

/**
 * Check whether the host is idle to perform new commands.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 * @return true if the host is idle, otherwise false
 */
static inline bool spi_flash_ll_host_idle(const spi_dev_t *dev)
{
    return dev->ext2.st == 0;
}

/*------------------------------------------------------------------------------
 * Configs
 *----------------------------------------------------------------------------*/
/**
 * Select which pin to use for the flash
 *
 * @param dev Beginning address of the peripheral registers.
 * @param pin Pin ID to use, 0-2. Set to other values to disable all the CS pins.
 */
static inline void spi_flash_ll_set_cs_pin(spi_dev_t *dev, int pin)
{
    dev->pin.cs0_dis = (pin != 0);
    dev->pin.cs1_dis = (pin != 1);
    dev->pin.cs2_dis = (pin != 2);
}

/**
 * Set the read io mode.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param read_mode I/O mode to use in the following transactions.
 */
static inline void spi_flash_ll_set_read_mode(spi_dev_t *dev, esp_flash_io_mode_t read_mode)
{
    typeof (dev->ctrl) ctrl = dev->ctrl;
    ctrl.val &= ~(SPI_FREAD_QIO_M | SPI_FREAD_QUAD_M | SPI_FREAD_DIO_M | SPI_FREAD_DUAL_M);
    ctrl.val |= SPI_FASTRD_MODE_M;
    switch (read_mode) {
    case SPI_FLASH_FASTRD:
        //the default option
        break;
    case SPI_FLASH_QIO:
        ctrl.fread_qio = 1;
        break;
    case SPI_FLASH_QOUT:
        ctrl.fread_quad = 1;
        break;
    case SPI_FLASH_DIO:
        ctrl.fread_dio = 1;
        break;
    case SPI_FLASH_DOUT:
        ctrl.fread_dual = 1;
        break;
    case SPI_FLASH_SLOWRD:
        ctrl.fastrd_mode = 0;
        break;
    default:
        abort();
    }
    dev->ctrl = ctrl;
}

/**
 * Set clock frequency to work at.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param clock_val pointer to the clock value to set
 */
static inline void spi_flash_ll_set_clock(spi_dev_t *dev, spi_flash_ll_clock_reg_t *clock_val)
{
    dev->clock.val = *clock_val;
}

/**
 * Set the input length, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of input, in bits.
 */
static inline void spi_flash_ll_set_miso_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_miso = bitlen > 0;
    dev->miso_dlen.usr_miso_dbitlen = bitlen ? (bitlen - 1) : 0;
}

/**
 * Set the output length, in bits (not including command, address and dummy
 * phases)
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of output, in bits.
 */
static inline void spi_flash_ll_set_mosi_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_mosi = bitlen > 0;
    dev->mosi_dlen.usr_mosi_dbitlen = bitlen ? (bitlen - 1) : 0;
}

/**
 * Set the command.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param command Command to send
 * @param bitlen Length of the command
 */
static inline void spi_flash_ll_set_command(spi_dev_t *dev, uint8_t command, uint32_t bitlen)
{
    dev->user.usr_command = 1;
    typeof(dev->user2) user2 = {
        .usr_command_value = command,
        .usr_command_bitlen = (bitlen - 1),
    };
    dev->user2 = user2;
}

/**
 * Get the address length that is set in register, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 */
static inline int spi_flash_ll_get_addr_bitlen(spi_dev_t *dev)
{
    return dev->user.usr_addr ? dev->user1.usr_addr_bitlen + 1 : 0;
}

/**
 * Set the address length to send, in bits. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of the address, in bits
 */
static inline void spi_flash_ll_set_addr_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user1.usr_addr_bitlen = (bitlen - 1);
    dev->user.usr_addr = bitlen ? 1 : 0;
}

/**
 * Set the address to send in user command mode. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void spi_flash_ll_set_usr_address(spi_dev_t *dev, uint32_t addr, int bit_len)
{
    // The blank region should be all ones
    if (bit_len >= 32) {
        dev->addr = addr;
        dev->slv_wr_status = UINT32_MAX;
    } else {
        uint32_t padding_ones = UINT32_MAX >> bit_len;
        dev->addr = (addr << (32 - bit_len)) | padding_ones;
    }
}

/**
 * Set the address to send. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void spi_flash_ll_set_address(spi_dev_t *dev, uint32_t addr)
{
    dev->addr = addr;
}

/**
 * Set the length of dummy cycles.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param dummy_n Cycles of dummy phases
 */
static inline void spi_flash_ll_set_dummy(spi_dev_t *dev, uint32_t dummy_n)
{
    dev->user.usr_dummy = dummy_n ? 1 : 0;
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->user1, usr_dummy_cyclelen, dummy_n - 1);
}

static inline void spi_flash_ll_set_hold(spi_dev_t *dev, uint32_t hold_n)
{
    dev->ctrl2.hold_time = hold_n;
    dev->user.cs_hold = (hold_n > 0? 1: 0);
}

static inline void spi_flash_ll_set_cs_setup(spi_dev_t *dev, uint32_t cs_setup_time)
{
    dev->user.cs_setup = (cs_setup_time > 0 ? 1 : 0);
    dev->ctrl2.setup_time = cs_setup_time - 1;
}

/**
 * Get the spi flash source clock frequency. Used for calculating
 * the divider parameters.
 *
 * @param host_id SPI host id. Not used in this function, but to keep
 * compatibility with other targets.
 *
 * @return the frequency of spi flash clock source.(MHz)
 */
static inline uint32_t spi_flash_ll_get_source_clock_freq_mhz(uint8_t host_id)
{
    return SPI_FLASH_LL_CLOCK_FREQUENCY_MHZ;
}

/**
 * Calculate spi_flash clock frequency division parameters for register.
 *
 * @param host_id SPI host id. Not used in this function, but to keep
 * compatibility with other targets.
 * @param clkdiv frequency division factor
 *
 * @return Register setting for the given clock division factor.
 */
static inline uint32_t spi_flash_ll_calculate_clock_reg(uint8_t host_id, uint8_t clkdiv)
{
    uint32_t div_parameter;
    // See comments of `clock` in `spi_struct.h`
    if (clkdiv == 1) {
        div_parameter = (1 << 31);
    } else {
        div_parameter = ((clkdiv - 1) | (((clkdiv/2 - 1) & 0xff) << 6 ) | (((clkdiv - 1) & 0xff) << 12));
    }
    return div_parameter;
}

/**
 * Set extra address for bits M0-M7 in DIO/QIO mode.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param extra_addr extra address(M0-M7) to send.
 */
static inline void spi_flash_ll_set_extra_address(spi_dev_t *dev, uint32_t extra_addr)
{
    // Not supported on ESP32.
}

#ifdef __cplusplus
}
#endif
