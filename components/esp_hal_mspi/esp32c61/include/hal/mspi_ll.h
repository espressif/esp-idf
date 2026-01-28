/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * Background
 *
 * This file is for the MSPI related, but not Flash driver related registers, these registers:
 * - may influence both Flash and PSRAM
 * - not related or directly related to Flash controller driver
 *
 * Some hints for naming convention:
 * - For MSPI timing tuning related registers, the LL should start with `mspi_timing_ll_`
 * - For others, the LL should start with `mspi_ll_`
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"
#include "soc/pcr_struct.h"
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_reg.h"
#include "hal/misc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PERIPH_NUM                            2
#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1
#define MSPI_LL_CORE_CLOCK_80_MHZ                     80
#define MSPI_LL_CORE_CLOCK_120_MHZ                    120
#define MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT         MSPI_LL_CORE_CLOCK_80_MHZ

// PSRAM frequency should be constrained by AXI frequency to avoid FIFO underflow.
#define MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED     1

#define MSPI_LL_EVENT_SLV_ST_END                      (1<<3)
#define MSPI_LL_EVENT_MST_ST_END                      (1<<4)
#define MSPI_LL_EVENT_ECC_ERR                         (1<<5)
#define MSPI_LL_EVENT_PMS_REJECT                      (1<<6)
#define MSPI_LL_EVENT_AXI_RADDR_ERR                   (1<<7)
#define MSPI_LL_EVENT_AXI_WR_FLASH_ERR                (1<<8)
#define MSPI_LL_EVENT_AXI_WADDR_ERR                   (1<<9)
#define MSPI_LL_EVENT_MASK                            (MSPI_LL_EVENT_ECC_ERR | MSPI_LL_EVENT_PMS_REJECT | MSPI_LL_EVENT_AXI_RADDR_ERR | \
                                                       MSPI_LL_EVENT_AXI_WR_FLASH_ERR | MSPI_LL_EVENT_AXI_WADDR_ERR)

#define MSPI_LL_ADDR_INT_SUPPORTED                    1
#define MSPI_LL_PMS_INT_SUPPORTED                     1
#define MSPI_LL_INTR_EVENT_SUPPORTED                  1
#define MSPI_LL_INTR_SHARED                           1

/************************** MSPI pll clock configurations **************************/

/*
 * @brief Select FLASH clock source
 *
 * @param mspi_id      mspi_id
 * @param clk_src      clock source, see valid sources in type `soc_periph_flash_clk_src_t`
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_set_flash_clk_src(uint32_t mspi_id, soc_periph_flash_clk_src_t clk_src)
{
    HAL_ASSERT(mspi_id == 0);
    switch (clk_src) {
    case FLASH_CLK_SRC_XTAL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 0;
        break;
    case FLASH_CLK_SRC_RC_FAST:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 1;
        break;
    case FLASH_CLK_SRC_SPLL:
        PCR.mspi_clk_conf.mspi_func_clk_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set MSPI_FAST_CLK's high-speed divider (valid when SOC_ROOT clock source is PLL)
 *
 * @param mspi_id       SPI0 / SPI1
 * @param core_clk_mhz  core clock mhz
 */
static inline __attribute__((always_inline)) void mspi_timing_ll_set_core_clock(uint8_t mspi_id, uint32_t core_clk_mhz)
{
    HAL_ASSERT(mspi_id == 0);
    uint32_t divider = 0;
    switch (core_clk_mhz) {
    case 80:
        divider = 6;
        break;
    case 120:
        divider = 4;
        break;
    default:
        HAL_ASSERT(false);
    }

    HAL_FORCE_MODIFY_U32_REG_FIELD(PCR.mspi_clk_conf, mspi_fast_div_num, divider - 1);
}

/**
 * @brief Enable the mspi core clock
 *
 * @param mspi_id       SPI0 / SPI1
 * @param enable        enable the core clock
 */
static inline __attribute__((always_inline)) void mspi_timing_ll_enable_core_clock(uint8_t mspi_id, bool enable)
{
    PCR.mspi_conf.mspi_clk_en = enable;
}

/**
 * Reset the MSPI clock
 */
static inline __attribute__((always_inline)) void _mspi_timing_ll_reset_mspi(void)
{
    PCR.mspi_clk_conf.mspi_axi_rst_en = 1;
    PCR.mspi_clk_conf.mspi_axi_rst_en = 0;
    // Wait for mspi to be ready
    while (!PCR.mspi_conf.mspi_ready) {
        ;
    };
}

/**
 * Calculate spi_flash clock frequency division parameters for register.
 *
 * @param clkdiv frequency division factor
 *
 * @return Register setting for the given clock division factor.
 */
static inline uint32_t mspi_timing_ll_calculate_clock_reg(uint8_t clkdiv)
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
 * Clear MSPI hw fifo
 *
 * @param mspi_id       SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_clear_fifo(uint8_t mspi_id)
{
    for (int i = 0; i < 16; i++) {
        REG_WRITE(SPI_MEM_W0_REG(mspi_id) + i * 4, 0);
    }
}

/*---------------------------------------------------------------
                            FLASH
---------------------------------------------------------------*/
/**
 * @brief Set Flash clock
 *
 * @param mspi_id     mspi_id
 * @param clock_conf  Configuration value for flash clock
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_clock(uint32_t mspi_id, uint32_t clock_conf)
{
    if (mspi_id == MSPI_TIMING_LL_MSPI_ID_0) {
        SPIMEM0.mem_clock.val = clock_conf;
    } else if (mspi_id == MSPI_TIMING_LL_MSPI_ID_1) {
        SPIMEM1.clock.val = clock_conf;
    }
}

/**
 * Enable Flash timing adjust clock
 *
 * @param mspi_id  SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timinng_ll_enable_flash_timing_adjust_clk(uint8_t mspi_id)
{
    SPIMEM0.mem_timing_cali.mem_timing_clk_ena = true;
}

/**
 * Set MSPI Flash din mode
 *
 * @param mspi_id   SPI0 / SPI1
 * @param din_mode  Din mode value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_din_mode(uint8_t mspi_id, uint8_t din_mode)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_DIN_MODE_REG(mspi_id)) & (~(SPI_MEM_DIN0_MODE_M | SPI_MEM_DIN1_MODE_M | SPI_MEM_DIN2_MODE_M | SPI_MEM_DIN3_MODE_M | SPI_MEM_DIN4_MODE_M | SPI_MEM_DIN5_MODE_M | SPI_MEM_DIN6_MODE_M | SPI_MEM_DIN7_MODE_M | SPI_MEM_DINS_MODE_M)))
                       | (din_mode << SPI_MEM_DIN0_MODE_S) | (din_mode << SPI_MEM_DIN1_MODE_S) | (din_mode << SPI_MEM_DIN2_MODE_S) | (din_mode << SPI_MEM_DIN3_MODE_S)
                       | (din_mode << SPI_MEM_DIN4_MODE_S) | (din_mode << SPI_MEM_DIN5_MODE_S) | (din_mode << SPI_MEM_DIN6_MODE_S) | (din_mode << SPI_MEM_DIN7_MODE_S) | (din_mode << SPI_MEM_DINS_MODE_S);
    REG_WRITE(SPI_MEM_DIN_MODE_REG(mspi_id), reg_val);
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Flash din num
 *
 * @param mspi_id   SPI0 / SPI1
 * @param din_num   Din num value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_din_num(uint8_t mspi_id, uint8_t din_num)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_DIN_NUM_REG(mspi_id)) & (~(SPI_MEM_DIN0_NUM_M | SPI_MEM_DIN1_NUM_M | SPI_MEM_DIN2_NUM_M | SPI_MEM_DIN3_NUM_M | SPI_MEM_DIN4_NUM_M | SPI_MEM_DIN5_NUM_M | SPI_MEM_DIN6_NUM_M | SPI_MEM_DIN7_NUM_M | SPI_MEM_DINS_NUM_M)))
                       | (din_num << SPI_MEM_DIN0_NUM_S) | (din_num << SPI_MEM_DIN1_NUM_S) | (din_num << SPI_MEM_DIN2_NUM_S) | (din_num << SPI_MEM_DIN3_NUM_S)
                       | (din_num << SPI_MEM_DIN4_NUM_S) | (din_num << SPI_MEM_DIN5_NUM_S) | (din_num << SPI_MEM_DIN6_NUM_S) | (din_num << SPI_MEM_DIN7_NUM_S) | (din_num << SPI_MEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_DIN_NUM_REG(mspi_id), reg_val);
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Flash extra dummy
 *
 * @param mspi_id      SPI0 / SPI1
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_extra_dummy(uint8_t mspi_id, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(mspi_id), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(mspi_id), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy, SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(mspi_id), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(mspi_id), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, 0, SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    }
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Flash user dummy
 *
 * @param mspi_id      SPI0 / SPI1
 * @param user_dummy   user dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_user_dummy(uint8_t mspi_id, uint8_t user_dummy)
{
    REG_SET_FIELD(SPI_MEM_USER1_REG(mspi_id), SPI_MEM_USR_DUMMY_CYCLELEN, user_dummy);
}

/**
 * Get MSPI flash dummy info
 *
 * @param mspi_id      SPI0 / SPI1
 * @param usr_dummy    User dummy
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_get_flash_dummy(uint8_t mspi_id, int *usr_dummy, int *extra_dummy)
{
    *usr_dummy = REG_GET_FIELD(SPI_MEM_USER1_REG(mspi_id), SPI_MEM_USR_DUMMY_CYCLELEN);
    *extra_dummy = REG_GET_FIELD(SPI_MEM_TIMING_CALI_REG(mspi_id), SPI_MEM_EXTRA_DUMMY_CYCLELEN);
}

/**
 * Enable/Disable Flash variable dummy
 *
 * @param mspi_id  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_enable_flash_variable_dummy(uint8_t mspi_id, bool enable)
{
    REG_SET_FIELD(SPI_MEM_DDR_REG(mspi_id), SPI_FMEM_VAR_DUMMY, enable);
}

/**
 * Get if cs setup is enabled or not
 *
 * @param mspi_id    SPI0 / SPI1
 *
 * @return
 *        true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool mspi_timing_ll_is_cs_setup_enabled(uint8_t mspi_id)
{
    return REG_GET_BIT(SPI_MEM_USER_REG(mspi_id), SPI_MEM_CS_SETUP);
}

/**
 * Get cs setup val
 *
 * @param mspi_id    SPI0 / SPI1
 *
 * @return
 *        cs setup reg val
 */
static inline uint32_t mspi_timing_ll_get_cs_setup_val(uint8_t mspi_id)
{
    return REG_GET_FIELD(SPI_MEM_CTRL2_REG(mspi_id), SPI_MEM_CS_SETUP_TIME);
}

/**
 * Get if cs hold is enabled or not
 *
 * @param mspi_id    SPI0 / SPI1
 *
 * @return
 *        true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool mspi_timing_ll_is_cs_hold_enabled(uint8_t mspi_id)
{
    return REG_GET_FIELD(SPI_MEM_USER_REG(mspi_id), SPI_MEM_CS_HOLD);
}

/**
 * Get cs hold val
 *
 * @param mspi_id    SPI0 / SPI1
 *
 * @return
 *        cs hold reg val
 */
static inline uint32_t mspi_timing_ll_get_cs_hold_val(uint8_t mspi_id)
{
    return REG_GET_FIELD(SPI_MEM_CTRL2_REG(mspi_id), SPI_MEM_CS_HOLD_TIME);
}

/**
 * Get clock reg val
 *
 * @param mspi_id    SPI0 / SPI1
 *
 * @return
 *        clock reg val
 */
__attribute__((always_inline))
static inline uint32_t mspi_timing_ll_get_clock_reg(uint8_t mspi_id)
{
    return READ_PERI_REG(SPI_MEM_CLOCK_REG(mspi_id));
}

/*---------------------------------------------------------------
                            PSRAM
---------------------------------------------------------------*/
/**
 * @brief Set PSRAM clock
 *
 * @param mspi_id     mspi_id
 * @param clock_conf  Configuration value for psram clock
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_clock(uint32_t mspi_id, uint32_t clock_conf)
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
static inline void mspi_timing_ll_set_spi1_bus_clock(uint32_t mspi_id, uint32_t clock_conf)
{
    HAL_ASSERT(mspi_id == MSPI_TIMING_LL_MSPI_ID_1);
    SPIMEM1.clock.val = clock_conf;
}

/**
 * Enable PSRAM timing adjust clock
 *
 * @param mspi_id  SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timinng_ll_enable_psram_timing_adjust_clk(uint8_t mspi_id)
{
    SPIMEM0.smem_timing_cali.smem_timing_clk_ena = true;
}

/**
 * Set MSPI PSRAM din mode
 *
 * @param mspi_id   SPI0 / SPI1
 * @param din_mode  Din mode value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_din_mode(uint8_t mspi_id, uint8_t din_mode)
{
    uint32_t reg_val = (REG_READ(SPI_SMEM_DIN_MODE_REG(mspi_id)) & (~(SPI_SMEM_DIN0_MODE_M | SPI_SMEM_DIN1_MODE_M | SPI_SMEM_DIN2_MODE_M | SPI_SMEM_DIN3_MODE_M | SPI_SMEM_DIN4_MODE_M | SPI_SMEM_DIN5_MODE_M | SPI_SMEM_DIN6_MODE_M | SPI_SMEM_DIN7_MODE_M | SPI_SMEM_DINS_MODE_M)))
                       | (din_mode << SPI_SMEM_DIN0_MODE_S) | (din_mode << SPI_SMEM_DIN1_MODE_S) | (din_mode << SPI_SMEM_DIN2_MODE_S) | (din_mode << SPI_SMEM_DIN3_MODE_S)
                       | (din_mode << SPI_SMEM_DIN4_MODE_S) | (din_mode << SPI_SMEM_DIN5_MODE_S) | (din_mode << SPI_SMEM_DIN6_MODE_S) | (din_mode << SPI_SMEM_DIN7_MODE_S) | (din_mode << SPI_SMEM_DINS_MODE_S);
    REG_WRITE(SPI_SMEM_DIN_MODE_REG(mspi_id), reg_val);
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI PSRAM din num
 *
 * @param mspi_id   SPI0 / SPI1
 * @param din_num  Din num value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_din_num(uint8_t mspi_id, uint8_t din_num)
{
    uint32_t reg_val = (REG_READ(SPI_SMEM_DIN_NUM_REG(mspi_id)) & (~(SPI_SMEM_DIN0_NUM_M | SPI_SMEM_DIN1_NUM_M | SPI_SMEM_DIN2_NUM_M | SPI_SMEM_DIN3_NUM_M | SPI_SMEM_DIN4_NUM_M | SPI_SMEM_DIN5_NUM_M | SPI_SMEM_DIN6_NUM_M | SPI_SMEM_DIN7_NUM_M | SPI_SMEM_DINS_NUM_M)))
                       | (din_num << SPI_SMEM_DIN0_NUM_S) | (din_num << SPI_SMEM_DIN1_NUM_S) | (din_num << SPI_SMEM_DIN2_NUM_S) | (din_num << SPI_SMEM_DIN3_NUM_S)
                       | (din_num << SPI_SMEM_DIN4_NUM_S) | (din_num << SPI_SMEM_DIN5_NUM_S) | (din_num << SPI_SMEM_DIN6_NUM_S) | (din_num << SPI_SMEM_DIN7_NUM_S) | (din_num << SPI_SMEM_DINS_NUM_S);
    REG_WRITE(SPI_SMEM_DIN_NUM_REG(mspi_id), reg_val);
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Octal PSRAM extra dummy
 *
 * @param mspi_id      SPI0 / SPI1
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_extra_dummy(uint8_t mspi_id, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_SMEM_TIMING_CALI_REG(mspi_id), SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_SMEM_TIMING_CALI_REG(mspi_id), SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
                          SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_SMEM_TIMING_CALI_REG(mspi_id), SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_SMEM_TIMING_CALI_REG(mspi_id), SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, 0,
                          SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    }
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(MSPI_TIMING_LL_MSPI_ID_0), SPI_MEM_TIMING_CALI_UPDATE);
}

/**
 * Get MSPI PSRAM dummy info
 *
 * @param mspi_id      SPI0 / SPI1
 * @param usr_rdummy    User read dummy
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_get_psram_dummy(uint8_t mspi_id, int *usr_rdummy, int *extra_dummy)
{
    *usr_rdummy = REG_GET_FIELD(SPI_MEM_CACHE_SCTRL_REG(mspi_id), SPI_MEM_SRAM_RDUMMY_CYCLELEN);
    *extra_dummy = REG_GET_FIELD(SPI_SMEM_TIMING_CALI_REG(mspi_id), SPI_SMEM_EXTRA_DUMMY_CYCLELEN);
}

/**
 * @brief Enable/Disable MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 * @param enable      enable / disable
 */
__attribute__((always_inline))
static inline void mspi_ll_enable_intr(uint8_t spi_num, uint32_t intr_mask, bool enable)
{
    if (enable) {
        SPIMEM0.mem_int_ena.val |= intr_mask;
    } else {
        SPIMEM0.mem_int_ena.val &= ~intr_mask;
    }
}

/**
 * @brief Clear MSPI controller interrupt
 *
 * @param mspi_id     mspi_id
 * @param intr_mask   interrupt mask
 */
__attribute__((always_inline))
static inline void mspi_ll_clear_intr(uint8_t spi_num, uint32_t intr_mask)
{
    SPIMEM0.mem_int_clr.val = intr_mask;
}

/**
 * @brief Get MSPI controller interrupt raw
 *
 * @param mspi_id     mspi_id
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_get_intr_raw(uint8_t spi_num)
{
    return SPIMEM0.mem_int_raw.val;
}

#ifdef __cplusplus
}
#endif
