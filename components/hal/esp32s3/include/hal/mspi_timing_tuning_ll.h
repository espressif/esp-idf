/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/soc.h"
#include "soc/spi_mem_reg.h"
#include "soc/io_mux_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ARRAY_SIZE(arr) (sizeof((arr))/sizeof(*(arr)))
#define MSPI_TIMING_LL_FLASH_OCT_MASK                 (SPI_MEM_FCMD_OCT | SPI_MEM_FADDR_OCT | SPI_MEM_FDIN_OCT | SPI_MEM_FDOUT_OCT)
#define MSPI_TIMING_LL_FLASH_QUAD_MASK                (SPI_MEM_FASTRD_MODE | SPI_MEM_FREAD_DUAL | SPI_MEM_FREAD_DIO | SPI_MEM_FREAD_QUAD | SPI_MEM_FREAD_QIO)
#define MSPI_TIMING_LL_FLASH_QIO_MODE_MASK            (SPI_MEM_FREAD_QIO | SPI_MEM_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_QUAD_MODE_MASK           (SPI_MEM_FREAD_QUAD | SPI_MEM_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_DIO_MODE_MASK            (SPI_MEM_FREAD_DIO | SPI_MEM_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_DUAL_MODE_MASK           (SPI_MEM_FREAD_DUAL | SPI_MEM_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_FAST_MODE_MASK           (SPI_MEM_FASTRD_MODE)
#define MSPI_TIMING_LL_FLASH_SLOW_MODE_MASK           0

#define MSPI_TIMING_LL_CORE_CLOCK_MHZ_DEFAULT         80

#define MSPI_TIMING_LL_FLASH_CPU_CLK_SRC_BINDED       1

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
 * Set all MSPI pin drive strength
 *
 * @param spi_num  SPI0 / SPI1
 * @param val      Pin drive strength
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_all_pin_drive(uint8_t spi_num, uint32_t val)
{
    SET_PERI_REG_MASK(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPICLK_PAD_DRV_CTL_EN);
    REG_SET_FIELD(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPI_SMEM_SPICLK_FUN_DRV, val);
    REG_SET_FIELD(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPI_FMEM_SPICLK_FUN_DRV, val);

    uint32_t regs[] = {IO_MUX_GPIO27_REG, IO_MUX_GPIO28_REG,
                       IO_MUX_GPIO31_REG, IO_MUX_GPIO32_REG,
                       IO_MUX_GPIO33_REG, IO_MUX_GPIO34_REG,
                       IO_MUX_GPIO35_REG, IO_MUX_GPIO36_REG,
                       IO_MUX_GPIO37_REG};
    for (int i = 0; i < ARRAY_SIZE(regs); i++) {
        PIN_SET_DRV(regs[i], val);
    }
}

/**
 * Set all MSPI Flash clock pin drive strength
 *
 * @param spi_num  SPI0 / SPI1
 * @param val      Pin drive strength
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_clock_pin_drive(uint8_t spi_num, uint32_t val)
{
    bool clk_pin_drive_control = GET_PERI_REG_MASK(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPICLK_PAD_DRV_CTL_EN);
    //You should never call this function, while `mspi_timing_ll_set_all_pin_drive()` isn't called
    HAL_ASSERT(clk_pin_drive_control);
    REG_SET_FIELD(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPI_FMEM_SPICLK_FUN_DRV, val);
}

/**
 * Set all MSPI PSRAM clock pin drive strength
 *
 * @param spi_num  SPI0 / SPI1
 * @param val      Pin drive strength
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_clock_pin_drive(uint8_t spi_num, uint32_t val)
{
    bool clk_pin_drive_control = GET_PERI_REG_MASK(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPICLK_PAD_DRV_CTL_EN);
    //You should never call this function, while `mspi_timing_ll_set_all_pin_drive()` isn't called
    HAL_ASSERT(clk_pin_drive_control);
    REG_SET_FIELD(SPI_MEM_DATE_REG(spi_num), SPI_MEM_SPI_SMEM_SPICLK_FUN_DRV, val);
}

/**
 * Enable Flash HCLK
 *
 * @param spi_num  SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timinng_ll_enable_flash_hclk(uint8_t spi_num)
{
    REG_SET_BIT(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CLK_ENA);
}

/**
 * Enable PSRAM HCLK
 *
 * @param spi_num  SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timinng_ll_enable_psram_hclk(uint8_t spi_num)
{
    REG_SET_BIT(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CLK_ENA);
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
    REG_SET_FIELD(SPI_MEM_DDR_REG(1), SPI_MEM_SPI_FMEM_VAR_DUMMY, enable);
}

/**
 * Set MSPI core clock
 *
 * @param spi_num       SPI0 / SPI1
 * @param core_clk_mhz  core clock mhz
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_core_clock(uint8_t spi_num, uint32_t core_clk_mhz)
{
    uint32_t reg_val = 0;

    switch (core_clk_mhz) {
        case 80:
            reg_val = 0;
            break;
        case 120:
            reg_val = 1;
            break;
        case 160:
            reg_val = 2;
            break;
        case 240:
            reg_val = 3;
            break;
        default:
            HAL_ASSERT(false);
    }

    REG_SET_FIELD(SPI_MEM_CORE_CLK_SEL_REG(spi_num), SPI_MEM_CORE_CLK_SEL, reg_val);
}

/**
 * Set MSPI Flash clock
 *
 * @param spi_num  SPI0 / SPI1
 * @param freqdiv  Divider value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_clock(uint8_t spi_num, uint32_t freqdiv)
{
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), SPI_MEM_CLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv - 1) << SPI_MEM_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_CLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_CLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_CLOCK_REG(spi_num), freqbits);
    }
}

/**
 * Set MSPI PSRAM clock
 *
 * @param spi_num  SPI0 / SPI1
 * @param freqdiv  Divider value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_clock(uint8_t spi_num, uint32_t freqdiv)
{
    if (freqdiv == 1) {
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), SPI_MEM_SCLK_EQU_SYSCLK);
    } else {
        uint32_t freqbits = (((freqdiv-1)<<SPI_MEM_SCLKCNT_N_S)) | (((freqdiv/2-1)<<SPI_MEM_SCLKCNT_H_S)) | ((freqdiv-1)<<SPI_MEM_SCLKCNT_L_S);
        WRITE_PERI_REG(SPI_MEM_SRAM_CLK_REG(spi_num), freqbits);
    }
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
    uint32_t reg_val = (REG_READ(SPI_MEM_DIN_MODE_REG(spi_num)) & (~(SPI_MEM_DIN0_MODE_M | SPI_MEM_DIN1_MODE_M | SPI_MEM_DIN2_MODE_M | SPI_MEM_DIN3_MODE_M | SPI_MEM_DIN4_MODE_M | SPI_MEM_DIN5_MODE_M | SPI_MEM_DIN6_MODE_M | SPI_MEM_DIN7_MODE_M | SPI_MEM_DINS_MODE_M)))
        | (din_mode << SPI_MEM_DIN0_MODE_S) | (din_mode << SPI_MEM_DIN1_MODE_S) | (din_mode << SPI_MEM_DIN2_MODE_S) | (din_mode << SPI_MEM_DIN3_MODE_S)
        | (din_mode << SPI_MEM_DIN4_MODE_S) | (din_mode << SPI_MEM_DIN5_MODE_S) | (din_mode << SPI_MEM_DIN6_MODE_S) | (din_mode << SPI_MEM_DIN7_MODE_S) | (din_mode << SPI_MEM_DINS_MODE_S);
    REG_WRITE(SPI_MEM_DIN_MODE_REG(spi_num), reg_val);
}

/**
 * Set MSPI Flash din num
 *
 * @param spi_num   SPI0 / SPI1
 * @param din_num  Din num value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_flash_din_num(uint8_t spi_num, uint8_t din_num)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_DIN_NUM_REG(spi_num)) & (~(SPI_MEM_DIN0_NUM_M | SPI_MEM_DIN1_NUM_M | SPI_MEM_DIN2_NUM_M | SPI_MEM_DIN3_NUM_M | SPI_MEM_DIN4_NUM_M | SPI_MEM_DIN5_NUM_M | SPI_MEM_DIN6_NUM_M | SPI_MEM_DIN7_NUM_M | SPI_MEM_DINS_NUM_M)))
        | (din_num << SPI_MEM_DIN0_NUM_S) | (din_num << SPI_MEM_DIN1_NUM_S) | (din_num << SPI_MEM_DIN2_NUM_S) | (din_num << SPI_MEM_DIN3_NUM_S)
        | (din_num << SPI_MEM_DIN4_NUM_S) | (din_num << SPI_MEM_DIN5_NUM_S) | (din_num << SPI_MEM_DIN6_NUM_S) | (din_num << SPI_MEM_DIN7_NUM_S) | (din_num << SPI_MEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_DIN_NUM_REG(spi_num), reg_val);
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
    uint32_t ctrl_reg = READ_PERI_REG(SPI_MEM_CTRL_REG(0));
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
 * Set MSPI Octal Flash extra dummy
 *
 * @param spi_num      SPI0 / SPI1
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_octal_flash_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
            SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_TIMING_CALI_REG(spi_num), SPI_MEM_EXTRA_DUMMY_CYCLELEN_V, 0,
            SPI_MEM_EXTRA_DUMMY_CYCLELEN_S);
    }
}

/**
 * Set MSPI Quad Flash dummy
 *
 * @param spi_num    SPI0 / SPI1
 * @param dummy      dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_quad_flash_dummy(uint8_t spi_num, uint8_t dummy)
{
    //HW workaround: Use normal dummy register to set extra dummy, the calibration dedicated extra dummy register doesn't work for quad mode
    SET_PERI_REG_MASK(SPI_MEM_USER_REG(spi_num), SPI_MEM_USR_DUMMY);
    SET_PERI_REG_BITS(SPI_MEM_USER1_REG(spi_num), SPI_MEM_USR_DUMMY_CYCLELEN_V, dummy, SPI_MEM_USR_DUMMY_CYCLELEN_S);
}

/**
 * Set MSPI PSRAM din mode
 *
 * @param spi_num   SPI0 / SPI1
 * @param din_mode  Din mode value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_din_mode(uint8_t spi_num, uint8_t din_mode)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_SPI_SMEM_DIN_MODE_REG(spi_num)) & (~(SPI_MEM_SPI_SMEM_DIN0_MODE_M | SPI_MEM_SPI_SMEM_DIN1_MODE_M | SPI_MEM_SPI_SMEM_DIN2_MODE_M | SPI_MEM_SPI_SMEM_DIN3_MODE_M | SPI_MEM_SPI_SMEM_DIN4_MODE_M | SPI_MEM_SPI_SMEM_DIN5_MODE_M | SPI_MEM_SPI_SMEM_DIN6_MODE_M | SPI_MEM_SPI_SMEM_DIN7_MODE_M | SPI_MEM_SPI_SMEM_DINS_MODE_M)))
        | (din_mode << SPI_MEM_SPI_SMEM_DIN0_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN1_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN2_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN3_MODE_S)
        | (din_mode << SPI_MEM_SPI_SMEM_DIN4_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN5_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN6_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DIN7_MODE_S) | (din_mode << SPI_MEM_SPI_SMEM_DINS_MODE_S);
    REG_WRITE(SPI_MEM_SPI_SMEM_DIN_MODE_REG(spi_num), reg_val);
}

/**
 * Set MSPI PSRAM din num
 *
 * @param spi_num   SPI0 / SPI1
 * @param din_num  Din num value
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_psram_din_num(uint8_t spi_num, uint8_t din_num)
{
    uint32_t reg_val = (REG_READ(SPI_MEM_SPI_SMEM_DIN_NUM_REG(spi_num)) & (~(SPI_MEM_SPI_SMEM_DIN0_NUM_M | SPI_MEM_SPI_SMEM_DIN1_NUM_M | SPI_MEM_SPI_SMEM_DIN2_NUM_M | SPI_MEM_SPI_SMEM_DIN3_NUM_M | SPI_MEM_SPI_SMEM_DIN4_NUM_M | SPI_MEM_SPI_SMEM_DIN5_NUM_M | SPI_MEM_SPI_SMEM_DIN6_NUM_M | SPI_MEM_SPI_SMEM_DIN7_NUM_M | SPI_MEM_SPI_SMEM_DINS_NUM_M)))
        | (din_num << SPI_MEM_SPI_SMEM_DIN0_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN1_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN2_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN3_NUM_S)
        | (din_num << SPI_MEM_SPI_SMEM_DIN4_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN5_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN6_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DIN7_NUM_S) | (din_num << SPI_MEM_SPI_SMEM_DINS_NUM_S);
    REG_WRITE(SPI_MEM_SPI_SMEM_DIN_NUM_REG(spi_num), reg_val);
}

/**
 * Set MSPI Octal PSRAM extra dummy
 *
 * @param spi_num      SPI0 / SPI1
 * @param extra_dummy  Extra dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_octal_psram_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (extra_dummy > 0) {
        SET_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, extra_dummy,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    } else {
        CLEAR_PERI_REG_MASK(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_TIMING_CALI_M);
        SET_PERI_REG_BITS(SPI_MEM_SPI_SMEM_TIMING_CALI_REG(spi_num), SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_V, 0,
            SPI_MEM_SPI_SMEM_EXTRA_DUMMY_CYCLELEN_S);
    }
}

/**
 * Set MSPI Octal PSRAM dummy
 *
 * @param spi_num    SPI0 / SPI1
 * @param dummy      dummy
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_set_quad_psram_dummy(uint8_t spi_num, uint8_t dummy)
{
    //HW workaround: Use normal dummy register to set extra dummy, the calibration dedicated extra dummy register doesn't work for quad mode
    SET_PERI_REG_MASK(SPI_MEM_CACHE_SCTRL_REG(spi_num), SPI_MEM_USR_RD_SRAM_DUMMY_M);
    SET_PERI_REG_BITS(SPI_MEM_CACHE_SCTRL_REG(spi_num), SPI_MEM_SRAM_RDUMMY_CYCLELEN_V, dummy, SPI_MEM_SRAM_RDUMMY_CYCLELEN_S);
}

/**
 * Clear MSPI hw fifo
 *
 * @param spi_num    SPI0 / SPI1
 */
__attribute__((always_inline))
static inline void mspi_timing_ll_clear_fifo(uint8_t spi_num)
{
    for (int i = 0; i < 16; i++) {
        REG_WRITE(SPI_MEM_W0_REG(spi_num) + i*4, 0);
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
    return REG_GET_BIT(SPI_MEM_USER_REG(spi_num), SPI_MEM_CS_SETUP);
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
    return REG_GET_FIELD(SPI_MEM_CTRL2_REG(spi_num), SPI_MEM_CS_SETUP_TIME);
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
    return REG_GET_BIT(SPI_MEM_USER_REG(spi_num), SPI_MEM_CS_HOLD);
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
    return REG_GET_FIELD(SPI_MEM_CTRL2_REG(spi_num), SPI_MEM_CS_HOLD_TIME);
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
    return READ_PERI_REG(SPI_MEM_CLOCK_REG(spi_num));
}

#ifdef __cplusplus
}
#endif
