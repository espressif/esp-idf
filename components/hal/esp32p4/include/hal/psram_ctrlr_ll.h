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
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/spi_mem_s_struct.h"
#include "soc/spi_mem_s_reg.h"
#include "soc/spi1_mem_s_reg.h"
#include "soc/spi1_mem_s_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/clk_tree_defs.h"
#include "rom/opi_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PSRAM_CTRLR_LL_MSPI_ID_2            2
#define PSRAM_CTRLR_LL_MSPI_ID_3            3

#define PSRAM_CTRLR_LL_PMS_REGION_NUMS      4
#define PSRAM_CTRLR_LL_PMS_ATTR_WRITABLE    (1<<0)
#define PSRAM_CTRLR_LL_PMS_ATTR_READABLE    (1<<1)

#define PSRAM_CTRLR_LL_FIFO_MAX_BYTES       64


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
    SPIMEM2.mem_cache_sctrl.mem_cache_sram_usr_wcmd = 1;
    SPIMEM2.mem_sram_dwr_cmd.mem_cache_sram_usr_wr_cmd_bitlen = cmd_bitlen - 1;
    SPIMEM2.mem_sram_dwr_cmd.mem_cache_sram_usr_wr_cmd_value = cmd_val;
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
    SPIMEM2.mem_cache_sctrl.mem_cache_sram_usr_rcmd = 1;
    SPIMEM2.mem_sram_drd_cmd.mem_cache_sram_usr_rd_cmd_bitlen = cmd_bitlen - 1;
    SPIMEM2.mem_sram_drd_cmd.mem_cache_sram_usr_rd_cmd_value = cmd_val;
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
    SPIMEM2.mem_cache_sctrl.mem_sram_addr_bitlen = addr_bitlen - 1;
}

/**
 * @brief Enable PSRAM 4B addr
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_4byte_addr(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_cache_sctrl.mem_cache_usr_saddr_4byte = en;
}

/**
 * @brief Set PSRAM write dummy
 *
 * @param mspi_id      mspi_id
 * @param dummy_n      dummy number
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_wr_dummy(uint32_t mspi_id, uint32_t dummy_n)
{
    (void)mspi_id;
    HAL_ASSERT(dummy_n > 0);
    SPIMEM2.mem_cache_sctrl.mem_usr_wr_sram_dummy = 1;
    SPIMEM2.mem_cache_sctrl.mem_sram_wdummy_cyclelen = dummy_n - 1;
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
    SPIMEM2.mem_cache_sctrl.mem_usr_rd_sram_dummy = 1;
    SPIMEM2.mem_cache_sctrl.mem_sram_rdummy_cyclelen = dummy_n - 1;
}

/**
 * @brief Enable PSRAM variable dummy
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_variable_dummy(uint32_t mspi_id, bool en)
{
    if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_2) {
        SPIMEM2.smem_ddr.smem_var_dummy = en;
    } else if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_3) {
        SPIMEM3.ddr.fmem_var_dummy = en;
    }
}

/**
 * @brief Enable PSRAM write dummy level control
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_wr_dummy_level_control(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_sram_cmd.mem_sdummy_wout = en;
}

/**
 * @brief Enable PSRAM read dummy level control
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_rd_dummy_level_control(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_sram_cmd.mem_sdummy_rin = en;
}

/**
 * @brief Enable PSRAM ddr mode
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_ddr_mode(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ddr.smem_ddr_en = en;
}

/**
 * @brief Enable PSRAM ddr write data swap
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_ddr_wr_data_swap(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ddr.smem_ddr_wdat_swp = en;
}

/**
 * @brief Enable PSRAM ddr read data swap
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_ddr_rd_data_swap(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ddr.smem_ddr_rdat_swp = en;
}

/**
 * @brief Enable PSRAM octal mode
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_oct_line_mode(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_cache_sctrl.mem_sram_oct = en;
    SPIMEM2.mem_sram_cmd.mem_scmd_oct = en;
    SPIMEM2.mem_sram_cmd.mem_saddr_oct = en;
    SPIMEM2.mem_sram_cmd.mem_sdout_oct = en;
    SPIMEM2.mem_sram_cmd.mem_sdin_oct = en;
}

/**
 * @brief Enable PSRAM hex data line mode
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_hex_data_line_mode(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_sram_cmd.mem_sdin_hex = en;
    SPIMEM2.mem_sram_cmd.mem_sdout_hex = en;
}

/**
 * @brief Enable PSRAM AXI master access
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_axi_access(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_cache_fctrl.mem_axi_req_en = en;
    SPIMEM2.mem_cache_fctrl.close_axi_inf_en = !en;
}

/**
 * @brief Enable PSRAM write splice transfer
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_wr_splice(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_ctrl1.mem_aw_splice_en = en;
}

/**
 * @brief Enable PSRAM read splice transfer
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_rd_splice(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.mem_ctrl1.mem_ar_splice_en = en;
}

/**
 * @brief Enable PSRAM module clock
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_module_clock(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    HP_SYS_CLKRST.soc_clk_ctrl0.reg_psram_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define psram_ctrlr_ll_enable_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; psram_ctrlr_ll_enable_module_clock(__VA_ARGS__)

/**
 * @brief Reset PSRAM module clock
 *
 * @param mspi_id      mspi_id
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_reset_module_clock(uint32_t mspi_id)
{
    (void)mspi_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dual_mspi_axi = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dual_mspi_axi = 0;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dual_mspi_apb = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dual_mspi_apb = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define psram_ctrlr_ll_reset_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; psram_ctrlr_ll_reset_module_clock(__VA_ARGS__)

/**
 * @brief Select PSRAM clock source
 *
 * @param mspi_id      mspi_id
 * @param clk_src      clock source, see valid sources in type `soc_periph_psram_clk_src_t`
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_select_clk_source(uint32_t mspi_id, soc_periph_psram_clk_src_t clk_src)
{
    (void)mspi_id;
    uint32_t clk_val = 0;
    switch (clk_src) {
    case PSRAM_CLK_SRC_XTAL:
        clk_val = 0;
        break;
    case PSRAM_CLK_SRC_MPLL:
        clk_val = 1;
        break;
    case PSRAM_CLK_SRC_SPLL:
        clk_val = 2;
        break;
    case PSRAM_CLK_SRC_CPLL:
        clk_val = 3;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    HP_SYS_CLKRST.peri_clk_ctrl00.reg_psram_pll_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_psram_clk_src_sel = clk_val;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define psram_ctrlr_ll_select_clk_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; psram_ctrlr_ll_select_clk_source(__VA_ARGS__)

/**
 * @brief Set PSRAM core clock
 *
 * @param mspi_id  mspi_id
 * @param freqdiv  Divider value
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_core_clock(uint8_t spi_num, uint32_t freqdiv)
{
    HP_SYS_CLKRST.peri_clk_ctrl00.reg_psram_core_clk_en = 1;
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl00, reg_psram_core_clk_div_num, freqdiv - 1);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define psram_ctrlr_ll_set_core_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; psram_ctrlr_ll_set_core_clock(__VA_ARGS__)

/**
 * @brief Set PSRAM bus clock
 *
 * @param mspi_id  mspi_id
 * @param freqdiv  Divider value
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_bus_clock(uint32_t mspi_id, uint32_t freqdiv)
{
    if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_2) {
        if (freqdiv == 1) {
            WRITE_PERI_REG(SPI_MEM_S_SRAM_CLK_REG, SPI_MEM_S_SCLK_EQU_SYSCLK);
        } else {
            uint32_t freqbits = (((freqdiv - 1) << SPI_MEM_S_SCLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI_MEM_S_SCLKCNT_H_S)) | ((freqdiv - 1) << SPI_MEM_S_SCLKCNT_L_S);
            WRITE_PERI_REG(SPI_MEM_S_SRAM_CLK_REG, freqbits);
        }
    } else if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_3) {
        if (freqdiv == 1) {
            WRITE_PERI_REG(SPI1_MEM_S_CLOCK_REG, SPI1_MEM_S_CLK_EQU_SYSCLK);
        } else {
            uint32_t freqbits = (((freqdiv - 1) << SPI1_MEM_S_CLKCNT_N_S)) | (((freqdiv / 2 - 1) << SPI1_MEM_S_CLKCNT_H_S)) | ((freqdiv - 1) << SPI1_MEM_S_CLKCNT_L_S);
            WRITE_PERI_REG(SPI1_MEM_S_CLOCK_REG, freqbits);
        }
    }
}

/**
 * @brief Enable PSRAM DLL
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_dll(uint32_t mspi_id, bool en)
{
    if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_2) {
        SPIMEM2.smem_timing_cali.smem_dll_timing_cali = en;
    } else if (mspi_id == PSRAM_CTRLR_LL_MSPI_ID_3) {
        SPIMEM2.mem_timing_cali.mem_dll_timing_cali = en;
    } else {
        HAL_ASSERT(false);
    }
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
    SPIMEM2.smem_ac.smem_cs_setup = 1;
    SPIMEM2.smem_ac.smem_cs_setup_time = setup_n - 1;
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
    SPIMEM2.smem_ac.smem_cs_hold = 1;
    SPIMEM2.smem_ac.smem_cs_hold_time = hold_n - 1;
}

/**
 * @brief Set CS hold delay
 *
 * @param mspi_id      mspi_id
 * @param hold_n       cs hold time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_hold_delay(uint32_t mspi_id, uint32_t hold_delay_n)
{
    (void)mspi_id;
    HAL_ASSERT(hold_delay_n > 0);
    SPIMEM2.smem_ac.smem_cs_hold_delay = hold_delay_n - 1;
}

/**
 * @brief Set ECC CS hold
 *
 * @param mspi_id      mspi_id
 * @param hold_n       cs hold time
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_cs_hold_ecc(uint32_t mspi_id, uint32_t hold_n)
{
    (void)mspi_id;
    HAL_ASSERT(hold_n > 0);
    SPIMEM2.smem_ac.smem_ecc_cs_hold_time = hold_n - 1;
}

/**
 * @brief Enable 16to18 ECC
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_16to18_ecc(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ac.smem_ecc_16to18_byte_en = en;
}

/**
 * @brief Enable ECC skip page corner
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_skip_page_corner(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ac.smem_ecc_skip_page_corner = en;
}

/**
 * @brief Enable spliting transactions
 *
 * @param mspi_id      mspi_id
 * @param en           enable / disable
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_enable_split_trans(uint32_t mspi_id, bool en)
{
    (void)mspi_id;
    SPIMEM2.smem_ac.smem_split_trans_en = en;
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
    SPIMEM2.smem_ecc_ctrl.smem_ecc_addr_en = en;
}

/**
 * @brief Set page size
 *
 * @param mspi_id      mspi_id
 * @param page_size    page size
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_set_page_size(uint32_t mspi_id, uint32_t page_size)
{
    (void)mspi_id;
    switch(page_size) {
    case 256:
        SPIMEM2.smem_ecc_ctrl.smem_page_size = 0;
        break;
    case 512:
        SPIMEM2.smem_ecc_ctrl.smem_page_size = 1;
        break;
    case 1024:
        SPIMEM2.smem_ecc_ctrl.smem_page_size = 2;
        break;
    case 2048:
        SPIMEM2.smem_ecc_ctrl.smem_page_size = 3;
        break;
    default:
        HAL_ASSERT(false);
    }
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

    uint32_t reg_val = SPIMEM2.smem_ecc_ctrl.smem_page_size;
    switch(reg_val) {
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
    SPIMEM2.smem_pmsn_attr[region_id].smem_pms_ecc = en;
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
    SPIMEM2.smem_pmsn_attr[region_id].smem_pms_wr_attr = 0;
    SPIMEM2.smem_pmsn_attr[region_id].smem_pms_rd_attr = 0;
    if (attr_mask & PSRAM_CTRLR_LL_PMS_ATTR_WRITABLE) {
        SPIMEM2.smem_pmsn_attr[region_id].smem_pms_wr_attr = 1;
    }
    if (attr_mask & PSRAM_CTRLR_LL_PMS_ATTR_READABLE) {
        SPIMEM2.smem_pmsn_attr[region_id].smem_pms_rd_attr = 1;
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
    SPIMEM2.smem_pmsn_addr[region_id].smem_pms_addr_s = addr;
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
    SPIMEM2.smem_pmsn_size[region_id].smem_pms_size = size;
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
    return SPIMEM2.smem_pmsn_addr[region_id].smem_pms_addr_s;
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
    return SPIMEM2.smem_pmsn_size[region_id].smem_pms_size;
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
    esp_rom_spi_set_op_mode(mspi_id, mode);
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
 * @brief PSRAM common transaction
 *
 * See `opi_flash.h` for parameters
 */
__attribute__((always_inline))
static inline void psram_ctrlr_ll_common_transaction(uint32_t mspi_id,
                                                     uint32_t cmd, uint32_t cmd_bitlen,
                                                     uint32_t addr, uint32_t addr_bitlen,
                                                     uint32_t dummy_bits,
                                                     uint8_t* mosi_data, uint32_t mosi_bitlen,
                                                     uint8_t* miso_data, uint32_t miso_bitlen,
                                                     bool is_write_erase_operation)
{
    esp_rom_spiflash_read_mode_t mode = ESP_ROM_SPIFLASH_OPI_DTR_MODE;
    uint32_t cs_mask = 1<<1;
    psram_ctrlr_ll_common_transaction_base(mspi_id, mode, cmd, cmd_bitlen, addr, addr_bitlen, dummy_bits,
                                           mosi_data, mosi_bitlen, miso_data, miso_bitlen, cs_mask,
                                           is_write_erase_operation);
}

#ifdef __cplusplus
}
#endif
