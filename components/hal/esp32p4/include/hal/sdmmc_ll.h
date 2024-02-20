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
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "soc/clk_tree_defs.h"
#include "soc/sdmmc_struct.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/lp_clkrst_struct.h"


#ifdef __cplusplus
extern "C" {
#endif

#define SDMMC_LL_GET_HW(id)   (((id) == 0) ? (&SDMMC) : NULL)

/**
 * SDMMC capabilities
 */
#define SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(SLOT_ID)    ((SLOT_ID == 0) ? 0 : 1)

#define SDMMC_LL_IOMUX_FUNC    0

typedef enum {
    SDMMC_LL_DELAY_PHASE_0,
    SDMMC_LL_DELAY_PHASE_1,
    SDMMC_LL_DELAY_PHASE_2,
    SDMMC_LL_DELAY_PHASE_3,
} sdmmc_ll_delay_phase_t;


/**
 * @brief Enable the bus clock for SDMMC module
 *
 * @param hw    hardware instance address
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_bus_clock(sdmmc_dev_t *hw, bool en)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_sdmmc_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the SDMMC module
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_reset_register(sdmmc_dev_t *hw)
{
    LP_AON_CLKRST.hp_sdmmc_emac_rst_ctrl.rst_en_sdmmc = 1;
    LP_AON_CLKRST.hp_sdmmc_emac_rst_ctrl.rst_en_sdmmc = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_reset_register(__VA_ARGS__)

/**
 * @brief Select SDMMC clock source
 *
 * @param hw       hardware instance address
 * @param clk_src  clock source, see valid sources in type `soc_periph_psram_clk_src_t`
 */
static inline void sdmmc_ll_select_clk_source(sdmmc_dev_t *hw, soc_periph_sdmmc_clk_src_t clk_src)
{
    uint32_t clk_val = 0;
    switch (clk_src) {
    case SDMMC_CLK_SRC_PLL160M:
        clk_val = 0;
        break;
    // case SDMMC_CLK_SRC_PLL200M: // TODO: IDF-8886
    //     clk_val = 1;
    //     break;
    default:
        HAL_ASSERT(false);
        break;
    }

    HP_SYS_CLKRST.peri_clk_ctrl01.reg_sdio_ls_clk_src_sel = clk_val;
    HP_SYS_CLKRST.peri_clk_ctrl01.reg_sdio_ls_clk_en = true;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_select_clk_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_select_clk_source(__VA_ARGS__)

/**
 * @brief Set SDMMC clock div
 *
 * @param hw     hardware instance address
 * @param div    divider value
 */
static inline void sdmmc_ll_set_clock_div(sdmmc_dev_t *hw, uint32_t div)
{
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_h = div / 2 - 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_n = div - 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_l = div - 1;

    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_cfg_update = 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_cfg_update = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_set_clock_div(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_set_clock_div(__VA_ARGS__)

/**
 * @brief Deinit clock
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_deinit_clk(sdmmc_dev_t *hw)
{
    hw->clk_edge_sel.val = 0;
}

/**
 * @brief Get SDMMC clock div
 *
 * @param hw    hardware instance address
 *
 * @return      Divider value
 */
static inline uint32_t sdmmc_ll_get_clock_div(sdmmc_dev_t *hw)
{
    return HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_l + 1;
}

/**
 * @brief Initialise the din, dout, self delay phase
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_init_phase_delay(sdmmc_dev_t *hw)
{
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_drv_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_en = 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_slf_clk_en = 1;

    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_drv_clk_edge_sel = 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_edge_sel = 0;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_slf_clk_edge_sel = 0;

    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_cfg_update = 1;
    HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_clk_edge_cfg_update = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_init_phase_delay(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_init_phase_delay(__VA_ARGS__)

/**
 * @brief Set SDMMC din delay
 *
 * @param hw     hardware instance address
 * @param phase  delay phase
 */
static inline void sdmmc_ll_set_din_delay(sdmmc_dev_t *hw, sdmmc_ll_delay_phase_t phase)
{
    switch (phase) {
        case SDMMC_LL_DELAY_PHASE_1:
            HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_edge_sel = 0x1;
            break;
        case SDMMC_LL_DELAY_PHASE_2:
            HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_edge_sel = 0x2;
            break;
        case SDMMC_LL_DELAY_PHASE_3:
            HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_edge_sel = 0x3;
            break;
        default:
            HP_SYS_CLKRST.peri_clk_ctrl02.reg_sdio_ls_sam_clk_edge_sel = 0x0;
            break;
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define sdmmc_ll_set_din_delay(...) (void)__DECLARE_RCC_ATOMIC_ENV; sdmmc_ll_set_din_delay(__VA_ARGS__)

/**
 * @brief Enable card clock
 *
 * @param hw    hardware instance address
 * @param slot  slot
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_card_clock(sdmmc_dev_t *hw, uint32_t slot, bool en)
{
    if (en) {
        hw->clkena.cclk_enable |= BIT(slot);
    } else {
        hw->clkena.cclk_enable &= ~BIT(slot);
    }
}

/**
 * @brief Set card clock div
 *
 * @param hw        hardware instance address
 * @param slot      slot
 * @param card_div  divider value
 */
static inline void sdmmc_ll_set_card_clock_div(sdmmc_dev_t *hw, uint32_t slot, uint32_t card_div)
{
    if (slot == 0) {
        hw->clksrc.card0 = 0;
        hw->clkdiv.clk_divider0 = card_div;
    } else if (slot == 1) {
        hw->clksrc.card1 = 1;
        hw->clkdiv.clk_divider1 = card_div;
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * @brief Get card clock div
 *
 * @param hw        hardware instance address
 * @param slot      slot
 *
 * @return Divider  value
 */
static inline uint32_t sdmmc_ll_get_card_clock_div(sdmmc_dev_t *hw, uint32_t slot)
{
    uint32_t card_div = 0;

    if (slot == 0) {
        HAL_ASSERT(hw->clksrc.card0 == 0);
        card_div = hw->clkdiv.clk_divider0;
    } else if (slot == 1) {
        HAL_ASSERT(hw->clksrc.card1 == 1);
        card_div = hw->clkdiv.clk_divider1;
    } else {
        HAL_ASSERT(false);
    }

    return card_div;
}

/**
 * @brief Disable clock when the card is in IDLE state
 *
 * @param hw    hardware instance address
 * @param slot  slot
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_card_clock_low_power(sdmmc_dev_t *hw, uint32_t slot, bool en)
{
    if (en) {
        hw->clkena.lp_enable |= BIT(slot);
    } else {
        hw->clkena.lp_enable &= ~BIT(slot);
    }
}

/**
 * @brief Set card data read timeout cycles
 *
 * @param hw              hardware instance address
 * @param timeout_cycles  timeout cycles
 */
static inline void sdmmc_ll_set_data_timeout(sdmmc_dev_t *hw, uint32_t timeout_cycles)
{
    if (timeout_cycles > 0xffffff) {
        timeout_cycles = 0xffffff;
    }

    hw->tmout.data_timeout = timeout_cycles;
}

/**
 * @brief Set response timeout cycles (in card output clocks)
 *
 * @param hw              hardware instance address
 * @param timeout_cycles  timeout cycles
 */
static inline void sdmmc_ll_set_response_timeout(sdmmc_dev_t *hw, uint32_t timeout_cycles)
{
    hw->tmout.response_timeout = timeout_cycles;
}

/**
 * @brief Check if card is detected
 *
 * @param hw        hardware instance address
 * @param slot      slot
 *
 * @return True for detected
 */
static inline bool sdmmc_ll_is_card_detected(sdmmc_dev_t *hw, uint32_t slot)
{
    return ((hw->cdetect.card_detect_n & BIT(slot)) == 0);
}

/**
 * @brief Check if card is write protected
 *
 * @param hw        hardware instance address
 * @param slot      slot
 *
 * @return True for write protected
 */
static inline bool sdmmc_ll_is_card_write_protected(sdmmc_dev_t *hw, uint32_t slot)
{
    bool is_protected = hw->wrtprt.write_protect & BIT(slot);
    return is_protected;
}

/**
 * @brief Enable DDR mode
 *
 * @param hw    hardware instance address
 * @param slot  slot
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_ddr_mode(sdmmc_dev_t *hw, uint32_t slot, bool en)
{
    if (en) {
        hw->uhs.ddr |= BIT(slot);
        hw->emmcddr.halfstartbit_reg |= BIT(slot);
    } else {
        hw->uhs.ddr &= ~BIT(slot);
        hw->emmcddr.halfstartbit_reg &= ~BIT(slot);
    }
}

/**
 * @brief Set data transfer length
 *
 * @param hw   hardware instance address
 * @param len  length
 */
static inline void sdmmc_ll_set_data_transfer_len(sdmmc_dev_t *hw, uint32_t len)
{
    hw->bytcnt.byte_count = len;
}

/**
 * @brief Set block size
 *
 * @param hw          hardware instance address
 * @param block_size  block size
 */
static inline void sdmmc_ll_set_block_size(sdmmc_dev_t *hw, uint32_t block_size)
{
    hw->blksiz.block_size = block_size;
}

/**
 * @brief Set descriptor addr
 *
 * @param hw          hardware instance address
 * @param block_size  block size
 */
static inline void sdmmc_ll_set_desc_addr(sdmmc_dev_t *hw, uint32_t desc_addr)
{
    hw->dbaddr.dbaddr_reg = desc_addr;
}

/**
 * @brief Enable DMA
 *
 * @param hw    hardware instance address
 * @param slot  slot
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_dma(sdmmc_dev_t *hw, bool en)
{
    hw->ctrl.dma_enable = en;
    hw->ctrl.use_internal_dma = en;
    hw->bmod.enable = en;
    hw->bmod.fb = en;
}

/**
 * @brief Poll demand
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_poll_demand(sdmmc_dev_t *hw)
{
    hw->pldmnd.pldmnd_pd = 1;
}

/**
 * @brief Get interrupt status
 *
 * @param hw    hardware instance address
 */
static inline uint32_t sdmmc_ll_get_intr_status(sdmmc_dev_t *hw)
{
    return hw->mintsts.val;
}

/**
 * @brief Enable interrupt
 *
 * @param hw    hardware instance address
 * @param mask  interrupt mask
 * @param en    enable / disable
 */
static inline void sdmmc_ll_enable_interrupt(sdmmc_dev_t *hw, uint32_t mask, bool en)
{
    if (en) {
        hw->intmask.val |= mask;
    } else {
        hw->intmask.val &= ~mask;
    }
}

/**
 * @brief Clear interrupt
 *
 * @param hw    hardware instance address
 * @param mask  interrupt mask
 */
static inline void sdmmc_ll_clear_interrupt(sdmmc_dev_t *hw, uint32_t mask)
{
    hw->rintsts.val = mask;
}

#ifdef __cplusplus
}
#endif
