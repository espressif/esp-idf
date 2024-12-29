/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
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
#include <string.h>
#include "esp_bit_defs.h"
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/sd_types.h"
#include "soc/clk_tree_defs.h"
#include "soc/sdmmc_struct.h"
#include "soc/sdmmc_reg.h"
#include "soc/system_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SDMMC_LL_GET_HW(id)   (((id) == 0) ? (&SDMMC) : NULL)

#define SDMMC_LL_EVENT_IO_SLOT1  (1<<17)
#define SDMMC_LL_EVENT_IO_SLOT0  (1<<16)
#define SDMMC_LL_EVENT_EBE       (1<<15)
#define SDMMC_LL_EVENT_ACD       (1<<14)
#define SDMMC_LL_EVENT_SBE       (1<<13)
#define SDMMC_LL_EVENT_BCI       (1<<13)
#define SDMMC_LL_EVENT_HLE       (1<<12)
#define SDMMC_LL_EVENT_FRUN      (1<<11)
#define SDMMC_LL_EVENT_HTO       (1<<10)
#define SDMMC_LL_EVENT_DTO       (1<<9)
#define SDMMC_LL_EVENT_RTO       (1<<8)
#define SDMMC_LL_EVENT_DCRC      (1<<7)
#define SDMMC_LL_EVENT_RCRC      (1<<6)
#define SDMMC_LL_EVENT_RXDR      (1<<5)
#define SDMMC_LL_EVENT_TXDR      (1<<4)
#define SDMMC_LL_EVENT_DATA_OVER (1<<3)
#define SDMMC_LL_EVENT_CMD_DONE  (1<<2)
#define SDMMC_LL_EVENT_RESP_ERR  (1<<1)
#define SDMMC_LL_EVENT_CD        (1<<0)

/* Default disabled interrupts (on init):
 *  SDMMC_LL_EVENT_RXDR,
 *  SDMMC_LL_EVENT_TXDR,
 *  SDMMC_LL_EVENT_BCI,
 *  SDMMC_LL_EVENT_ACD,
 *  SDMMC_LL_EVENT_IO_SLOT1,
 *  SDMMC_LL_EVENT_IO_SLOT0
 */
// Default enabled interrupts (sdio is enabled only when use):
#define SDMMC_LL_EVENT_DEFAULT \
    (SDMMC_LL_EVENT_CD | SDMMC_LL_EVENT_RESP_ERR | SDMMC_LL_EVENT_CMD_DONE | SDMMC_LL_EVENT_DATA_OVER | \
    SDMMC_LL_EVENT_RCRC | SDMMC_LL_EVENT_DCRC | SDMMC_LL_EVENT_RTO | SDMMC_LL_EVENT_DTO | SDMMC_LL_EVENT_HTO | \
    SDMMC_LL_EVENT_HLE | \
    SDMMC_LL_EVENT_SBE | \
    SDMMC_LL_EVENT_EBE)

#define SDMMC_LL_SD_EVENT_MASK \
    (SDMMC_LL_EVENT_CD | SDMMC_LL_EVENT_RESP_ERR | SDMMC_LL_EVENT_CMD_DONE | SDMMC_LL_EVENT_DATA_OVER | \
    SDMMC_LL_EVENT_TXDR | SDMMC_LL_EVENT_RXDR |\
    SDMMC_LL_EVENT_RCRC | SDMMC_LL_EVENT_DCRC | SDMMC_LL_EVENT_RTO | SDMMC_LL_EVENT_DTO | SDMMC_LL_EVENT_HTO | \
    SDMMC_LL_EVENT_FRUN | SDMMC_LL_EVENT_HLE |\
    SDMMC_LL_EVENT_SBE | SDMMC_LL_EVENT_ACD |\
    SDMMC_LL_EVENT_EBE)

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


/*---------------------------------------------------------------
                    Clock & Reset
---------------------------------------------------------------*/
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
    uint32_t reg_val = HAL_FORCE_READ_U32_REG_FIELD(hw->clkena, cclk_enable);
    if (en) {
        reg_val |= BIT(slot);
    } else {
        reg_val &= ~BIT(slot);
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clkena, cclk_enable, reg_val);
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
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clkdiv, div0, card_div);
    } else if (slot == 1) {
        hw->clksrc.card1 = 1;
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clkdiv, div1, card_div);
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
        card_div = HAL_FORCE_READ_U32_REG_FIELD(hw->clkdiv, div0);
    } else if (slot == 1) {
        HAL_ASSERT(hw->clksrc.card1 == 1);
        card_div = HAL_FORCE_READ_U32_REG_FIELD(hw->clkdiv, div1);
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
    uint32_t reg_val = HAL_FORCE_READ_U32_REG_FIELD(hw->clkena, cclk_low_power);
    if (en) {
        reg_val |= BIT(slot);
    } else {
        reg_val &= ~BIT(slot);
    }
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->clkena, cclk_low_power, reg_val);
}

/**
 * @brief Reset controller
 *
 * @note Self clear after two AHB clock cycles, needs wait done
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_reset_controller(sdmmc_dev_t *hw)
{
    hw->ctrl.controller_reset = 1;
}

/**
 * @brief Get if controller reset is done
 *
 * @param hw    hardware instance address
 *
 * @return true: done; false: not done
 */
static inline bool sdmmc_ll_is_controller_reset_done(sdmmc_dev_t *hw)
{
    return hw->ctrl.controller_reset == 0;
}

/**
 * @brief Reset DMA
 *
 * @note Self clear after two AHB clock cycles, needs wait done
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_reset_dma(sdmmc_dev_t *hw)
{
    hw->ctrl.dma_reset = 1;
}

/**
 * @brief Get if dma reset is done
 *
 * @param hw    hardware instance address
 *
 * @return true: done; false: not done
 */
static inline bool sdmmc_ll_is_dma_reset_done(sdmmc_dev_t *hw)
{
    return hw->ctrl.dma_reset == 0;
}

/**
 * @brief Reset fifo
 *
 * @note Self clear after reset done, needs wait done
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_reset_fifo(sdmmc_dev_t *hw)
{
    hw->ctrl.fifo_reset = 1;
}

/**
 * @brief Get if fifo reset is done
 *
 * @param hw    hardware instance address
 *
 * @return true: done; false: not done
 */
static inline bool sdmmc_ll_is_fifo_reset_done(sdmmc_dev_t *hw)
{
    return hw->ctrl.fifo_reset == 0;
}

/*---------------------------------------------------------------
                      MISC
---------------------------------------------------------------*/
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->tmout, response, timeout_cycles);
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
 * @brief Switch between 3.3V and 1.8V mode
 *
 * @param hw    hardware instance address
 * @param slot  slot
 * @param en    enable / disable 1.8V (3.3V on disable)
 */
static inline void sdmmc_ll_enable_1v8_mode(sdmmc_dev_t *hw, uint32_t slot, bool en)
{
    //for compatibility
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
    uint32_t ddr_reg_val = HAL_FORCE_READ_U32_REG_FIELD(hw->uhs, ddr);
    if (en) {
        ddr_reg_val|= BIT(slot);
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->uhs, ddr, ddr_reg_val);
        hw->emmc_ddr_reg |= BIT(slot);
    } else {
        ddr_reg_val&= ~BIT(slot);
        HAL_FORCE_MODIFY_U32_REG_FIELD(hw->uhs, ddr, ddr_reg_val);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->blksiz, block_size, block_size);
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
 * @brief Poll demand
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_poll_demand(sdmmc_dev_t *hw)
{
    hw->pldmnd = 1;
}

/**
 * @brief Set command
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_set_command(sdmmc_dev_t *hw, sdmmc_hw_cmd_t cmd)
{
    memcpy((void *)&hw->cmd, &cmd, sizeof(sdmmc_hw_cmd_t));
}

/**
 * @brief Get if command is taken by CIU
 *
 * @param hw    hardware instance address
 *
 * @return 1: is taken; 0: not taken, should not write to any command regs
 */
static inline bool sdmmc_ll_is_command_taken(sdmmc_dev_t *hw)
{
    return hw->cmd.start_command == 0;
}

/**
 * @brief Set command argument
 *
 * @param hw    hardware instance address
 * @param arg   value indicates command argument to be passed to card
 */
static inline void sdmmc_ll_set_command_arg(sdmmc_dev_t *hw, uint32_t arg)
{
    hw->cmdarg = arg;
}

/**
 * @brief Get version ID
 *
 * @param hw    hardware instance address
 *
 * @return version ID
 */
static inline uint32_t sdmmc_ll_get_version_id(sdmmc_dev_t *hw)
{
    return hw->verid;
}

/**
 * @brief Get hardware configuration info
 *
 * @param hw    hardware instance address
 *
 * @return hardware configurations
 */
static inline uint32_t sdmmc_ll_get_hw_config_info(sdmmc_dev_t *hw)
{
    return hw->hcon.val;
}

/**
 * @brief Set card width
 *
 * @param hw    hardware instance address
 * @param slot  slot ID
 * @param width card width
 */
static inline void sdmmc_ll_set_card_width(sdmmc_dev_t *hw, uint32_t slot, sd_bus_width_t width)
{
    uint16_t mask = 1 << slot;
    uint32_t reg_val = HAL_FORCE_READ_U32_REG_FIELD(hw->ctype, card_width);
    uint32_t reg_val_8 = HAL_FORCE_READ_U32_REG_FIELD(hw->ctype, card_width_8);

    switch (width) {
    case SD_BUS_WIDTH_1_BIT:
        reg_val_8 &= ~mask;
        reg_val &= ~mask;
        break;
    case SD_BUS_WIDTH_4_BIT:
        reg_val_8 &= ~mask;
        reg_val |= mask;
        break;
    case SD_BUS_WIDTH_8_BIT:
        reg_val_8 |= mask;
        break;
    default:
        HAL_ASSERT(false);
    }

    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->ctype, card_width, reg_val);
    HAL_FORCE_MODIFY_U32_REG_FIELD(hw->ctype, card_width_8, reg_val_8);
}

/**
 * @brief Is card data busy
 *
 * @param hw    hardware instance address
 *
 * @return 1: busy; 0: idle
 */
static inline bool sdmmc_ll_is_card_data_busy(sdmmc_dev_t *hw)
{
    return hw->status.data_busy == 1;
}

/*---------------------------------------------------------------
                      DMA
---------------------------------------------------------------*/
/**
 * @brief Init DMA
 *        - enable dma
 *        - clear bus mode reg and reset all dmac internal regs
 *        - enable internal dmac interrupt
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_init_dma(sdmmc_dev_t *hw)
{
    hw->ctrl.dma_enable = 1;
    hw->bmod.val = 0;
    hw->bmod.sw_reset = 1;
    hw->idinten.ni = 1;
    hw->idinten.ri = 1;
    hw->idinten.ti = 1;
}

/**
 * @brief Enable DMA
 *
 * @param hw    hardware instance address
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
 * @brief Stop DMA
 *
 * @param hw    hardware instance address
 */
static inline void sdmmc_ll_stop_dma(sdmmc_dev_t *hw)
{
    hw->ctrl.use_internal_dma = 0;
    hw->ctrl.dma_reset = 1;     //here might be an issue as we don't wait the `dma_reset` to be self-cleared, check in next steps
    hw->bmod.fb = 0;
    hw->bmod.enable = 0;
}

/*---------------------------------------------------------------
                      INTR
---------------------------------------------------------------*/
/**
 * @brief Get masked interrupt-status register value
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
 * @brief Get RAW interrupt-status register value
 */
static inline uint32_t sdmmc_ll_get_interrupt_raw(sdmmc_dev_t *hw)
{
    return hw->rintsts.val;
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

/**
 * @brief Enable / disable interrupts globally
 *
 * @param hw  hardware instance address
 * @param en  enable / disable
 */
static inline void sdmmc_ll_enable_global_interrupt(sdmmc_dev_t *hw, bool en)
{
    hw->ctrl.int_enable = (uint32_t)en;
}


/**
 * @brief Enable / disable busy clear interrupt
 *
 * @param hw  hardware instance address
 * @param en  enable / disable
 */
static inline void sdmmc_ll_enable_busy_clear_interrupt(sdmmc_dev_t *hw, bool en)
{
    hw->cardthrctl.busy_clr_int_en = en;
}

/**
 * @brief Get internal dmac status register val
 */
static inline uint32_t sdmmc_ll_get_idsts_interrupt_raw(sdmmc_dev_t *hw)
{
    return hw->idsts.val;
}

/**
 * @brief Clear internal dmac status register events
 *
 * @param hw    hardware instance address
 * @param mask  interrupt mask
 */
static inline void sdmmc_ll_clear_idsts_interrupt(sdmmc_dev_t *hw, uint32_t mask)
{
    hw->idsts.val = mask;
}

#ifdef __cplusplus
}
#endif
