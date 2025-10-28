/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
#include "rom/spi_flash.h"
#include "hal/psram_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_CTRLR_LL_MSPI_ID_0            0
#define PSRAM_CTRLR_LL_MSPI_ID_1            1

#define PSRAM_LL_CS_SEL                     SPI_MEM_CS1_DIS_M
#define PSRAM_CTRLR_LL_PMS_REGION_NUMS      4
#define PSRAM_CTRLR_LL_PMS_ATTR_WRITABLE    (1<<0)
#define PSRAM_CTRLR_LL_PMS_ATTR_READABLE    (1<<1)

/**
 * @brief PSRAM enum for cs id.
 */
typedef enum {
    PSRAM_LL_CS_ID_0 = 0,
    PSRAM_LL_CS_ID_1 = 1,
} psram_ll_cs_id_t;

/**
 * @brief PSRAM ECC mode
 */
typedef enum {
    PSRAM_LL_ECC_MODE_16TO17 = 0,
    PSRAM_LL_ECC_MODE_16TO18 = 1,
} psram_ll_ecc_mode_t;

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
    SPIMEM0.mem_cache_sctrl.mem_cache_sram_usr_wcmd = 1;
    SPIMEM0.mem_sram_dwr_cmd.mem_cache_sram_usr_wr_cmd_bitlen = cmd_bitlen - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SPIMEM0.mem_sram_dwr_cmd, mem_cache_sram_usr_wr_cmd_value, cmd_val);
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
    SPIMEM0.mem_cache_sctrl.mem_cache_sram_usr_rcmd = 1;
    SPIMEM0.mem_sram_drd_cmd.mem_cache_sram_usr_rd_cmd_bitlen = cmd_bitlen - 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(SPIMEM0.mem_sram_drd_cmd, mem_cache_sram_usr_rd_cmd_value, cmd_val);
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
    SPIMEM0.mem_cache_sctrl.mem_sram_addr_bitlen = addr_bitlen - 1;
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
    SPIMEM0.mem_cache_sctrl.mem_usr_rd_sram_dummy = 1;
    SPIMEM0.mem_cache_sctrl.mem_sram_rdummy_cyclelen = dummy_n - 1;
}

/**
 * @brief Set PSRAM bus clock
 *
 * @param mspi_id     mspi_id
 * @param clock_conf  Configuration value for psram clock
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_bus_clock(uint32_t mspi_id, uint32_t clock_conf)
{
    SPIMEM0.mem_sram_clk.val = clock_conf;
}

/**
 * @brief Set SPI1 bus clock to initialise PSRAM
 *
 * @param mspi_id     mspi_id
 * @param clock_conf  Configuration value for psram clock
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_spi1_bus_clock(uint32_t mspi_id, uint32_t clock_conf)
{
    HAL_ASSERT(mspi_id == PSRAM_CTRLR_LL_MSPI_ID_1);
    SPIMEM1.clock.val = clock_conf;
}

/**
 * Calculate spi_flash clock frequency division parameters for register.
 *
 * @param clkdiv frequency division factor
 *
 * @return Register setting for the given clock division factor.
 */
static inline uint32_t psram_ctrlr_ll_calculate_clock_reg(uint8_t clkdiv)
{
    uint32_t div_parameter;
    // See comments of `clock` in `spi_mem_struct.h`
    if (clkdiv == 1) {
        div_parameter = (1 << 31);
    } else {
        div_parameter = ((clkdiv - 1) | (((clkdiv - 1) / 2 & 0xff) << 8) | (((clkdiv - 1) & 0xff) << 16));
    }
    return div_parameter;
}

/**
 * Configure the psram read mode
 *
 * @param mspi_id      mspi_id
 * @param read_mode    read mode
 */
static inline void psram_ctrlr_ll_set_read_mode(uint32_t mspi_id, psram_cmd_mode_t read_mode)
{
    typeof(SPIMEM0.mem_cache_sctrl) mem_cache_sctrl;
    mem_cache_sctrl.val = SPIMEM0.mem_cache_sctrl.val;

    mem_cache_sctrl.val &= ~(SPI_MEM_USR_SRAM_DIO_M | SPI_MEM_USR_SRAM_QIO_M);
    switch (read_mode) {
    case PSRAM_HAL_CMD_SPI:
        mem_cache_sctrl.mem_usr_sram_dio = 1;
        break;
    case PSRAM_HAL_CMD_QPI:
        mem_cache_sctrl.mem_usr_sram_qio = 1;
        break;
    default:
        abort();
    }
    SPIMEM0.mem_cache_sctrl.val = mem_cache_sctrl.val;
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
    SPIMEM0.smem_ac.smem_cs_setup = 1;
    SPIMEM0.smem_ac.smem_cs_setup_time = setup_n - 1;
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
    SPIMEM0.smem_ac.smem_cs_hold = 1;
    SPIMEM0.smem_ac.smem_cs_hold_time = hold_n - 1;
}

/**
 * @brief Set CS hold delay
 *
 * @param mspi_id        mspi_id
 * @param hold_delay_n   cs hold delay time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_hold_delay(uint32_t mspi_id, uint32_t hold_delay_n)
{
    (void)mspi_id;
    HAL_ASSERT(hold_delay_n > 0);
    SPIMEM0.smem_ac.smem_cs_hold_delay = hold_delay_n - 1;
}

/**
 * @brief PSRAM common transaction
 *
 * See `spi_flash.h` for parameters
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
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_pin(uint32_t mspi_id, psram_ll_cs_id_t cs_id)
{
    SPIMEM1.misc.cs0_dis = (cs_id == PSRAM_LL_CS_ID_0) ? 0 : 1;
    SPIMEM1.misc.cs1_dis = (cs_id == PSRAM_LL_CS_ID_1) ? 0 : 1;
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

/*---------------------------------------------------------------
                    ECC
---------------------------------------------------------------*/
/**
 * @brief Set ECC CS hold
 *
 * @param mspi_id      mspi_id
 * @param hold_n       cs hold time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_ecc_cs_hold(uint32_t mspi_id, uint32_t hold_n)
{
    HAL_ASSERT(hold_n > 0);
    SPIMEM0.smem_ac.smem_ecc_cs_hold_time = hold_n - 1;
}

/**
 * @brief Set ECC mode
 *
 * @param mspi_id      mspi_id
 * @param mode         ecc mode
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_ecc_mode(uint32_t mspi_id, psram_ll_ecc_mode_t mode)
{
    SPIMEM0.smem_ac.smem_ecc_16to18_byte_en = mode;
}

/**
 * @brief Set page size
 *
 * @param mspi_id      mspi_id
 * @param size         page size
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_page_size(uint32_t mspi_id, uint32_t size)
{
    uint32_t size_val = 0;
    switch (size) {
    case 256:
        size_val = 0;
        break;
    case 512:
        size_val = 1;
        break;
    case 1024:
        size_val = 2;
        break;
    case 2048:
        size_val = 3;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    SPIMEM0.smem_ecc_ctrl.smem_page_size = size_val;
}

/**
 * @brief Get page size
 *
 * @param mspi_id      mspi_id
 *
 * @return             page size
 */
__attribute__((always_inline))
static inline uint32_t psram_ctrlr_ll_get_page_size(uint32_t mspi_id)
{
    (void)mspi_id;
    uint32_t page_size = 0;

    uint32_t reg_val = SPIMEM0.smem_ecc_ctrl.smem_page_size;
    switch (reg_val) {
    case 0:
        page_size = 256;
        break;
    case 1:
        page_size = 512;
        break;
    case 2:
        page_size = 1024;
        break;
    case 3:
        page_size = 2048;
        break;
    default:
        HAL_ASSERT(false);
    }

    return page_size;
}

/**
 * @brief Skip page corner
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_skip_page_corner(uint32_t mspi_id, bool en)
{
    SPIMEM0.smem_ac.smem_ecc_skip_page_corner = en;
}

/**
 * @brief Enable splitting transactions
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_split_trans(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM0.smem_ac.smem_split_trans_en = en;
}

/**
 * @brief Enable ECC address conversion
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_ecc_addr_conversion(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM0.smem_ecc_ctrl.smem_ecc_addr_en = en;
}

/*---------------------------------------------------------------
                    PMS
---------------------------------------------------------------*/
/**
 * @brief Enable PMS ECC
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_pms_region_ecc(uint32_t mspi_id, uint32_t region_id, bool en)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    SPIMEM0.smem_pmsn_attr[region_id].smem_pmsn_ecc = en;
}

/**
 * @brief Set PMS attr
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 * @param attr_mask    attribute mask
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_pms_region_attr(uint32_t mspi_id, uint32_t region_id, uint32_t attr_mask)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    SPIMEM0.smem_pmsn_attr[region_id].smem_pmsn_wr_attr = 0;
    SPIMEM0.smem_pmsn_attr[region_id].smem_pmsn_rd_attr = 0;
    if (attr_mask & PSRAM_CTRLR_LL_PMS_ATTR_WRITABLE) {
        SPIMEM0.smem_pmsn_attr[region_id].smem_pmsn_wr_attr = 1;
    }
    if (attr_mask & PSRAM_CTRLR_LL_PMS_ATTR_READABLE) {
        SPIMEM0.smem_pmsn_attr[region_id].smem_pmsn_rd_attr = 1;
    }
}

/**
 * @brief Set PMS address
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 * @param addr         start addr
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_pms_region_start_addr(uint32_t mspi_id, uint32_t region_id, uint32_t addr)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    SPIMEM0.smem_pmsn_addr[region_id].smem_pmsn_addr_s = addr;
}

/**
 * @brief Set PMS size
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 * @param size         size
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_pms_region_size(uint32_t mspi_id, uint32_t region_id, uint32_t size)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    SPIMEM0.smem_pmsn_size[region_id].smem_pmsn_size = size;
}

/**
 * @brief Get PMS address
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 */
__attribute__((always_inline))
static inline uint32_t psram_ctrlr_ll_get_pms_region_start_addr(uint32_t mspi_id, uint32_t region_id)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    return SPIMEM0.smem_pmsn_addr[region_id].smem_pmsn_addr_s;
}

/**
 * @brief Get PMS size
 *
 * @param mspi_id      mspi_id
 * @param region_id    region_id
 */
__attribute__((always_inline))
static inline uint32_t psram_ctrlr_ll_get_pms_region_size(uint32_t mspi_id, uint32_t region_id)
{
    (void)mspi_id;
    HAL_ASSERT(region_id < PSRAM_CTRLR_LL_PMS_REGION_NUMS);
    return SPIMEM0.smem_pmsn_size[region_id].smem_pmsn_size;
}

#ifdef __cplusplus
}
#endif
