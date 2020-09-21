// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash

#pragma once

#include <stdlib.h>
#include "soc/spi_periph.h"
#include "hal/spi_types.h"
#include "hal/spi_flash_types.h"
#include <sys/param.h> // For MIN/MAX
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define gpspi_flash_ll_get_hw(host_id)  ( ((host_id)==SPI2_HOST) ? &GPSPI2 : (\
                                          ((host_id)==SPI3_HOST) ? &GPSPI3 : (\
                                          {abort();(spi_dev_t*)0;}\
                                        )) )
#define gpspi_flash_ll_hw_get_id(dev)   ( ((dev) == (void*)&GPSPI2) ? SPI2_HOST : (\
                                          ((dev) == (void*)&GPSPI3) ? SPI3_HOST : (\
                                          -1 \
                                        )) )

typedef typeof(GPSPI2.clock) gpspi_flash_ll_clock_reg_t;

//Supported clock register values
#define GPSPI_FLASH_LL_CLKREG_VAL_5MHZ   ((gpspi_flash_ll_clock_reg_t){.val=0x0000F1CF})   ///< Clock set to 5 MHz
#define GPSPI_FLASH_LL_CLKREG_VAL_10MHZ  ((gpspi_flash_ll_clock_reg_t){.val=0x000070C7})   ///< Clock set to 10 MHz
#define GPSPI_FLASH_LL_CLKREG_VAL_20MHZ  ((gpspi_flash_ll_clock_reg_t){.val=0x00003043})   ///< Clock set to 20 MHz
#define GPSPI_FLASH_LL_CLKREG_VAL_26MHZ  ((gpspi_flash_ll_clock_reg_t){.val=0x00002002})   ///< Clock set to 26 MHz
#define GPSPI_FLASH_LL_CLKREG_VAL_40MHZ  ((gpspi_flash_ll_clock_reg_t){.val=0x00001001})   ///< Clock set to 40 MHz
#define GPSPI_FLASH_LL_CLKREG_VAL_80MHZ  ((gpspi_flash_ll_clock_reg_t){.val=0x80000000})   ///< Clock set to 80 MHz

/*------------------------------------------------------------------------------
 * Control
 *----------------------------------------------------------------------------*/
/**
 * Reset peripheral registers before configuration and starting control
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void gpspi_flash_ll_reset(spi_dev_t *dev)
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
static inline bool gpspi_flash_ll_cmd_is_done(const spi_dev_t *dev)
{
    return (dev->cmd.val == 0);
}

/**
 * Get the read data from the buffer after ``gpspi_flash_ll_read`` is done.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer to hold the output data
 * @param read_len Length to get out of the buffer
 */
static inline void gpspi_flash_ll_get_buffer_data(spi_dev_t *dev, void *buffer, uint32_t read_len)
{
    if (((intptr_t)buffer % 4 == 0) && (read_len % 4 == 0)) {
        // If everything is word-aligned, do a faster memcpy
        memcpy(buffer, (void *)dev->data_buf, read_len);
    } else {
        // Otherwise, slow(er) path copies word by word
        int copy_len = read_len;
        for (int i = 0; i < (read_len + 3) / 4; i++) {
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
static inline void gpspi_flash_ll_write_word(spi_dev_t *dev, uint32_t word)
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
static inline void gpspi_flash_ll_set_buffer_data(spi_dev_t *dev, const void *buffer, uint32_t length)
{
    // Load data registers, word at a time
    int num_words = (length + 3) / 4;
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
 * Trigger a user defined transaction. All phases, including command, address, dummy, and the data phases,
 * should be configured before this is called.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void gpspi_flash_ll_user_start(spi_dev_t *dev)
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
static inline bool gpspi_flash_ll_host_idle(const spi_dev_t *dev)
{
    return dev->fsm.st == 0;
}

/**
 * Set phases for user-defined transaction to read
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void gpspi_flash_ll_read_phase(spi_dev_t *dev)
{
    typeof (dev->user) user = {
        .usr_command = 1,
        .usr_mosi = 0,
        .usr_miso = 1,
        .usr_addr = 1,
    };
    dev->user = user;
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
static inline void gpspi_flash_ll_set_cs_pin(spi_dev_t *dev, int pin)
{
    dev->misc.cs0_dis = (pin != 0);
    dev->misc.cs1_dis = (pin != 1);
    dev->misc.cs2_dis = (pin != 2);
    dev->misc.cs3_dis = (pin != 3);
    dev->misc.cs4_dis = (pin != 4);
    dev->misc.cs5_dis = (pin != 5);
}

/**
 * Set the read io mode.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param read_mode I/O mode to use in the following transactions.
 */
static inline void gpspi_flash_ll_set_read_mode(spi_dev_t *dev, esp_flash_io_mode_t read_mode)
{
    typeof (dev->ctrl) ctrl = dev->ctrl;
    typeof (dev->user) user = dev->user;

    ctrl.val &= ~(SPI_FCMD_QUAD_M | SPI_FADDR_QUAD_M | SPI_FREAD_QUAD_M | SPI_FCMD_DUAL_M | SPI_FADDR_DUAL_M | SPI_FREAD_DUAL_M);
    user.val &= ~(SPI_FWRITE_QUAD_M | SPI_FWRITE_DUAL_M);

    switch (read_mode) {
    case SPI_FLASH_FASTRD:
        //the default option
    case SPI_FLASH_SLOWRD:
        break;
    case SPI_FLASH_QIO:
        ctrl.fread_quad = 1;
        ctrl.faddr_quad = 1;
        user.fwrite_quad = 1;
        break;
    case SPI_FLASH_QOUT:
        ctrl.fread_quad = 1;
        user.fwrite_quad = 1;
        break;
    case SPI_FLASH_DIO:
        ctrl.fread_dual = 1;
        ctrl.faddr_dual = 1;
        user.fwrite_dual = 1;
        break;
    case SPI_FLASH_DOUT:
        ctrl.fread_dual = 1;
        user.fwrite_dual = 1;
        break;
    default:
        abort();
    }

    dev->ctrl = ctrl;
    dev->user = user;
}

/**
 * Set clock frequency to work at.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param clock_val pointer to the clock value to set
 */
static inline void gpspi_flash_ll_set_clock(spi_dev_t *dev, gpspi_flash_ll_clock_reg_t *clock_val)
{
    dev->clock = *clock_val;
}

/**
 * Set the input length, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of input, in bits.
 */
static inline void gpspi_flash_ll_set_miso_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_miso = bitlen > 0;
    dev->miso_dlen.usr_miso_bit_len = bitlen ? (bitlen - 1) : 0;
}

/**
 * Set the output length, in bits (not including command, address and dummy
 * phases)
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of output, in bits.
 */
static inline void gpspi_flash_ll_set_mosi_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_mosi = bitlen > 0;
    dev->mosi_dlen.usr_mosi_bit_len = bitlen ? (bitlen - 1) : 0;
}

/**
 * Set the command with fixed length (8 bits).
 *
 * @param dev Beginning address of the peripheral registers.
 * @param command Command to send
 */
static inline void gpspi_flash_ll_set_command8(spi_dev_t *dev, uint8_t command)
{
    dev->user.usr_command = 1;
    typeof(dev->user2) user2 = {
        .usr_command_value = command,
        .usr_command_bitlen = (8 - 1),
    };
    dev->user2 = user2;
}

/**
 * Get the address length that is set in register, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 */
static inline int gpspi_flash_ll_get_addr_bitlen(spi_dev_t *dev)
{
    return dev->user.usr_addr ? dev->user1.usr_addr_bitlen + 1 : 0;
}

/**
 * Set the address length to send, in bits. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of the address, in bits
 */
static inline void gpspi_flash_ll_set_addr_bitlen(spi_dev_t *dev, uint32_t bitlen)
{
    dev->user1.usr_addr_bitlen = (bitlen - 1);
    dev->user.usr_addr = bitlen ? 1 : 0;
}

/**
 * Set the address to send in user mode. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void gpspi_flash_ll_set_usr_address(spi_dev_t *dev, uint32_t addr, uint32_t bitlen)
{
    dev->addr = (addr << (32 - bitlen));
}

/**
 * Set the address to send. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void gpspi_flash_ll_set_address(spi_dev_t *dev, uint32_t addr)
{
    dev->addr = addr;
}

/**
 * Set the length of dummy cycles.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param dummy_n Cycles of dummy phases
 */
static inline void gpspi_flash_ll_set_dummy(spi_dev_t *dev, uint32_t dummy_n)
{
    dev->user.usr_dummy = dummy_n ? 1 : 0;
    dev->user1.usr_dummy_cyclelen = dummy_n - 1;
}

/**
 * Set D/Q output level during dummy phase
 *
 * @param dev Beginning address of the peripheral registers.
 * @param out_en whether to enable IO output for dummy phase
 * @param out_level dummy output level
 */
static inline void gpspi_flash_ll_set_dummy_out(spi_dev_t *dev, uint32_t out_en, uint32_t out_lev)
{
    dev->ctrl.dummy_out = out_en;
    dev->ctrl.q_pol = out_lev;
    dev->ctrl.d_pol = out_lev;
}

#ifdef __cplusplus
}
#endif
