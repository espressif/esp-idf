/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * RISC-V trace encoder HAL: register-sequencing layer used by the
 * `esp_riscv_trace` driver. It sits on top of the target-specific LL
 * (hal/riscv_trace_ll.h), which performs the raw register accesses.
 *
 * These interfaces are internal to ESP-IDF and subject to change. The HAL
 * performs no locking: callers that use it directly are responsible for
 * serializing concurrent access to the same trace encoder instance.
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "esp_rom_sys.h"
#include "soc/soc_caps.h"
#include "hal/riscv_trace_hal.h"
#include "hal/assert.h"
#include "hal/riscv_trace_ll.h"

/* Keep the chip-independent HAL interrupt flags in sync with this target's register layout. */
_Static_assert(RISCV_TRACE_INTR_FIFO_OVERFLOW == TRACE_FIFO_OVERFLOW_INTR_ENA,
               "RISCV_TRACE_INTR_FIFO_OVERFLOW does not match TRACE_FIFO_OVERFLOW_INTR_ENA");
_Static_assert(RISCV_TRACE_INTR_MEM_FULL == TRACE_MEM_FULL_INTR_ENA,
               "RISCV_TRACE_INTR_MEM_FULL does not match TRACE_MEM_FULL_INTR_ENA");

void riscv_trace_hal_init(int core_id, const riscv_trace_hal_config_t *config, riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL && config != NULL);
    HAL_ASSERT(config->mem_end_addr > config->mem_start_addr);

    ctx->dev = riscv_trace_ll_get_hw(core_id);
    ctx->core_id = core_id;

    /* Memory configuration */
    riscv_trace_ll_set_mem_start_addr(ctx->dev, config->mem_start_addr);
    riscv_trace_ll_set_mem_end_addr(ctx->dev, config->mem_end_addr);
    riscv_trace_ll_update_mem_current_addr(ctx->dev);

    /* Trace configuration */
    riscv_trace_ll_set_mem_loop(ctx->dev, config->mem_loop);
    riscv_trace_ll_set_restart_ena(ctx->dev, config->auto_restart);
    riscv_trace_ll_set_resync_mode(ctx->dev, config->resync_mode);
    riscv_trace_ll_set_resync_threshold(ctx->dev, config->resync_threshold);
#if SOC_RISCV_TRACE_HAS_CONFIG_REG
    riscv_trace_ll_set_full_address(ctx->dev, config->full_address);
    riscv_trace_ll_set_stall_ena(ctx->dev, config->stall_cpu);
    riscv_trace_ll_set_halt_ena(ctx->dev, config->halt_enable);
    riscv_trace_ll_set_reset_ena(ctx->dev, config->reset_enable);
    riscv_trace_ll_set_dm_trigger_ena(ctx->dev, config->debug_trigger_enable);
#endif
#if SOC_RISCV_TRACE_AHB_CONFIGURABLE
    riscv_trace_ll_set_ahb_burst(ctx->dev, config->ahb_burst);
    riscv_trace_ll_set_ahb_max_incr(ctx->dev, config->ahb_max_incr);
#endif

    /* Interrupts */
    riscv_trace_ll_set_intr_ena(ctx->dev, config->intr_mask);
}

void riscv_trace_hal_start(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    riscv_trace_ll_trigger_on(ctx->dev);
}

void riscv_trace_hal_set_auto_restart(riscv_trace_hal_context_t *ctx, bool enable)
{
    HAL_ASSERT(ctx != NULL);
    riscv_trace_ll_set_restart_ena(ctx->dev, enable);
}

void riscv_trace_hal_set_intr_enable(riscv_trace_hal_context_t *ctx, uint32_t mask)
{
    HAL_ASSERT(ctx != NULL);
    riscv_trace_ll_set_intr_ena(ctx->dev, mask);
}

bool riscv_trace_hal_stop(riscv_trace_hal_context_t *ctx, uint32_t timeout_us)
{
    HAL_ASSERT(ctx != NULL);

    /* Clear auto-restart before the off-trigger so the encoder cannot
     * restart itself while the FIFO is emptying. */
    riscv_trace_hal_set_auto_restart(ctx, false);
    riscv_trace_ll_trigger_off(ctx->dev);

    const uint32_t poll_step_us = 10;
    uint64_t waited_us = 0;
    while (!riscv_trace_hal_fifo_is_empty(riscv_trace_hal_read_fifo_status(ctx))) {
        if (waited_us >= timeout_us) {
            return false;
        }
        esp_rom_delay_us(poll_step_us);
        waited_us += poll_step_us;
    }
    return true;
}

void riscv_trace_hal_deinit(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    riscv_trace_ll_set_restart_ena(ctx->dev, false);
    riscv_trace_ll_trigger_off(ctx->dev);
    riscv_trace_ll_set_intr_ena(ctx->dev, 0);
}

uint32_t riscv_trace_hal_read_fifo_status(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    return riscv_trace_ll_get_fifo_status(ctx->dev);
}

uint32_t riscv_trace_hal_get_current_addr(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    return riscv_trace_ll_get_mem_current_addr(ctx->dev);
}

uint32_t riscv_trace_hal_read_intr_raw(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    return riscv_trace_ll_get_intr_raw(ctx->dev);
}

bool riscv_trace_hal_fifo_is_empty(uint32_t fifo_status)
{
    return (fifo_status & TRACE_FIFO_EMPTY_M) != 0;
}

riscv_trace_work_status_t riscv_trace_hal_get_work_status(uint32_t fifo_status)
{
    return (riscv_trace_work_status_t)((fifo_status & TRACE_WORK_STATUS_M) >> TRACE_WORK_STATUS_S);
}

bool riscv_trace_hal_memory_is_full(uint32_t intr_status)
{
    return (intr_status & TRACE_MEM_FULL_INTR_RAW_M) != 0;
}

bool riscv_trace_hal_fifo_is_overflowed(uint32_t intr_status)
{
    return (intr_status & TRACE_FIFO_OVERFLOW_INTR_RAW_M) != 0;
}

void riscv_trace_hal_prepare_capture(riscv_trace_hal_context_t *ctx)
{
    HAL_ASSERT(ctx != NULL);
    riscv_trace_ll_update_mem_current_addr(ctx->dev);
    riscv_trace_ll_clear_intr(ctx->dev, TRACE_FIFO_OVERFLOW_INTR_RAW | TRACE_MEM_FULL_INTR_RAW);
}

#if SOC_RISCV_TRACE_FILTER_SUPPORTED
void riscv_trace_hal_set_filter(riscv_trace_hal_context_t *ctx, const riscv_trace_hal_filter_config_t *config)
{
    HAL_ASSERT(ctx != NULL && config != NULL);

    riscv_trace_ll_set_filter_control(ctx->dev, config->match_comparators, config->match_privilege,
                                      config->match_ecause, config->match_interrupt);
    riscv_trace_ll_set_filter_match_control(ctx->dev, config->privilege_machine,
                                            config->interrupt_itype2, config->ecause);

    riscv_trace_ll_set_p_comparator(ctx->dev, config->primary.input, config->primary.function,
                                    config->primary.notify);
    riscv_trace_ll_set_p_match_value(ctx->dev, config->primary.match_value);
    riscv_trace_ll_set_s_comparator(ctx->dev, config->secondary.input, config->secondary.function,
                                    config->secondary.notify);
    riscv_trace_ll_set_s_match_value(ctx->dev, config->secondary.match_value);
    riscv_trace_ll_set_match_mode(ctx->dev, config->match_mode);

    /* Enable last, once all matching parameters are in place. */
    riscv_trace_ll_set_filter_en(ctx->dev, config->enable);
}
#else
void riscv_trace_hal_set_filter(riscv_trace_hal_context_t *ctx, const riscv_trace_hal_filter_config_t *config)
{
    (void)ctx;
    (void)config;
}
#endif // SOC_RISCV_TRACE_FILTER_SUPPORTED
