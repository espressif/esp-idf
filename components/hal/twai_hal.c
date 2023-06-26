/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include "sdkconfig.h"
#include "hal/twai_hal.h"
#include "hal/efuse_hal.h"
#include "soc/soc_caps.h"

//Default values written to various registers on initialization
#define TWAI_HAL_INIT_TEC    0
#define TWAI_HAL_INIT_REC    0
#define TWAI_HAL_INIT_EWL    96

/* ---------------------------- Init and Config ----------------------------- */

bool twai_hal_init(twai_hal_context_t *hal_ctx, const twai_hal_config_t *config)
{
    //Initialize HAL context
    hal_ctx->dev = TWAI_LL_GET_HW(config->controller_id);
    hal_ctx->state_flags = 0;
    hal_ctx->clock_source_hz = config->clock_source_hz;
    //Enable functional clock
    twai_ll_enable_clock(hal_ctx->dev, true);
    //Initialize TWAI controller, and set default values to registers
    twai_ll_enter_reset_mode(hal_ctx->dev);
    if (!twai_ll_is_in_reset_mode(hal_ctx->dev)) {    //Must enter reset mode to write to config registers
        return false;
    }
#if SOC_TWAI_SUPPORT_MULTI_ADDRESS_LAYOUT
    twai_ll_enable_extended_reg_layout(hal_ctx->dev);        //Changes the address layout of the registers
#endif
    twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    //Both TEC and REC should start at 0
    twai_ll_set_tec(hal_ctx->dev, TWAI_HAL_INIT_TEC);
    twai_ll_set_rec(hal_ctx->dev, TWAI_HAL_INIT_REC);
    twai_ll_set_err_warn_lim(hal_ctx->dev, TWAI_HAL_INIT_EWL);    //Set default value of for EWL
    return true;
}

void twai_hal_deinit(twai_hal_context_t *hal_ctx)
{
    //Clear any pending registers
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_enabled_intrs(hal_ctx->dev, 0);
    twai_ll_clear_arb_lost_cap(hal_ctx->dev);
    twai_ll_clear_err_code_cap(hal_ctx->dev);
    //Disable functional clock
    twai_ll_enable_clock(hal_ctx->dev, false);
    hal_ctx->dev = NULL;
}

void twai_hal_configure(twai_hal_context_t *hal_ctx, const twai_timing_config_t *t_config, const twai_filter_config_t *f_config, uint32_t intr_mask, uint32_t clkout_divider)
{
    uint32_t brp = t_config->brp;
    // both quanta_resolution_hz and brp can affect the baud rate
    // but a non-zero quanta_resolution_hz takes higher priority
    if (t_config->quanta_resolution_hz) {
        brp = hal_ctx->clock_source_hz / t_config->quanta_resolution_hz;
    }

    // set clock source
    twai_clock_source_t clk_src = t_config->clk_src;
    //for backward compatible, zero value means default a default clock source
    if (t_config->clk_src == 0) {
        clk_src = TWAI_CLK_SRC_DEFAULT;
    }
    twai_ll_set_clock_source(hal_ctx->dev, clk_src);

    //Configure bus timing, acceptance filter, CLKOUT, and interrupts
    twai_ll_set_bus_timing(hal_ctx->dev, brp, t_config->sjw, t_config->tseg_1, t_config->tseg_2, t_config->triple_sampling);
    twai_ll_set_acc_filter(hal_ctx->dev, f_config->acceptance_code, f_config->acceptance_mask, f_config->single_filter);
    twai_ll_set_clkout(hal_ctx->dev, clkout_divider);
    twai_ll_set_enabled_intrs(hal_ctx->dev, intr_mask);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
}

/* -------------------------------- Actions --------------------------------- */

void twai_hal_start(twai_hal_context_t *hal_ctx, twai_mode_t mode)
{
    twai_ll_set_mode(hal_ctx->dev, mode);                //Set operating mode
    //Clear the TEC and REC
    twai_ll_set_tec(hal_ctx->dev, 0);
#ifdef CONFIG_TWAI_ERRATA_FIX_LISTEN_ONLY_DOM
    /*
    Errata workaround: Prevent transmission of dominant error frame while in listen only mode by setting REC to 128
    before exiting reset mode. This forces the controller to be error passive (thus only transmits recessive bits).
    The TEC/REC remain frozen in listen only mode thus ensuring we remain error passive.
    */
    if (mode == TWAI_MODE_LISTEN_ONLY) {
        twai_ll_set_rec(hal_ctx->dev, 128);
    } else
#endif
    {
        twai_ll_set_rec(hal_ctx->dev, 0);
    }
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);    //Clear any latched interrupts
    TWAI_HAL_SET_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_RUNNING);
    twai_ll_exit_reset_mode(hal_ctx->dev);
}

void twai_hal_stop(twai_hal_context_t *hal_ctx)
{
    twai_ll_enter_reset_mode(hal_ctx->dev);
    (void) twai_ll_get_and_clear_intrs(hal_ctx->dev);
    twai_ll_set_mode(hal_ctx->dev, TWAI_MODE_LISTEN_ONLY);    //Freeze REC by changing to LOM mode
    //Any TX is immediately halted on entering reset mode
    TWAI_HAL_CLEAR_BITS(hal_ctx->state_flags, TWAI_HAL_STATE_FLAG_TX_BUFF_OCCUPIED | TWAI_HAL_STATE_FLAG_RUNNING);
}
