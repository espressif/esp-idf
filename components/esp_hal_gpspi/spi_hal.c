/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// The HAL layer for SPI (common part)

#include "hal/spi_hal.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"

void spi_hal_init(spi_hal_context_t *hal, uint32_t host_id)
{
    memset(hal, 0, sizeof(spi_hal_context_t));
    spi_dev_t *hw = SPI_LL_GET_HW(host_id);
    hal->hw = hw;
    spi_ll_master_init(hw);

    //Force a transaction done interrupt. This interrupt won't fire yet because
    //we initialized the SPI interrupt as disabled. This way, we can just
    //enable the SPI interrupt and the interrupt handler will kick in, handling
    //any transactions that are queued.
    spi_ll_enable_int(hw);
    spi_ll_set_int_stat(hw);
    spi_ll_set_mosi_delay(hw, 0, 0);
    spi_ll_apply_config(hw);
}

void spi_hal_set_data_pin_idle_level(spi_hal_context_t *hal, bool level)
{
#if SPI_LL_MOSI_FREE_LEVEL
    // Config default output data line level when don't have transaction
    spi_ll_set_data_pin_idle_level(hal->hw, level);
    spi_ll_apply_config(hal->hw);
#endif
}

void spi_hal_deinit(spi_hal_context_t *hal)
{
    spi_dev_t *hw = hal->hw;
    if (hw) {
        spi_ll_disable_int(hw);
        spi_ll_clear_int_stat(hw);
    }
}

#ifdef SOC_SPI_SCT_SUPPORTED
void spi_hal_sct_init(spi_hal_context_t *hal)
{
    spi_ll_conf_state_enable(hal->hw, true);
    spi_ll_set_magic_number(hal->hw, SPI_LL_SCT_MAGIC_NUMBER);
    spi_ll_disable_int(hal->hw);    //trans_done intr enabled in `add device` phase, sct mode should use sct_trans_done only
    spi_ll_enable_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_set_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
}

void spi_hal_sct_deinit(spi_hal_context_t *hal)
{
    spi_ll_conf_state_enable(hal->hw, false);
    spi_ll_disable_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_clear_intr(hal->hw, SPI_LL_INTR_SEG_DONE);
    spi_ll_clear_int_stat(hal->hw);
    spi_ll_enable_int(hal->hw); //recover trans_done intr
}
#endif  //#ifdef SOC_SPI_SCT_SUPPORTED

int spi_hal_master_cal_clock(int fapb, int hz, int duty_cycle)
{
    return spi_ll_master_cal_clock(fapb, hz, duty_cycle, NULL);
}
