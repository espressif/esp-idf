/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <sys/param.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_reg.h"
#include "soc/clk_tree_defs.h"
#include "rom/opi_flash.h"
#include "hal/psram_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_CTRLR_LL_MSPI_ID_0            0
#define PSRAM_CTRLR_LL_MSPI_ID_1            1

#define PSRAM_LL_CS_SEL   SPI_MEM_CS1_DIS_M

/**
 * @brief PSRAM enum for cs id.
 */
typedef enum {
    PSRAM_LL_CS_ID_0 = 0,
    PSRAM_LL_CS_ID_1 = 1,
} psram_ll_cs_id_t;

/**
 * @brief Set PSRAM write cmd
 *
 * @param mspi_id     mspi_id
 * @param cmd_bitlen  command bitlen
 * @param cmd_val     command value
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_wr_cmd(uint32_t mspi_id, uint32_t cmd_bitlen, uint32_t cmd_val)
{
    (void)mspi_id;
    HAL_ASSERT(cmd_bitlen > 0);
    SPIMEM0.cache_sctrl.usr_wcmd = 1;
    SPIMEM0.sram_dwr_cmd.usr_wr_cmd_bitlen = cmd_bitlen - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SPIMEM0.sram_dwr_cmd, usr_wr_cmd_value, cmd_val);
}

/**
 * @brief Set PSRAM read cmd
 *
 * @param mspi_id     mspi_id
 * @param cmd_bitlen  command bitlen
 * @param cmd_val     command value
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_rd_cmd(uint32_t mspi_id, uint32_t cmd_bitlen, uint32_t cmd_val)
{
    (void)mspi_id;
    HAL_ASSERT(cmd_bitlen > 0);
    SPIMEM0.cache_sctrl.usr_rcmd = 1;
    SPIMEM0.sram_drd_cmd.usr_rd_cmd_bitlen = cmd_bitlen - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SPIMEM0.sram_drd_cmd, usr_rd_cmd_value, cmd_val);
}

/**
 * @brief Set PSRAM addr bitlen
 *
 * @param mspi_id      mspi_id
 * @param addr_bitlen  address bitlen
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_addr_bitlen(uint32_t mspi_id, uint32_t addr_bitlen)
{
    (void)mspi_id;
    HAL_ASSERT(addr_bitlen > 0);
    SPIMEM0.cache_sctrl.sram_addr_bitlen = addr_bitlen - 1;
}

/**
 * @brief Set PSRAM read dummy
 *
 * @param mspi_id      mspi_id
 * @param dummy_n      dummy number
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_rd_dummy(uint32_t mspi_id, uint32_t dummy_n)
{
    (void)mspi_id;
    HAL_ASSERT(dummy_n > 0);
    SPIMEM0.cache_sctrl.usr_rd_sram_dummy = 1;
    SPIMEM0.cache_sctrl.sram_rdummy_cyclelen = dummy_n - 1;
}

/**
 * Configure the psram read mode
 *
 * @param mspi_id      mspi_id
 * @param read_mode    read mode
 */
static inline void psram_ctrlr_ll_set_read_mode(uint32_t mspi_id, psram_hal_cmd_mode_t read_mode)
{
    typeof (SPIMEM0.cache_sctrl) cache_sctrl;
    cache_sctrl.val = SPIMEM0.cache_sctrl.val;

    cache_sctrl.val &= ~(SPI_MEM_USR_SRAM_DIO_M | SPI_MEM_USR_SRAM_QIO_M);
    switch (read_mode) {
    case PSRAM_HAL_CMD_SPI:
        cache_sctrl.usr_sram_dio = 1;
        break;
    case PSRAM_HAL_CMD_QPI:
        cache_sctrl.usr_sram_qio = 1;
        break;
    default:
        abort();
    }
    SPIMEM0.cache_sctrl.val = cache_sctrl.val;
}

/**
 * @brief Set CS setup
 *
 * @param mspi_id      mspi_id
 * @param setup_n       cs setup time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_setup(uint32_t mspi_id, uint32_t setup_n)
{
    (void)mspi_id;
    HAL_ASSERT(setup_n > 0);
    SPIMEM0.spi_smem_ac.smem_cs_setup = 1;
    SPIMEM0.spi_smem_ac.smem_cs_setup_time = setup_n - 1;
}

/**
 * @brief Set CS hold
 *
 * @param mspi_id      mspi_id
 * @param hold_n       cs hold time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_hold(uint32_t mspi_id, uint32_t hold_n)
{
    (void)mspi_id;
    HAL_ASSERT(hold_n > 0);
    SPIMEM0.spi_smem_ac.smem_cs_hold = 1;
    SPIMEM0.spi_smem_ac.smem_cs_hold_time = hold_n - 1;
}

/**
 * @brief Set CS hold delay
 *
 * @param mspi_id         mspi_id
 * @param hold_delay_n    cs hold delay time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_hold_delay(uint32_t mspi_id, uint32_t hold_delay_n)
{
    (void)mspi_id;
    HAL_ASSERT(hold_delay_n > 0);
    SPIMEM0.spi_smem_ac.smem_cs_hold_delay = hold_delay_n - 1;
}

/**
 * @brief PSRAM common transaction
 *
 * See `opi_flash.h` for parameters
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_common_transaction_base(uint32_t mspi_id, esp_rom_spiflash_read_mode_t mode,
                                                          uint32_t cmd, uint32_t cmd_bitlen,
                                                          uint32_t addr, uint32_t addr_bitlen,
                                                          uint32_t dummy_bits,
                                                          uint8_t* mosi_data, uint32_t mosi_bitlen,
                                                          uint8_t* miso_data, uint32_t miso_bitlen,
                                                          uint32_t cs_mask,
                                                          bool is_write_erase_operation)
{
    esp_rom_spi_cmd_t conf = {
        .cmd = cmd,
        .cmdBitLen = cmd_bitlen,
        .addr = &addr,
        .addrBitLen = addr_bitlen,
        .txData = (uint32_t *)mosi_data,
        .txDataBitLen = mosi_bitlen,
        .rxData = (uint32_t *)miso_data,
        .rxDataBitLen = miso_bitlen,
        .dummyBitLen = dummy_bits,
    };
    esp_rom_spi_cmd_config(mspi_id, &conf);
    esp_rom_spi_cmd_start(mspi_id, miso_data, miso_bitlen / 8, cs_mask, is_write_erase_operation);
}

/**
 * Select which pin to use for the psram
 *
 * @param mspi_id mspi_id
 * @param cs_id cs_id for psram to use.
 */
static inline void psram_ctrlr_ll_set_cs_pin(uint32_t mspi_id, psram_ll_cs_id_t cs_id)
{
    SPIMEM0.misc.cs0_dis = (cs_id == PSRAM_LL_CS_ID_0) ? 0 : 1;
    SPIMEM0.misc.cs1_dis = (cs_id == PSRAM_LL_CS_ID_1) ? 0 : 1;
}

/**
 * Enable the psram quad command
 *
 * @param mspi_id    mspi_id
 * @param ena        true if enable, otherwise false
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_quad_command(uint32_t mspi_id, bool ena)
{
    SPIMEM1.ctrl.fcmd_quad = ena;
}

#ifdef __cplusplus
}
#endif
