/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash

#pragma once

#include <stdlib.h>
#include <sys/param.h> // For MIN/MAX
#include <stdbool.h>
#include <string.h>

#include "soc/spi_periph.h"
#include "hal/spi_types.h"
#include "hal/spi_flash_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define spimem_flash_ll_get_hw(host_id)  (((host_id)==SPI1_HOST ?  &SPIMEM1 : NULL ))
#define spimem_flash_ll_hw_get_id(dev)   ((dev) == (void*)&SPIMEM1? SPI1_HOST: -1)

#define SPIMEM_FLASH_LL_PERIPHERAL_FREQUENCY_MHZ  (60)
#define SPIMEM_FLASH_LL_SPI0_MAX_LOCK_VAL_MSPI_TICKS  (0x1f)

typedef typeof(SPIMEM1.clock.val) spimem_flash_ll_clock_reg_t;

/*------------------------------------------------------------------------------
 * Control
 *----------------------------------------------------------------------------*/
/**
 * Reset peripheral registers before configuration and starting control
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_reset(spi_mem_dev_t *dev)
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
static inline bool spimem_flash_ll_cmd_is_done(const spi_mem_dev_t *dev)
{
    return (dev->cmd.val == 0);
}

/**
 * Erase the flash chip.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_erase_chip(spi_mem_dev_t *dev)
{
    dev->cmd.flash_ce = 1;
}

/**
 * Erase the sector, the address should be set by spimem_flash_ll_set_address.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_erase_sector(spi_mem_dev_t *dev)
{
    dev->ctrl.val = 0;
    dev->cmd.flash_se = 1;
}

/**
 * Erase the block, the address should be set by spimem_flash_ll_set_address.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_erase_block(spi_mem_dev_t *dev)
{
    dev->cmd.flash_be = 1;
}

/**
 * Suspend erase/program operation.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_suspend(spi_mem_dev_t *dev)
{
    dev->flash_sus_ctrl.flash_pes = 1;
}

/**
 * Resume suspended erase/program operation.
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_resume(spi_mem_dev_t *dev)
{
    dev->flash_sus_ctrl.flash_per = 1;
}

/**
 * Initialize auto suspend mode, and esp32c2 doesn't support disable auto-suspend.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param auto_sus Enable/disable Flash Auto-Suspend.
 */
static inline void spimem_flash_ll_auto_suspend_init(spi_mem_dev_t *dev, bool auto_sus)
{
    dev->flash_sus_ctrl.flash_pes_en = auto_sus;
}

/**
 * Initialize auto resume mode
 *
 * @param dev Beginning address of the peripheral registers.
 * @param auto_res Enable/Disable Flash Auto-Resume.
 *
 */
static inline void spimem_flash_ll_auto_resume_init(spi_mem_dev_t *dev, bool auto_res)
{
    dev->flash_sus_ctrl.pes_per_en = auto_res;
}

/**
 * Setup the flash suspend command, may vary from chips to chips.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param sus_cmd Flash suspend command.
 *
 */
static inline void spimem_flash_ll_suspend_cmd_setup(spi_mem_dev_t *dev, uint32_t sus_cmd)
{
    dev->flash_sus_cmd.flash_pes_command = sus_cmd;
}

/**
 * Setup the flash resume command, may vary from chips to chips.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param res_cmd Flash resume command.
 *
 */
static inline void spimem_flash_ll_resume_cmd_setup(spi_mem_dev_t *dev, uint32_t res_cmd)
{
    dev->flash_sus_cmd.flash_per_command = res_cmd;
}

/**
 * Setup the flash read suspend status command, may vary from chips to chips.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param pesr_cmd Flash read suspend status command.
 *
 */
static inline void spimem_flash_ll_rd_sus_cmd_setup(spi_mem_dev_t *dev, uint32_t pesr_cmd)
{
    dev->flash_sus_cmd.wait_pesr_command = pesr_cmd;
}

/**
 * Setup to check SUS/SUS1/SUS2 to ensure the suspend status of flashs.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param sus_check_sus_en 1: enable, 0: disable.
 *
 */
static inline void spimem_flash_ll_sus_check_sus_setup(spi_mem_dev_t *dev, bool sus_check_sus_en)
{
    dev->flash_sus_ctrl.sus_timeout_cnt = 5;
    dev->flash_sus_ctrl.pes_end_en = sus_check_sus_en;
}

/**
 * Setup to check SUS/SUS1/SUS2 to ensure the resume status of flashs.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param sus_check_sus_en 1: enable, 0: disable.
 *
 */
static inline void spimem_flash_ll_res_check_sus_setup(spi_mem_dev_t *dev, bool res_check_sus_en)
{
    dev->flash_sus_ctrl.sus_timeout_cnt = 5;
    dev->flash_sus_ctrl.per_end_en = res_check_sus_en;
}

/**
 * Set 8 bit command to read suspend status
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_set_read_sus_status(spi_mem_dev_t *dev, uint32_t sus_conf)
{
    dev->flash_sus_ctrl.frd_sus_2b = 0;
    dev->flash_sus_ctrl.pesr_end_msk = sus_conf;
}

/**
 * Configure the delay after Suspend/Resume
 *
 * @param dev Beginning address of the peripheral registers.
 * @param dly_val delay time
 */
static inline void spimem_flash_ll_set_sus_delay(spi_mem_dev_t *dev, uint32_t dly_val)
{
    dev->ctrl1.cs_hold_dly_res = dly_val;
    dev->sus_status.flash_pes_dly_128 = 1;
    dev->sus_status.flash_per_dly_128 = 1;
}

/**
 * Configure the cs hold delay time(used to set the minimum CS high time tSHSL)
 *
 * @param dev Beginning address of the peripheral registers.
 * @param cs_hold_delay cs hold delay time
 */
static inline void spimem_flash_set_cs_hold_delay(spi_mem_dev_t *dev, uint32_t cs_hold_delay)
{
    SPIMEM0.ctrl2.cs_hold_delay = cs_hold_delay;
}

/**
 * Initialize auto wait idle mode
 *
 * @param dev Beginning address of the peripheral registers.
 * @param auto_waiti Enable/disable auto wait-idle function
 */
static inline void spimem_flash_ll_auto_wait_idle_init(spi_mem_dev_t *dev, bool auto_waiti)
{
    dev->flash_waiti_ctrl.waiti_cmd = 0x05;
    dev->flash_sus_ctrl.flash_per_wait_en = auto_waiti;
    dev->flash_sus_ctrl.flash_pes_wait_en = auto_waiti;
}

/**
 * This function is used to set dummy phase when auto suspend is enabled.
 *
 * @note This function is only used when timing tuning is enabled.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param extra_dummy extra dummy length. Get from timing tuning.
 */
static inline void spimem_flash_ll_set_wait_idle_dummy_phase(spi_mem_dev_t *dev, uint32_t extra_dummy)
{
    // Not supported on this chip.
}

/**
 * Return the suspend status of erase or program operations.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 * @return true if suspended, otherwise false.
 */
static inline bool spimem_flash_ll_sus_status(spi_mem_dev_t *dev)
{
    return dev->sus_status.flash_sus;
}

/**
 * @brief Set lock for SPI0 so that spi0 can request new cache request after a cache transfer.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param lock_time Lock delay time
 */
static inline void spimem_flash_ll_sus_set_spi0_lock_trans(spi_mem_dev_t *dev, uint32_t lock_time)
{
    dev->sus_status.spi0_lock_en = 1;
    SPIMEM0.fsm.cspi_lock_delay_time = lock_time;
}

/**
 * @brief Get tsus unit values in SPI_CLK cycles
 *
 * @param dev Beginning address of the peripheral registers.
 * @return uint32_t tsus unit values
 */
static inline uint32_t spimem_flash_ll_get_tsus_unit_in_cycles(spi_mem_dev_t *dev)
{
    uint32_t tsus_unit = 0;
    if (dev->sus_status.flash_pes_dly_128 == 1) {
        tsus_unit = 128;
    } else {
        tsus_unit = 4;
    }
    return tsus_unit;
}

/**
 * Enable/disable write protection for the flash chip.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param wp true to enable the protection, false to disable (write enable).
 */
static inline void spimem_flash_ll_set_write_protect(spi_mem_dev_t *dev, bool wp)
{
    if (wp) {
        dev->cmd.flash_wrdi = 1;
    } else {
        dev->cmd.flash_wren = 1;
    }
}

/**
 * Get the read data from the buffer after ``spimem_flash_ll_read`` is done.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer to hold the output data
 * @param read_len Length to get out of the buffer
 */
__attribute__((always_inline))
static inline void spimem_flash_ll_get_buffer_data(spi_mem_dev_t *dev, void *buffer, uint32_t read_len)
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
 * Set the data to be written in the data buffer.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer holding the data
 * @param length Length of data in bytes.
 */
__attribute__((always_inline))
static inline void spimem_flash_ll_set_buffer_data(spi_mem_dev_t *dev, const void *buffer, uint32_t length)
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
 * Program a page of the flash chip. Call ``spimem_flash_ll_set_address`` before
 * this to set the address to program.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param buffer Buffer holding the data to program
 * @param length Length to program.
 */
static inline void spimem_flash_ll_program_page(spi_mem_dev_t *dev, const void *buffer, uint32_t length)
{
    dev->user.usr_dummy = 0;
    spimem_flash_ll_set_buffer_data(dev, buffer, length);
    dev->cmd.flash_pp = 1;
}

/**
 * Trigger a user defined transaction. All phases, including command, address, dummy, and the data phases,
 * should be configured before this is called.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param pe_ops Is page program/erase operation or not.
 */
static inline void spimem_flash_ll_user_start(spi_mem_dev_t *dev, bool pe_ops)
{
    uint32_t usr_pe = (pe_ops ? 0x60000 : 0x40000);
    dev->cmd.val |= usr_pe;
}

/**
 * Check whether the host is idle to perform new commands.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 * @return true if the host is idle, otherwise false
 */
static inline bool spimem_flash_ll_host_idle(const spi_mem_dev_t *dev)
{
    return dev->cmd.mst_st == 0;
}

/**
 * Set phases for user-defined transaction to read
 *
 * @param dev Beginning address of the peripheral registers.
 */
static inline void spimem_flash_ll_read_phase(spi_mem_dev_t *dev)
{
    typeof (dev->user) user = {
        .usr_mosi = 0,
        .usr_miso = 1,
        .usr_addr = 1,
        .usr_command = 1,
    };
    dev->user.val = user.val;
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
static inline void spimem_flash_ll_set_cs_pin(spi_mem_dev_t *dev, int pin)
{
    dev->misc.cs0_dis = (pin == 0) ? 0 : 1;
    dev->misc.cs1_dis = (pin == 1) ? 0 : 1;
}

/**
 * Set the read io mode.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param read_mode I/O mode to use in the following transactions.
 */
static inline void spimem_flash_ll_set_read_mode(spi_mem_dev_t *dev, esp_flash_io_mode_t read_mode)
{
    typeof (dev->ctrl) ctrl;
    ctrl.val = dev->ctrl.val;

    ctrl.val &= ~(SPI_MEM_FREAD_QIO_M | SPI_MEM_FREAD_QUAD_M | SPI_MEM_FREAD_DIO_M | SPI_MEM_FREAD_DUAL_M);
    ctrl.val |= SPI_MEM_FASTRD_MODE_M;
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
    dev->ctrl.val = ctrl.val;
}

/**
 * Set clock frequency to work at.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param clock_val pointer to the clock value to set
 */
static inline void spimem_flash_ll_set_clock(spi_mem_dev_t *dev, spimem_flash_ll_clock_reg_t *clock_val)
{
    dev->clock.val = *clock_val;
}

/**
 * Set the input length, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of input, in bits.
 */
static inline void spimem_flash_ll_set_miso_bitlen(spi_mem_dev_t *dev, uint32_t bitlen)
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
__attribute__((always_inline))
static inline void spimem_flash_ll_set_mosi_bitlen(spi_mem_dev_t *dev, uint32_t bitlen)
{
    dev->user.usr_mosi = bitlen > 0;
    dev->mosi_dlen.usr_mosi_bit_len = bitlen ? (bitlen - 1) : 0;
}

/**
 * Set the command.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param command Command to send
 * @param bitlen Length of the command
 */
static inline void spimem_flash_ll_set_command(spi_mem_dev_t *dev, uint32_t command, uint32_t bitlen)
{
    dev->user.usr_command = 1;
    typeof(dev->user2) user2 = {
        .usr_command_value = command,
        .usr_command_bitlen = (bitlen - 1),
    };
    dev->user2.val = user2.val;
}

/**
 * Get the address length that is set in register, in bits.
 *
 * @param dev Beginning address of the peripheral registers.
 *
 */
static inline int spimem_flash_ll_get_addr_bitlen(spi_mem_dev_t *dev)
{
    return dev->user.usr_addr ? dev->user1.usr_addr_bitlen + 1 : 0;
}

/**
 * Set the address length to send, in bits. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param bitlen Length of the address, in bits
 */
__attribute__((always_inline))
static inline void spimem_flash_ll_set_addr_bitlen(spi_mem_dev_t *dev, uint32_t bitlen)
{
    dev->user1.usr_addr_bitlen = (bitlen - 1);
    dev->user.usr_addr = bitlen ? 1 : 0;
}

/**
 * Set the address to send. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
static inline void spimem_flash_ll_set_address(spi_mem_dev_t *dev, uint32_t addr)
{
    dev->addr = addr;
}

/**
 * Set the address to send in user mode. Should be called before commands that requires the address e.g. erase sector, read, write...
 *
 * @param dev Beginning address of the peripheral registers.
 * @param addr Address to send
 */
__attribute__((always_inline))
static inline void spimem_flash_ll_set_usr_address(spi_mem_dev_t *dev, uint32_t addr, uint32_t bitlen)
{
    (void)bitlen;
    spimem_flash_ll_set_address(dev, addr);
}

/**
 * Set the length of dummy cycles.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param dummy_n Cycles of dummy phases
 */
static inline void spimem_flash_ll_set_dummy(spi_mem_dev_t *dev, uint32_t dummy_n)
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
static inline void spimem_flash_ll_set_dummy_out(spi_mem_dev_t *dev, uint32_t out_en, uint32_t out_lev)
{
    dev->ctrl.fdummy_out = out_en;
    dev->ctrl.q_pol = out_lev;
    dev->ctrl.d_pol = out_lev;
}

/**
 * Set CS hold time.
 *
 * @param dev Beginning address of the peripheral registers.
 * @param hold_n CS hold time config used by the host.
 */
static inline void spimem_flash_ll_set_hold(spi_mem_dev_t *dev, uint32_t hold_n)
{
    dev->ctrl2.cs_hold_time = hold_n - 1;
    dev->user.cs_hold = (hold_n > 0? 1: 0);
}

static inline void spimem_flash_ll_set_cs_setup(spi_mem_dev_t *dev, uint32_t cs_setup_time)
{
    dev->user.cs_setup = (cs_setup_time > 0 ? 1 : 0);
    dev->ctrl2.cs_setup_time = cs_setup_time - 1;
}

/**
 * Get the spi flash source clock frequency. Used for calculating
 * the divider parameters.
 *
 * @param None
 *
 * @return the frequency of spi flash clock source.(MHz)
 */
static inline uint8_t spimem_flash_ll_get_source_freq_mhz(void)
{
    // Default is PLL120M, this is hard-coded.
    // In the future, we can get the CPU clock source by calling interface.
    // When PLL120M is selected, mspi clock is 60MHz.
    return SPIMEM_FLASH_LL_PERIPHERAL_FREQUENCY_MHZ;
}

/**
 * Calculate spi_flash clock frequency division parameters for register.
 *
 * @param clkdiv frequency division factor
 *
 * @return Register setting for the given clock division factor.
 */
static inline uint32_t spimem_flash_ll_calculate_clock_reg(uint8_t clkdiv)
{
    uint32_t div_parameter;
    // See comments of `clock` in `spi_mem_struct.h`
    if (clkdiv == 1) {
        div_parameter = (1 << 31);
    } else {
        div_parameter = ((clkdiv - 1) | (((clkdiv - 1) / 2 & 0xff) << 8 ) | (((clkdiv - 1) & 0xff) << 16));
    }
    return div_parameter;
}

/**
 * @brief Write protect signal output when SPI is idle

 * @param level 1: 1: output high, 0: output low
 */
static inline void spimem_flash_ll_set_wp_level(spi_mem_dev_t *dev, bool level)
{
    dev->ctrl.wp = level;
}

/**
 * @brief Get the ctrl value of mspi
 *
 * @return uint32_t The value of ctrl register
 */
static inline uint32_t spimem_flash_ll_get_ctrl_val(spi_mem_dev_t *dev)
{
    return dev->ctrl.val;
}

/**
 * @brief Reset whole memory spi
 */
static inline void spimem_flash_ll_sync_reset(void)
{
    SPIMEM1.ctrl2.sync_reset = 0;
    SPIMEM0.ctrl2.sync_reset = 0;
    SPIMEM1.ctrl2.sync_reset = 1;
    SPIMEM0.ctrl2.sync_reset = 1;
    SPIMEM1.ctrl2.sync_reset = 0;
    SPIMEM0.ctrl2.sync_reset = 0;
}

/**
 * @brief Get common command related registers
 *
 * @param ctrl_reg ctrl_reg
 * @param user_reg user_reg
 * @param user1_reg user1_reg
 * @param user2_reg user2_reg
 */
static inline void spimem_flash_ll_get_common_command_register_info(spi_mem_dev_t *dev, uint32_t *ctrl_reg, uint32_t *user_reg, uint32_t *user1_reg, uint32_t *user2_reg)
{
    *ctrl_reg = dev->ctrl.val;
    *user_reg = dev->user.val;
    *user1_reg = dev->user1.val;
    *user2_reg = dev->user2.val;
}

/**
 * @brief Set common command related registers
 *
 * @param ctrl_reg ctrl_reg
 * @param user_reg user_reg
 * @param user1_reg user1_reg
 * @param user2_reg user2_reg
 */
static inline void spimem_flash_ll_set_common_command_register_info(spi_mem_dev_t *dev, uint32_t ctrl_reg, uint32_t user_reg, uint32_t user1_reg, uint32_t user2_reg)
{
    dev->ctrl.val = ctrl_reg;
    dev->user.val = user_reg;
    dev->user1.val = user1_reg;
    dev->user2.val = user2_reg;
}

#ifdef __cplusplus
}
#endif
