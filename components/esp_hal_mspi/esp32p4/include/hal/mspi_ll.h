/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

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
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/soc.h"
#include "soc/iomux_mspi_pin_reg.h"
#include "soc/iomux_mspi_pin_struct.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/spi_mem_c_reg.h"
#include "soc/spi1_mem_c_reg.h"
#include "soc/clk_tree_defs.h"
#include "soc/spi_mem_struct.h"
#include "soc/spi_mem_s_struct.h"
#include "hal/config.h"
#include "soc/lp_system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MSPI_LL_PERIPH_NUM                            4
#define MSPI_TIMING_LL_MSPI_ID_0                      0
#define MSPI_TIMING_LL_MSPI_ID_1                      1

// PSRAM frequency should be constrained by AXI frequency to avoid FIFO underflow.
#define MSPI_TIMING_LL_PSRAM_FREQ_AXI_CONSTRAINED     1

#define MSPI_TIMING_LL_HP_FLASH_CORE_CLK_DIV          4
#define MSPI_TIMING_LL_LP_FLASH_CORE_CLK_DIV          6
#define MSPI_TIMING_LL_FLASH_FDUMMY_RIN_SUPPORTED     1

#define MSPI_TIMING_LL_FLASH_OCT_MASK                 (SPI_MEM_C_FCMD_OCT | SPI_MEM_C_FADDR_OCT | SPI_MEM_C_FDIN_OCT | SPI_MEM_C_FDOUT_OCT)
#define MSPI_TIMING_LL_FLASH_QUAD_MASK                (SPI_MEM_C_FASTRD_MODE | SPI_MEM_C_FREAD_DUAL | SPI_MEM_C_FREAD_DIO | SPI_MEM_C_FREAD_QUAD | SPI_MEM_C_FREAD_QIO)
#define MSPI_TIMING_LL_FLASH_QIO_MODE_MASK            (SPI_MEM_C_FREAD_QIO | SPI_MEM_C_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_QUAD_MODE_MASK           (SPI_MEM_C_FREAD_QUAD | SPI_MEM_C_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_DIO_MODE_MASK            (SPI_MEM_C_FREAD_DIO | SPI_MEM_C_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_DUAL_MODE_MASK           (SPI_MEM_C_FREAD_DUAL | SPI_MEM_C_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_FAST_MODE_MASK           (SPI_MEM_C_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_SLOW_MODE_MASK           0

#define MSPI_LL_ADDR_INT_SUPPORTED                    1
#define MSPI_LL_PMS_INT_SUPPORTED                     1
#define MSPI_LL_ECC_INT_SUPPORTED                     1
#define MSPI_LL_FLASH_THRESH_INT_SUPPORTED            1
#define MSPI_LL_EVENT_SLV_ST_END                      (1<<3)
#define MSPI_LL_EVENT_MST_ST_END                      (1<<4)
#define MSPI_LL_EVENT_ECC_ERR                         (1<<5)
#define MSPI_LL_EVENT_PMS_REJECT                      (1<<6)
#define MSPI_LL_EVENT_AXI_RADDR_ERR                   (1<<7)
#define MSPI_LL_EVENT_AXI_WR_FLASH_ERR                (1<<8)
#define MSPI_LL_EVENT_AXI_WADDR_ERR                   (1<<9)
#define MSPI_LL_EVENT_RX_TRANS_OVF                    (1<<26)
#define MSPI_LL_EVENT_TX_TRANS_UDF                    (1<<27)
#define MSPI_LL_EVENT_MASK                            (MSPI_LL_EVENT_ECC_ERR | MSPI_LL_EVENT_PMS_REJECT | MSPI_LL_EVENT_AXI_RADDR_ERR | \
                                                      MSPI_LL_EVENT_AXI_WR_FLASH_ERR | MSPI_LL_EVENT_AXI_WADDR_ERR | MSPI_LL_EVENT_RX_TRANS_OVF | \
                                                      MSPI_LL_EVENT_TX_TRANS_UDF)

#define MSPI_LL_AXI_DISABLE_SUPPORTED                 1
#define MSPI_LL_INTR_EVENT_SUPPORTED                  1

/**
 * MSPI DQS ID
 */
typedef enum {
    MSPI_LL_DQS_ID_0,
    MSPI_LL_DQS_ID_1,
} mspi_ll_dqs_id_t;

/**
 * MSPI DQS Phase
 */
typedef enum {
    MSPI_LL_DQS_PHASE_67_5,
    MSPI_LL_DQS_PHASE_78_75,
    MSPI_LL_DQS_PHASE_90,
    MSPI_LL_DQS_PHASE_101_25,
    MSPI_LL_DQS_PHASE_MAX,
} mspi_ll_dqs_phase_t;

/**
 * MSPI Delayline
 *
 * @note The Sequence of these enums should not be changed
 */
typedef enum {
    MSPI_LL_PIN_D = 0,
    MSPI_LL_PIN_Q,
    MSPI_LL_PIN_WP,
    MSPI_LL_PIN_HD,
    MSPI_LL_PIN_D4,
    MSPI_LL_PIN_D5,
    MSPI_LL_PIN_D6,
    MSPI_LL_PIN_D7,
    MSPI_LL_PIN_DQS0,
    MSPI_LL_PIN_CLK,
    MSPI_LL_PIN_CS,
    MSPI_LL_PIN_D8,
    MSPI_LL_PIN_D9,
    MSPI_LL_PIN_D10,
    MSPI_LL_PIN_D11,
    MSPI_LL_PIN_D12,
    MSPI_LL_PIN_D13,
    MSPI_LL_PIN_D14,
    MSPI_LL_PIN_D15,
    MSPI_LL_PIN_DQS1,
    MSPI_LL_PIN_MAX,
} mspi_ll_pin_t;

/**
 * MSPI flash mode
 */
typedef enum {
    MSPI_TIMING_LL_FLASH_OPI_MODE = BIT(0),
    MSPI_TIMING_LL_FLASH_QIO_MODE = BIT(1),
    MSPI_TIMING_LL_FLASH_QUAD_MODE = BIT(2),
    MSPI_TIMING_LL_FLASH_DIO_MODE = BIT(3),
    MSPI_TIMING_LL_FLASH_DUAL_MODE = BIT(4),
    MSPI_TIMING_LL_FLASH_FAST_MODE = BIT(5),
    MSPI_TIMING_LL_FLASH_SLOW_MODE = BIT(6),
} mspi_timing_ll_flash_mode_t;

/**
 * Reset the MSPI clock
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_reset_mspi(void)
{
    REG_SET_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
    REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define mspi_timing_ll_reset_mspi(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _mspi_timing_ll_reset_mspi(__VA_ARGS__); \
    } while(0)

__attribute__((always_inline))
static inline void _mspi_timing_ll_reset_mspi_apb(void)
{
    REG_SET_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_APB);
    REG_CLR_BIT(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_APB);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_RC_ATOMIC_ENV variable in advance
#define mspi_timing_ll_reset_mspi_apb(...) do { \
        (void)__DECLARE_RCC_RC_ATOMIC_ENV; \
        _mspi_timing_ll_reset_mspi_apb(__VA_ARGS__); \
    } while(0)

/*---------------------------------------------------------------
                    PSRAM tuning
---------------------------------------------------------------*/
/**
 * Set all MSPI DQS phase
 *
 * @param dqs_id  DQS ID
 * @param phase   Phase
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_dqs_phase(mspi_ll_dqs_id_t dqs_id, mspi_ll_dqs_phase_t phase)
{
    HAL_ASSERT(dqs_id < 2);
    if (dqs_id == MSPI_LL_DQS_ID_0) {
        REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_PHASE, phase);
    } else {
        REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_PHASE, phase);
    }
}

/**
 * Set all MSPI delayline
 *
 * @param pin        Pin
 * @param delayline  Delayline
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_delayline(mspi_ll_pin_t pin, uint8_t delayline)
{
    HAL_ASSERT(pin < MSPI_LL_PIN_MAX);

    switch (pin) {
    case MSPI_LL_PIN_DQS0:
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_90 = delayline;
        MSPI_IOMUX.psram_pin_group.dqs0.reg_psram_dqs_delay_270 = delayline;
        break;
    case MSPI_LL_PIN_DQS1:
        MSPI_IOMUX.psram_pin_group.dqs1.reg_psram_dqs_delay_90 = delayline;
        MSPI_IOMUX.psram_pin_group.dqs1.reg_psram_dqs_delay_270 = delayline;
        break;
    case MSPI_LL_PIN_D:
    case MSPI_LL_PIN_Q:
    case MSPI_LL_PIN_WP:
    case MSPI_LL_PIN_HD:
    case MSPI_LL_PIN_D4:
    case MSPI_LL_PIN_D5:
    case MSPI_LL_PIN_D6:
    case MSPI_LL_PIN_D7:
        MSPI_IOMUX.psram_pin_group.pin_group0[pin].reg_psram_pin_dlc = delayline;
        break;
    case MSPI_LL_PIN_CLK:
    case MSPI_LL_PIN_CS:
    case MSPI_LL_PIN_D8:
    case MSPI_LL_PIN_D9:
    case MSPI_LL_PIN_D10:
    case MSPI_LL_PIN_D11:
    case MSPI_LL_PIN_D12:
    case MSPI_LL_PIN_D13:
    case MSPI_LL_PIN_D14:
    case MSPI_LL_PIN_D15:
        pin = (mspi_ll_pin_t)(pin - MSPI_LL_PIN_CLK);
        MSPI_IOMUX.psram_pin_group.pin_group1[pin].reg_psram_pin_dlc = delayline;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }
}

/**
 * Enable DQS
 *
 * @param en  Enable/disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_enable_dqs(bool en)
{
    if (en) {
        REG_SET_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
        REG_SET_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_XPD);
    } else {
        REG_CLR_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_XPD);
        REG_CLR_BIT(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_XPD);
    }
}

/**
 * Set all MSPI pin drive
 *
 * @param drv  Pin drive
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_pin_drv_set(uint8_t drv)
{
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_D_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_D_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_Q_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_Q_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_WP_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_WP_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_HOLD_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_HOLD_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ4_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ4_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ5_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ5_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ6_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ6_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ7_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ7_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ8_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ8_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ9_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ9_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ10_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ10_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ11_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ11_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ12_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ12_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ13_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ13_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ14_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ14_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQ15_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQ15_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_0_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_0_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_DQS_1_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_DQS_1_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CK_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CK_DRV, drv);
    REG_SET_FIELD(IOMUX_MSPI_PIN_PSRAM_CS_PIN0_REG, IOMUX_MSPI_PIN_REG_PSRAM_CS_DRV, drv);
}

/*---------------------------------------------------------------
                    Flash tuning
---------------------------------------------------------------*/
/*
 * @brief Select FLASH clock source
 *
 * @param mspi_id      mspi_id
 * @param clk_src      clock source, see valid sources in type `soc_periph_flash_clk_src_t`
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_set_flash_clk_src(uint32_t mspi_id, soc_periph_flash_clk_src_t clk_src)
{
    HAL_ASSERT(mspi_id == MSPI_TIMING_LL_MSPI_ID_0);
    uint32_t clk_val = 0;
    switch (clk_src) {
    case FLASH_CLK_SRC_XTAL:
        clk_val = 0;
        break;
    case FLASH_CLK_SRC_SPLL:
        clk_val = 1;
        break;
    case FLASH_CLK_SRC_CPLL:
        clk_val = 2;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    HP_SYS_CLKRST.soc_clk_ctrl0.reg_flash_sys_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_flash_pll_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_flash_clk_src_sel = clk_val;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mspi_timing_ll_set_flash_clk_src(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _mspi_timing_ll_set_flash_clk_src(__VA_ARGS__); \
    } while(0)

/**
 * Set MSPI Flash core clock
 *
 * @param spi_num       SPI0 / SPI1
 * @param core_clk_mhz  core clock mhz
 */
__attribute__((always_inline))
static inline void _mspi_timing_ll_set_flash_core_clock(int spi_num, uint32_t core_clk_mhz)
{
    HAL_ASSERT(spi_num == MSPI_TIMING_LL_MSPI_ID_0);
    if (core_clk_mhz == 120) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl00, reg_flash_core_clk_div_num, (MSPI_TIMING_LL_HP_FLASH_CORE_CLK_DIV - 1));
        HP_SYS_CLKRST.peri_clk_ctrl00.reg_flash_core_clk_en = 1;
    } else if (core_clk_mhz == 80) {
        HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl00, reg_flash_core_clk_div_num, (MSPI_TIMING_LL_LP_FLASH_CORE_CLK_DIV - 1));
        HP_SYS_CLKRST.peri_clk_ctrl00.reg_flash_core_clk_en = 1;
    } else {
        //ESP32P4 flash timing tuning is based on SPLL==480MHz, flash_core_clock==120MHz / 80MHz. We add assertion here to ensure this
        HAL_ASSERT(false);
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mspi_timing_ll_set_flash_core_clock(...) do { \
        (void)__DECLARE_RCC_ATOMIC_ENV; \
        _mspi_timing_ll_set_flash_core_clock(__VA_ARGS__); \
    } while(0)

/**
 * Set MSPI Flash clock
 *
 * @param spi_num  SPI0 / SPI1
 * @param freqdiv  Divider value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_clock(uint8_t spi_num, uint32_t freqdiv)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        if (freqdiv == 1) {
            WRITE_PERI_REG(SPI_MEM_C_CLOCK_REG, SPI_MEM_C_CLK_EQU_SYSCLK);
        } else {
            uint32_t freqbits = (((freqdiv - 1) << SPI_MEM_C_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_C_CLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_C_CLKCNT_L_S);
            WRITE_PERI_REG(SPI_MEM_C_CLOCK_REG, freqbits);
        }
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        if (freqdiv == 1) {
            WRITE_PERI_REG(SPI1_MEM_C_CLOCK_REG, SPI1_MEM_C_CLK_EQU_SYSCLK);
        } else {
            uint32_t freqbits = (((freqdiv - 1) << SPI1_MEM_C_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI1_MEM_C_CLKCNT_H_S)) | ((freqdiv - 1) << SPI1_MEM_C_CLKCNT_L_S);
            WRITE_PERI_REG(SPI1_MEM_C_CLOCK_REG, freqbits);
        }
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * Enable Flash timing adjust clock
 *
 * @param spi_num  SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timinng_ll_enable_flash_timing_adjust_clk(uint8_t spi_num)
{
    HAL_ASSERT(spi_num == MSPI_TIMING_LL_MSPI_ID_0);
    // Should set ie always on to ensure it can read flash sr2
    REG_SET_BIT(SPI_MEM_C_CTRL_REG, SPI_MEM_C_DATA_IE_ALWAYS_ON);
    REG_GET_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CLK_ENA);
}

/**
 * Set MSPI Flash din mode
 *
 * @param spi_num   SPI0 / SPI1
 * @param din_mode  Din mode value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_din_mode(uint8_t spi_num, uint8_t din_mode)
{
    (void)spi_num;
    uint32_t reg_val = (REG_READ(SPI_MEM_C_DIN_MODE_REG) & (~(SPI_MEM_C_DIN0_MODE_M | SPI_MEM_C_DIN1_MODE_M | SPI_MEM_C_DIN2_MODE_M | SPI_MEM_C_DIN3_MODE_M | SPI_MEM_C_DIN4_MODE_M | SPI_MEM_C_DIN5_MODE_M | SPI_MEM_C_DIN6_MODE_M | SPI_MEM_C_DIN7_MODE_M | SPI_MEM_C_DINS_MODE_M)))
                       | (din_mode << SPI_MEM_C_DIN0_MODE_S) | (din_mode << SPI_MEM_C_DIN1_MODE_S) | (din_mode << SPI_MEM_C_DIN2_MODE_S) | (din_mode << SPI_MEM_C_DIN3_MODE_S)
                       | (din_mode << SPI_MEM_C_DIN4_MODE_S) | (din_mode << SPI_MEM_C_DIN5_MODE_S) | (din_mode << SPI_MEM_C_DIN6_MODE_S) | (din_mode << SPI_MEM_C_DIN7_MODE_S) | (din_mode << SPI_MEM_C_DINS_MODE_S);
    REG_WRITE(SPI_MEM_C_DIN_MODE_REG, reg_val);
    REG_SET_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Flash din num
 *
 * @param spi_num   SPI0 / SPI1
 * @param din_num   Din num value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_din_num(uint8_t spi_num, uint8_t din_num)
{
    (void)spi_num;
    uint32_t reg_val = (REG_READ(SPI_MEM_C_DIN_NUM_REG) & (~(SPI_MEM_C_DIN0_NUM_M | SPI_MEM_C_DIN1_NUM_M | SPI_MEM_C_DIN2_NUM_M | SPI_MEM_C_DIN3_NUM_M | SPI_MEM_C_DIN4_NUM_M | SPI_MEM_C_DIN5_NUM_M | SPI_MEM_C_DIN6_NUM_M | SPI_MEM_C_DIN7_NUM_M | SPI_MEM_C_DINS_NUM_M)))
                       | (din_num << SPI_MEM_C_DIN0_NUM_S) | (din_num << SPI_MEM_C_DIN1_NUM_S) | (din_num << SPI_MEM_C_DIN2_NUM_S) | (din_num << SPI_MEM_C_DIN3_NUM_S)
                       | (din_num << SPI_MEM_C_DIN4_NUM_S) | (din_num << SPI_MEM_C_DIN5_NUM_S) | (din_num << SPI_MEM_C_DIN6_NUM_S) | (din_num << SPI_MEM_C_DIN7_NUM_S) | (din_num << SPI_MEM_C_DINS_NUM_S);
    REG_WRITE(SPI_MEM_C_DIN_NUM_REG, reg_val);
    REG_SET_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_UPDATE);
}

/**
 * Set MSPI Flash extra dummy
 *
 * @param spi_num      SPI0 / SPI1
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        if (extra_dummy > 0) {
            SET_PERI_REG_MASK(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_M);
            SET_PERI_REG_BITS(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_EXTRA_DUMMY_CYCLELEN_V, extra_dummy, SPI_MEM_C_EXTRA_DUMMY_CYCLELEN_S);
        } else {
            CLEAR_PERI_REG_MASK(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_M);
            SET_PERI_REG_BITS(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_EXTRA_DUMMY_CYCLELEN_V, 0, SPI_MEM_C_EXTRA_DUMMY_CYCLELEN_S);
        }
        REG_SET_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_UPDATE);
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        if (extra_dummy > 0) {
            SET_PERI_REG_MASK(SPI1_MEM_C_TIMING_CALI_REG, SPI1_MEM_C_TIMING_CALI);
            SET_PERI_REG_BITS(SPI1_MEM_C_TIMING_CALI_REG, SPI1_MEM_C_EXTRA_DUMMY_CYCLELEN_V, extra_dummy, SPI1_MEM_C_EXTRA_DUMMY_CYCLELEN_S);
        } else {
            CLEAR_PERI_REG_MASK(SPI1_MEM_C_TIMING_CALI_REG, SPI1_MEM_C_TIMING_CALI);
            SET_PERI_REG_BITS(SPI1_MEM_C_TIMING_CALI_REG, SPI1_MEM_C_EXTRA_DUMMY_CYCLELEN_V, 0, SPI1_MEM_C_EXTRA_DUMMY_CYCLELEN_S);
        }
        REG_SET_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CALI_UPDATE);
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * Set MSPI Flash user dummy
 *
 * @param spi_num      SPI0 / SPI1
 * @param user_dummy   user dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_user_dummy(uint8_t spi_num, uint8_t user_dummy)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        REG_SET_FIELD(SPI_MEM_C_USER1_REG, SPI_MEM_C_USR_DUMMY_CYCLELEN, user_dummy);
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        REG_SET_FIELD(SPI1_MEM_C_USER1_REG, SPI1_MEM_C_USR_DUMMY_CYCLELEN, user_dummy);
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * Enable/Disable Flash variable dummy
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_enable_flash_variable_dummy(uint8_t spi_num, bool enable)
{
    (void)spi_num;
    REG_SET_FIELD(SPI1_MEM_C_DDR_REG, SPI1_MEM_C_FMEM_VAR_DUMMY, enable);
}

/**
 * Mask invalid DQS
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_mask_invalid_dqs(uint8_t spi_num, bool enable)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        REG_SET_FIELD(SPI_MEM_C_CTRL_REG, SPI_MEM_C_FDUMMY_RIN, enable);
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        REG_SET_FIELD(SPI1_MEM_C_CTRL_REG, SPI1_MEM_C_FDUMMY_RIN, enable);
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * Get if cs setup is enabled or not
 *
 * @param spi_num    SPI0 / SPI1
 *
 * @return
 *        true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool mspi_timing_ll_is_cs_setup_enabled(uint8_t spi_num)
{
    (void)spi_num;
    return REG_GET_BIT(SPI_MEM_C_USER_REG, SPI_MEM_C_CS_SETUP);
}

/**
 * Get cs setup val
 *
 * @param spi_num    SPI0 / SPI1
 *
 * @return
 *        cs setup reg val
 */
static inline uint32_t mspi_timing_ll_get_cs_setup_val(uint8_t spi_num)
{
    (void)spi_num;
    return REG_GET_FIELD(SPI_MEM_C_CTRL2_REG, SPI_MEM_C_CS_SETUP_TIME);
}

/**
 * Get if cs hold is enabled or not
 *
 * @param spi_num    SPI0 / SPI1
 *
 * @return
 *        true: enabled; false: disabled
 */
__attribute__((always_inline))
static inline bool mspi_timing_ll_is_cs_hold_enabled(uint8_t spi_num)
{
    (void)spi_num;
    return REG_GET_FIELD(SPI_MEM_C_USER_REG, SPI_MEM_C_CS_HOLD);
}

/**
 * Get cs hold val
 *
 * @param spi_num    SPI0 / SPI1
 *
 * @return
 *        cs hold reg val
 */
static inline uint32_t mspi_timing_ll_get_cs_hold_val(uint8_t spi_num)
{
    (void)spi_num;
    return REG_GET_FIELD(SPI_MEM_C_CTRL2_REG, SPI_MEM_C_CS_HOLD_TIME);
}

/**
 * Get clock reg val
 *
 * @param spi_num    SPI0 / SPI1
 *
 * @return
 *        clock reg val
 */
__attribute__((always_inline))
static inline uint32_t mspi_timing_ll_get_clock_reg(uint8_t spi_num)
{
    (void)spi_num;
    return READ_PERI_REG(SPI1_MEM_C_CLOCK_REG);
}

/**
 * Get MSPI Flash mode
 *
 * @param spi_num   SPI0 / SPI1
 *
 * @return Flash mode
 */
__attribute__((always_inline))
static inline mspi_timing_ll_flash_mode_t mspi_timing_ll_get_flash_mode(uint8_t spi_num)
{
    uint32_t ctrl_reg = READ_PERI_REG(SPI_MEM_C_CTRL_REG);
    if (ctrl_reg & MSPI_TIMING_LL_FLASH_OCT_MASK) {
        return MSPI_TIMING_LL_FLASH_OPI_MODE;
    }

    switch (ctrl_reg & MSPI_TIMING_LL_FLASH_QUAD_MASK) {
    case MSPI_TIMING_LL_FLASH_QIO_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_QIO_MODE;
    case MSPI_TIMING_LL_FLASH_QUAD_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_QUAD_MODE;
    case MSPI_TIMING_LL_FLASH_DIO_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_DIO_MODE;
    case MSPI_TIMING_LL_FLASH_DUAL_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_DUAL_MODE;
    case MSPI_TIMING_LL_FLASH_FAST_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_FAST_MODE;
    case MSPI_TIMING_LL_FLASH_SLOW_MODE_MASK:
        return MSPI_TIMING_LL_FLASH_SLOW_MODE;
    default:
        HAL_ASSERT(false);
        return (mspi_timing_ll_flash_mode_t)0;
    }
}

/**
 * Get MSPI flash dummy info
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_get_flash_dummy(uint8_t spi_num, int *usr_dummy, int *extra_dummy)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        *usr_dummy = REG_GET_FIELD(SPI_MEM_C_USER1_REG, SPI_MEM_C_USR_DUMMY_CYCLELEN);
        *extra_dummy = REG_GET_FIELD(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_EXTRA_DUMMY_CYCLELEN);
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        *usr_dummy = REG_GET_FIELD(SPI1_MEM_C_USER1_REG, SPI1_MEM_C_USR_DUMMY_CYCLELEN);
        *extra_dummy = REG_GET_FIELD(SPI1_MEM_C_TIMING_CALI_REG, SPI1_MEM_C_EXTRA_DUMMY_CYCLELEN);
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * Mask invalid DQS
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline uint32_t mspi_timing_ll_get_invalid_dqs_mask(uint8_t spi_num)
{
    if (spi_num == MSPI_TIMING_LL_MSPI_ID_0) {
        return REG_GET_FIELD(SPI_MEM_C_CTRL_REG, SPI_MEM_C_FDUMMY_RIN);
    } else if (spi_num == MSPI_TIMING_LL_MSPI_ID_1) {
        return REG_GET_FIELD(SPI1_MEM_C_CTRL_REG, SPI1_MEM_C_FDUMMY_RIN);
    } else {
        HAL_ASSERT(false);
    }
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
        SPIMEM0.int_ena.val |= intr_mask;
    } else {
        SPIMEM0.int_ena.val &= ~intr_mask;
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
    SPIMEM0.int_clr.val = intr_mask;
}

/**
 * @brief Get MSPI controller interrupt raw
 *
 * @param mspi_id     mspi_id
 */
__attribute__((always_inline))
static inline uint32_t mspi_ll_get_intr_raw(uint8_t spi_num)
{
    return SPIMEM0.int_raw.val;
}

/**
 * Enable AXI access to flash
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_flash_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM0.cache_fctrl.close_axi_inf_en = !enable;
}

/**
 * Enable AXI access to PSRAM
 *
 * @param spi_num  SPI0 / SPI1
 * @param enable   Enable / Disable
 */
__attribute__((always_inline))
static inline void mspi_ll_psram_enable_axi_access(uint8_t spi_num, bool enable)
{
    SPIMEM2.mem_cache_fctrl.close_axi_inf_en = !enable;
}

/*---------------------------------------------------------------
                    MSPI IOMUX pin configuration
---------------------------------------------------------------*/
/**
 * @brief MSPI IOMUX pin ID enumeration for Flash pins
 * ID ranges:
 *   0-5:   Flash pins (CS, Q, WP, HOLD, CK, D) - maps to flash_pin_regs[] array index
 */
typedef enum {
    MSPI_LL_PIN_ID_FLASH_CS = 0,   /**< Flash CS pin */
    MSPI_LL_PIN_ID_FLASH_Q,        /**< Flash Q pin */
    MSPI_LL_PIN_ID_FLASH_WP,       /**< Flash WP pin */
    MSPI_LL_PIN_ID_FLASH_HOLD,     /**< Flash HOLD pin */
    MSPI_LL_PIN_ID_FLASH_CK,       /**< Flash CLK pin */
    MSPI_LL_PIN_ID_FLASH_D,        /**< Flash D pin */
} mspi_ll_flash_pin_id_t;

/**
 * @brief MSPI IOMUX Flash pin configuration structure
 * Register layout: hys(0), ie(1), wpu(2), wpd(3), drv(5:4)
 */
typedef union {
    struct {
        uint32_t hys: 1;    /**< Hysteresis enable */
        uint32_t ie: 1;     /**< Input enable */
        uint32_t wpu: 1;    /**< Weak pull-up enable */
        uint32_t wpd: 1;    /**< Weak pull-down enable */
        uint32_t drv: 2;    /**< Drive strength (0-3) */
        uint32_t reserved: 26; /**< Reserved bits */
    };
    uint32_t val;           /**< Raw register value for atomic write */
} mspi_ll_flash_pin_cfg_t;

/**
 * @brief Set configuration for Flash pin
 *
 * @param pin_id Pin ID (MSPI_LL_PIN_ID_FLASH_CS to MSPI_LL_PIN_ID_FLASH_D, i.e., 0-5)
 * @param cfg Pin configuration structure
 */
__attribute__((always_inline))
static inline void mspi_ll_set_flash_pin_cfg(mspi_ll_flash_pin_id_t pin_id, const mspi_ll_flash_pin_cfg_t *cfg)
{
    HAL_ASSERT(pin_id <= MSPI_LL_PIN_ID_FLASH_D);
    MSPI_IOMUX.flash_pin_regs[pin_id].val = cfg->val;
}

/**
 * @brief Hold all Flash pins
 * Sets all Flash pins (CS, Q, WP, HOLD, CK, D) to hold status
 */
__attribute__((always_inline))
static inline void mspi_ll_hold_all_flash_pins(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300 // Only rev3+ chips support mspi pad holding.
    LP_SYS.ded_pad_rtc_hold_ctrl.ded_pad_rtc_hold_ctrl |= 0x3F;
#endif
}

/**
 * @brief Unhold all Flash pins
 * Releases hold status for all Flash pins (CS, Q, WP, HOLD, CK, D)
 */
__attribute__((always_inline))
static inline void mspi_ll_unhold_all_flash_pins(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300 // Only rev3+ chips support mspi pad holding.
    LP_SYS.ded_pad_rtc_hold_ctrl.ded_pad_rtc_hold_ctrl &= ~0x3F;
#endif
}

/**
 * @brief Hold all PSRAM pins
 * Sets all PSRAM pins (pin_group0, dqs0, pin_group1, dqs1) to hold status
 */
__attribute__((always_inline))
static inline void mspi_ll_hold_all_psram_pins(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300 // Only rev3+ chips support mspi pad holding.
    LP_SYS.ded_pad_rtc_hold_ctrl.ded_pad_rtc_hold_ctrl |= 0x3FFFFC0;
#endif
}

/**
 * @brief Unhold all PSRAM pins
 * Releases hold status for all PSRAM pins (pin_group0, dqs0, pin_group1, dqs1)
 */
__attribute__((always_inline))
static inline void mspi_ll_unhold_all_psram_pins(void)
{
#if HAL_CONFIG(CHIP_SUPPORT_MIN_REV) >= 300 // Only rev3+ chips support mspi pad holding.
    LP_SYS.ded_pad_rtc_hold_ctrl.ded_pad_rtc_hold_ctrl &= ~0x3FFFFC0;
#endif
}

#ifdef __cplusplus
}
#endif
