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
#include "soc/system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDMMC_LL_GET_HW(id)   (((id) == 0) ? (&SDMMC) : NULL)

/**
 * SDMMC capabilities
 */
#define SDMMC_LL_SLOT_SUPPORT_GPIO_MATRIX(SLOT_ID)    1

#define SDMMC_LL_IOMUX_FUNC    -1

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
    SYSTEM.perip_clk_en1.sdio_host_clk_en = en;
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
    SYSTEM.perip_rst_en1.sdio_host_rst = 1;
    SYSTEM.perip_rst_en1.sdio_host_rst = 0;
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
        clk_val = 1;
        break;
    case SDMMC_CLK_SRC_XTAL:
        clk_val = 0;
        break;
    default:
        HAL_ASSERT(false);
        break;
    }

    hw->clock.clk_sel = clk_val;
}

/**
 * @brief Set SDMMC clock div
 *
 * @param hw     hardware instance address
 * @param div    divider value
 */
static inline void sdmmc_ll_set_clock_div(sdmmc_dev_t *hw, uint32_t div)
{
    /**
     * Set frequency to 160MHz / div
     *
     * n: counter resets at div_factor_n.
     * l: negedge when counter equals div_factor_l.
     * h: posedge when counter equals div_factor_h.
     *
     * We set the duty cycle to 1/2
     */

    HAL_ASSERT(div > 1 && div <= 16);
    int l = div - 1;
    int h = div / 2 - 1;

    hw->clock.div_factor_h = h;
    hw->clock.div_factor_l = l;
    hw->clock.div_factor_n = l;
}

/**
 * @brief Deinit clock
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_deinit_clk(sdmmc_dev_t *hw)
{
    hw->clock.val = 0;
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
    return hw->clock.div_factor_l + 1;
}

/**
 * @brief Initialise the din, dout, self delay phase
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_init_phase_delay(sdmmc_dev_t *hw)
{
    hw->clock.phase_core = 0;
    /* 90 deg. delay for cclk_out to satisfy large hold time for SDR12 (up to 25MHz) and SDR25 (up to 50MHz) modes.
     * Whether this delayed clock will be used depends on use_hold_reg bit in CMD structure,
     * determined when sending out the command.
     */
    hw->clock.phase_dout = 1;
    hw->clock.phase_din = 0;
}

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
            hw->clock.phase_din = 0x1;
            break;
        case SDMMC_LL_DELAY_PHASE_2:
            hw->clock.phase_din = 0x4;
            break;
        case SDMMC_LL_DELAY_PHASE_3:
            hw->clock.phase_din = 0x6;
            break;
        default:
            hw->clock.phase_din = 0x0;
            break;
    }
}

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
        hw->clkdiv.div0 = card_div;
    } else if (slot == 1) {
        hw->clksrc.card1 = 1;
        hw->clkdiv.div1 = card_div;
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
        card_div = hw->clkdiv.div0;
    } else if (slot == 1) {
        HAL_ASSERT(hw->clksrc.card1 == 1);
        card_div = hw->clkdiv.div1;
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
        hw->clkena.cclk_low_power |= BIT(slot);
    } else {
        hw->clkena.cclk_low_power &= ~BIT(slot);
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

    hw->tmout.data = timeout_cycles;
}

/**
 * @brief Set response timeout cycles (in card output clocks)
 *
 * @param hw              hardware instance address
 * @param timeout_cycles  timeout cycles
 */
static inline void sdmmc_ll_set_response_timeout(sdmmc_dev_t *hw, uint32_t timeout_cycles)
{
    hw->tmout.response = timeout_cycles;
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
    return ((hw->cdetect.cards & BIT(slot)) == 0);
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
    bool is_protected = hw->wrtprt.cards & BIT(slot);
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
        hw->emmc_ddr_reg |= BIT(slot);
    } else {
        hw->uhs.ddr &= ~BIT(slot);
        hw->emmc_ddr_reg &= ~BIT(slot);
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
    hw->bytcnt = len;
}

/**
 * @brief Set block size
 *
 * @param hw          hardware instance address
 * @param block_size  block size
 */
static inline void sdmmc_ll_set_block_size(sdmmc_dev_t *hw, uint32_t block_size)
{
    hw->blksiz = block_size;
}

/**
 * @brief Set descriptor addr
 *
 * @param hw          hardware instance address
 * @param block_size  block size
 */
static inline void sdmmc_ll_set_desc_addr(sdmmc_dev_t *hw, uint32_t desc_addr)
{
    hw->dbaddr = (sdmmc_desc_t *)desc_addr;
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
    hw->pldmnd = 1;
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
