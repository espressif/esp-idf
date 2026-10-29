/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_rom_sys.h"
#include "esp_dbg_stubs.h"
#include "esp_private/startup_internal.h"
#include "soc/soc_caps.h"
#include "hal/cache_hal.h"
#include "hal/riscv_trace_ll.h"
#include "esp_riscv_trace_priv.h"
#include "esp_riscv_trace_ocd.h"

#if CONFIG_ESP_RISCV_TRACE_OPENOCD_CONTROL
/*
 * The functions below are called by OpenOCD on a halted core while every core is halted.
 * They take no locks and do no logging or heap allocation.
 */

bool esp_riscv_trace_ocd_capture_active(int core_id)
{
    uint8_t state = g_esp_riscv_trace_ocd.cores[core_id].state;
    return state != ESP_RISCV_TRACE_OCD_STATE_IDLE && state != ESP_RISCV_TRACE_OCD_STATE_STOPPED;
}

static bool core_is_traced(int core)
{
    return esp_riscv_trace_get_handle(core) != NULL;
}

static bool core_is_busy(int core)
{
    esp_riscv_trace_handle_t handle = esp_riscv_trace_get_handle(core);
    return handle != NULL && handle->busy;
}

static uint32_t core_mask_of(bool (*pred)(int core))
{
    uint32_t mask = 0;
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        if (pred(core)) {
            mask |= BIT(core);
        }
    }
    return mask;
}

static uint32_t traced_core_mask(void)
{
    return core_mask_of(core_is_traced);
}

static uint32_t active_core_mask(void)
{
    return core_mask_of(esp_riscv_trace_ocd_capture_active);
}

static uint32_t busy_core_mask(void)
{
    return core_mask_of(core_is_busy);
}

/* Zeroes the buffer so the decoder can tell whether a loop capture wrapped. */
static void clear_buffer(esp_riscv_trace_handle_t handle)
{
    memset(handle->buffer, 0, handle->buffer_size);
#if SOC_CACHE_WRITEBACK_SUPPORTED
    /* The encoder writes memory directly, so no dirty line may be written back over its data. */
    cache_hal_writeback_addr((uint32_t)handle->buffer, handle->buffer_size);
#endif
}

static void read_capture_cfg(trace_dev_t *dev, esp_riscv_trace_capture_cfg_t *cfg)
{
    cfg->mem_loop = riscv_trace_ll_get_mem_loop(dev);
    cfg->resync_mode = riscv_trace_ll_get_resync_mode(dev);
    cfg->resync_threshold = riscv_trace_ll_get_resync_threshold(dev);
}

static void write_capture_cfg(trace_dev_t *dev, const esp_riscv_trace_capture_cfg_t *cfg)
{
    riscv_trace_ll_set_mem_loop(dev, cfg->mem_loop);
    riscv_trace_ll_set_resync_mode(dev, cfg->resync_mode);
    riscv_trace_ll_set_resync_threshold(dev, cfg->resync_threshold);
}

/* Marks the handle stopped and records the stop in the snapshot. The encoder is already off. */
static void mark_stopped(esp_riscv_trace_handle_t handle, int core_id)
{
    trace_dev_t *dev = handle->hal.dev;
    uint32_t head_offset;
    bool head_valid = esp_riscv_trace_head_offset(handle, riscv_trace_ll_get_mem_current_addr(dev), &head_offset);

    esp_riscv_trace_snapshot_stop_nolock(core_id, riscv_trace_ll_get_fifo_status(dev),
                                         riscv_trace_ll_get_intr_raw(dev), head_offset, head_valid);
    handle->state = ESP_RISCV_TRACE_STATE_STOPPED;
}

/* Starts a loop capture on one core. The caller checked that the core is free. */
static esp_err_t start_core(int core_id)
{
    esp_riscv_trace_handle_t handle = esp_riscv_trace_get_handle(core_id);
    trace_dev_t *dev = handle->hal.dev;
    esp_riscv_trace_ocd_core_t *ocd = &g_esp_riscv_trace_ocd.cores[core_id];

    /* Stop a running app or autostart capture. Its data is discarded. */
    riscv_trace_ll_set_intr_ena(dev, 0);
    riscv_trace_ll_set_restart_ena(dev, false);
    riscv_trace_ll_trigger_off(dev);
    if (!esp_riscv_trace_wait_fifo_empty(dev)) {
        /* Leftover FIFO data would land in the cleared buffer. Refuse and keep the app capture stopped. */
        if (handle->state == ESP_RISCV_TRACE_STATE_STARTED) {
            mark_stopped(handle, core_id);
        }
        ocd->error = ESP_RISCV_TRACE_OCD_ERR_FIFO_TIMEOUT;
        return ESP_ERR_TIMEOUT;
    }

    read_capture_cfg(dev, &handle->saved_cfg);
    esp_riscv_trace_capture_cfg_t cfg = handle->saved_cfg;
    cfg.mem_loop = true;
    if (cfg.resync_mode == RISCV_TRACE_RESYNC_DISABLED) {
        /* A wrapped loop buffer is decodable only with periodic resync. */
        cfg.resync_mode = RISCV_TRACE_RESYNC_PACKET;
        cfg.resync_threshold = CONFIG_ESP_RISCV_TRACE_RESYNC_THRESHOLD;
    }

    clear_buffer(handle);
    write_capture_cfg(dev, &cfg);
    riscv_trace_ll_update_mem_current_addr(dev);
    riscv_trace_ll_clear_intr(dev, RISCV_TRACE_INTR_FIFO_OVERFLOW | RISCV_TRACE_INTR_MEM_FULL);
    /* Interrupts stay off so stop records the raw overflow bit. Restart keeps the capture running after an overflow. */
    riscv_trace_ll_set_restart_ena(dev, true);

    handle->state = ESP_RISCV_TRACE_STATE_STARTED;
    ocd->mode = ESP_RISCV_TRACE_OCD_MODE_LOOP;
    ocd->state = ESP_RISCV_TRACE_OCD_STATE_LOOPING;
    ocd->error = ESP_RISCV_TRACE_OCD_ERR_NONE;
    esp_riscv_trace_snapshot_start_nolock(core_id, &cfg);

    riscv_trace_ll_trigger_on(dev);
    return ESP_OK;
}

/* Stops the OpenOCD capture on one core and gives the encoder back stopped. */
static esp_err_t stop_core(int core_id)
{
    esp_riscv_trace_handle_t handle = esp_riscv_trace_get_handle(core_id);
    trace_dev_t *dev = handle->hal.dev;
    esp_riscv_trace_ocd_core_t *ocd = &g_esp_riscv_trace_ocd.cores[core_id];

    riscv_trace_ll_set_restart_ena(dev, false);
    riscv_trace_ll_trigger_off(dev);
    bool fifo_empty = esp_riscv_trace_wait_fifo_empty(dev);
    bool overflowed = (riscv_trace_ll_get_intr_raw(dev) & RISCV_TRACE_INTR_FIFO_OVERFLOW) != 0;
    mark_stopped(handle, core_id);

    /* Give the encoder back with the app configuration. It stays stopped. */
    write_capture_cfg(dev, &handle->saved_cfg);
    /* Drop cached lines so an app read after this sees the encoder data. */
    cache_hal_invalidate_addr((uint32_t)handle->buffer, handle->buffer_size);

    /* A FIFO that did not empty is reported, but the encoder is still released. */
    ocd->state = ESP_RISCV_TRACE_OCD_STATE_STOPPED;
    ocd->error = !fifo_empty ? ESP_RISCV_TRACE_OCD_ERR_FIFO_TIMEOUT :
                 overflowed ? ESP_RISCV_TRACE_OCD_ERR_FIFO_OVF : ESP_RISCV_TRACE_OCD_ERR_NONE;

    return fifo_empty ? ESP_OK : ESP_ERR_TIMEOUT;
}

/* Stops the cores in core_mask and returns the first error. */
static esp_err_t stop_cores(uint32_t core_mask)
{
    esp_err_t ret = ESP_OK;
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        if (core_mask & BIT(core)) {
            esp_err_t err = stop_core(core);
            if (ret == ESP_OK) {
                ret = err;
            }
        }
    }
    return ret;
}

static bool core_mask_is_valid(uint32_t core_mask)
{
    return core_mask != 0 && (core_mask & ~traced_core_mask()) == 0;
}

/* Starts every core in core_mask, or none. A failure stops the cores already started here. */
static esp_err_t ocd_start(uint32_t core_mask, int mode)
{
    if (!core_mask_is_valid(core_mask)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (mode != ESP_RISCV_TRACE_OCD_MODE_LOOP) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    if (core_mask & active_core_mask()) {
        return ESP_ERR_INVALID_STATE;
    }
    /* A halted app call or snapshot update is in progress. OpenOCD resumes and retries. */
    if ((core_mask & busy_core_mask()) || esp_riscv_trace_snapshot_busy()) {
        return ESP_ERR_NOT_FINISHED;
    }

    uint32_t started = 0;
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        if (!(core_mask & BIT(core))) {
            continue;
        }
        esp_err_t err = start_core(core);
        if (err != ESP_OK) {
            stop_cores(started);
            return err;
        }
        started |= BIT(core);
    }
    return ESP_OK;
}

/* Stops every OpenOCD capture in core_mask and returns the first error. */
static esp_err_t ocd_stop(uint32_t core_mask)
{
    if (!core_mask_is_valid(core_mask)) {
        return ESP_ERR_INVALID_ARG;
    }
    if (esp_riscv_trace_snapshot_busy()) {
        return ESP_ERR_NOT_FINISHED;
    }

    uint32_t active = core_mask & active_core_mask();
    return active ? stop_cores(active) : ESP_ERR_INVALID_STATE;
}
#endif /* CONFIG_ESP_RISCV_TRACE_OPENOCD_CONTROL */

esp_riscv_trace_ocd_t g_esp_riscv_trace_ocd = {
    .hdr = {
        .magic = ESP_RISCV_TRACE_OCD_MAGIC,
        .abi_version = ESP_RISCV_TRACE_OCD_ABI_VERSION,
        .core_count = SOC_CPU_CORES_NUM,
        .snapshot = &g_esp_riscv_trace_snapshot,
#if CONFIG_ESP_RISCV_TRACE_OPENOCD_CONTROL
        .start = ocd_start,
        .stop = ocd_stop,
#endif
    },
};

/* Referenced with -u so the linker keeps this object. */
void esp_riscv_trace_include_ocd(void)
{
}

ESP_SYSTEM_INIT_FN(esp_riscv_trace_ocd_init, SECONDARY, BIT(0), 139)
{
    return esp_dbg_stub_entry_set(ESP_DBG_STUB_ENTRY_RISCV_TRACE, (uint32_t)&g_esp_riscv_trace_ocd);
}
