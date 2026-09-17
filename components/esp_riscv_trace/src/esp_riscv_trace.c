/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_cache.h"
#include "esp_check.h"
#include "esp_rom_sys.h"
#include "esp_private/esp_cache_private.h"
#include "esp_private/startup_internal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_cpu.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "hal/riscv_trace_hal.h"
#include "hal/riscv_trace_ll.h"
#include "esp_riscv_trace.h"
#include "esp_riscv_trace_snapshot.h"
#include "esp_riscv_trace_priv.h"

#define ESP_RISCV_TRACE_OBJ_CAPS              (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)

#define ESP_RISCV_TRACE_BUFFER_CAPS_INTERNAL  (MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define ESP_RISCV_TRACE_BUFFER_CAPS_EXTERNAL  (MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)

#define ESP_RISCV_TRACE_BUFFER_ALIGNMENT      4

/* One word is reserved so mem_end_addr can point at the last writable word. */
#define ESP_RISCV_TRACE_BUFFER_MIN_SIZE       8

static const char *TAG = "esp_riscv_trace";

/* Handles created at startup by esp_riscv_trace_early_init(), one slot per core. */
static esp_riscv_trace_handle_t s_handle[SOC_CPU_CORES_NUM];

static uint8_t *alloc_aligned_buffer(size_t requested, uint32_t caps, size_t *out_size)
{
    *out_size = 0;

    size_t cache_alignment = 0;
    ESP_RETURN_ON_FALSE_ISR(esp_cache_get_alignment(caps, &cache_alignment) == ESP_OK, NULL, TAG,
                            "failed to get buffer alignment");
    size_t alignment = MAX(cache_alignment, ESP_RISCV_TRACE_BUFFER_ALIGNMENT);

    ESP_RETURN_ON_FALSE_ISR(requested <= SIZE_MAX - (alignment - 1), NULL, TAG,
                            "trace buffer size too large");
    size_t size = ESP_ALIGN_UP(requested, alignment);
    uint8_t *buf = heap_caps_aligned_calloc(alignment, 1, size, caps);
    ESP_RETURN_ON_FALSE_ISR(buf != NULL, NULL, TAG, "failed to allocate buffer");
    *out_size = size;
    return buf;
}

#if SOC_CACHE_INTERNAL_MEM_VIA_L1CACHE || SOC_RISCV_TRACE_MEM_SUPPORT_PSRAM
static esp_err_t sync_trace_buffer(uint8_t *buffer, size_t size, int flags)
{
    if (esp_cache_get_line_size_by_addr(buffer) == 0) {
        /* Buffer is not cacheable. Skip synchronization. */
        return ESP_OK;
    }
    return esp_cache_msync(buffer, size, flags);
}
#else
static inline esp_err_t sync_trace_buffer(uint8_t *buffer, size_t size, int flags)
{
    (void)buffer;
    (void)size;
    (void)flags;
    return ESP_OK;
}
#endif

static esp_err_t clear_trace_buffer(esp_riscv_trace_handle_t handle)
{
    memset(handle->buffer, 0, handle->buffer_size);
    return sync_trace_buffer(handle->buffer, handle->buffer_size,
                             ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
}

static esp_riscv_trace_handle_t handle_from_core(esp_riscv_trace_core_t core_id)
{
    if ((int)core_id < 0 || (int)core_id >= SOC_CPU_CORES_NUM) {
        return NULL;
    }
    return s_handle[core_id];
}

static bool is_valid_address_mode(esp_riscv_trace_address_mode_t mode)
{
    return mode == ESP_RISCV_TRACE_ADDR_DELTA || mode == ESP_RISCV_TRACE_ADDR_FULL;
}

static bool is_valid_mem_mode(esp_riscv_trace_mem_mode_t mode)
{
    return mode == ESP_RISCV_TRACE_MEM_NON_LOOP || mode == ESP_RISCV_TRACE_MEM_LOOP;
}

static bool is_valid_resync_mode(esp_riscv_trace_resync_mode_t mode)
{
    return mode == ESP_RISCV_TRACE_RESYNC_DISABLED ||
           mode == ESP_RISCV_TRACE_RESYNC_PACKET ||
           mode == ESP_RISCV_TRACE_RESYNC_CYCLE;
}

static bool is_valid_ahb_burst(esp_riscv_trace_ahb_burst_t burst)
{
    return burst == ESP_RISCV_TRACE_AHB_SINGLE ||
           burst == ESP_RISCV_TRACE_AHB_INCR ||
           burst == ESP_RISCV_TRACE_AHB_INCR4 ||
           burst == ESP_RISCV_TRACE_AHB_INCR8;
}

static bool is_valid_core_mask(uint32_t core_mask)
{
    const uint32_t valid_mask = BIT(SOC_CPU_CORES_NUM) - 1;
    return core_mask != 0 && (core_mask & ~valid_mask) == 0;
}

static bool is_valid_buffer_mem(esp_riscv_trace_buffer_mem_t mem)
{
    return mem == ESP_RISCV_TRACE_BUFFER_INTERNAL || mem == ESP_RISCV_TRACE_BUFFER_EXTERNAL;
}

static uint32_t trace_buffer_caps(esp_riscv_trace_buffer_mem_t mem)
{
    return mem == ESP_RISCV_TRACE_BUFFER_EXTERNAL ? ESP_RISCV_TRACE_BUFFER_CAPS_EXTERNAL
           : ESP_RISCV_TRACE_BUFFER_CAPS_INTERNAL;
}

#if SOC_RISCV_TRACE_FILTER_SUPPORTED
static bool is_valid_filter_comparator(const esp_riscv_trace_filter_comparator_t *c)
{
    return (c->input == ESP_RISCV_TRACE_FILTER_INPUT_IADDR ||
            c->input == ESP_RISCV_TRACE_FILTER_INPUT_TVAL) &&
           ((int)c->function >= ESP_RISCV_TRACE_FILTER_COMPARATOR_EQ &&
            (int)c->function <= ESP_RISCV_TRACE_FILTER_COMPARATOR_GE);
}

static esp_err_t validate_filter_config(const esp_riscv_trace_filter_config_t *config)
{
    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    if (!config->enable) {
        return ESP_OK;
    }
    if (config->match_comparators) {
        ESP_RETURN_ON_FALSE(is_valid_filter_comparator(&config->primary) &&
                            is_valid_filter_comparator(&config->secondary),
                            ESP_ERR_INVALID_ARG, TAG, "invalid filter comparator input/function");
        ESP_RETURN_ON_FALSE((int)config->mode >= ESP_RISCV_TRACE_FILTER_MODE_PRIMARY &&
                            (int)config->mode <= ESP_RISCV_TRACE_FILTER_MODE_RANGE,
                            ESP_ERR_INVALID_ARG, TAG, "invalid filter match mode");
    }
    if (config->match_privilege) {
        ESP_RETURN_ON_FALSE(config->privilege == ESP_RISCV_TRACE_FILTER_PRIV_USER ||
                            config->privilege == ESP_RISCV_TRACE_FILTER_PRIV_SUPERVISOR ||
                            config->privilege == ESP_RISCV_TRACE_FILTER_PRIV_MACHINE,
                            ESP_ERR_INVALID_ARG, TAG, "invalid filter privilege");
        ESP_RETURN_ON_FALSE(riscv_trace_ll_priv_is_supported((uint32_t)config->privilege),
                            ESP_ERR_NOT_SUPPORTED, TAG, "filter privilege not supported on this target");
    }
    if (config->match_ecause) {
        ESP_RETURN_ON_FALSE(config->ecause <= 0x3F, ESP_ERR_INVALID_ARG, TAG, "filter ecause out of range");
    }
    return ESP_OK;
}

/* Configure the encoder filter to skip mask ROM instructions */
__attribute__((unused))
static void apply_default_rom_filter(esp_riscv_trace_handle_t handle)
{
    riscv_trace_hal_filter_config_t hal_filter = {
        .enable = true,
        .match_comparators = true,
        .primary = {
            .input = (uint32_t)ESP_RISCV_TRACE_FILTER_INPUT_IADDR,
            .function = (uint32_t)ESP_RISCV_TRACE_FILTER_COMPARATOR_GE,
            .match_value = SOC_IROM_MASK_LOW,
        },
        .secondary = {
            .input = (uint32_t)ESP_RISCV_TRACE_FILTER_INPUT_IADDR,
            .function = (uint32_t)ESP_RISCV_TRACE_FILTER_COMPARATOR_LT,
            .match_value = SOC_IROM_MASK_HIGH,
        },
        .match_mode = (uint32_t)ESP_RISCV_TRACE_FILTER_MODE_NAND,
    };
    riscv_trace_hal_set_filter(&handle->hal, &hal_filter);
}
#endif // SOC_RISCV_TRACE_FILTER_SUPPORTED

static esp_err_t validate_trace_config(esp_riscv_trace_core_t core_id, const esp_riscv_trace_config_t *config,
                                       esp_riscv_trace_handle_t *ret_handle)
{
    ESP_RETURN_ON_FALSE_ISR(config != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR(ret_handle != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE_ISR((int)core_id >= 0 && (int)core_id < SOC_CPU_CORES_NUM,
                            ESP_ERR_INVALID_ARG, TAG, "invalid core id");
    ESP_RETURN_ON_FALSE_ISR(config->buffer_size >= ESP_RISCV_TRACE_BUFFER_MIN_SIZE, ESP_ERR_INVALID_SIZE, TAG,
                            "trace buffer must be at least %d bytes", ESP_RISCV_TRACE_BUFFER_MIN_SIZE);
    ESP_RETURN_ON_FALSE_ISR(is_valid_address_mode(config->address_mode), ESP_ERR_INVALID_ARG, TAG,
                            "invalid address mode");
    ESP_RETURN_ON_FALSE_ISR(is_valid_mem_mode(config->mem_mode), ESP_ERR_INVALID_ARG, TAG, "invalid memory mode");
    ESP_RETURN_ON_FALSE_ISR(is_valid_resync_mode(config->resync_mode), ESP_ERR_INVALID_ARG, TAG,
                            "invalid resync mode");
    ESP_RETURN_ON_FALSE_ISR(riscv_trace_ll_resync_mode_is_supported((uint32_t)config->resync_mode),
                            ESP_ERR_NOT_SUPPORTED, TAG, "resync mode not supported on this target");
    ESP_RETURN_ON_FALSE_ISR(is_valid_ahb_burst(config->ahb_burst), ESP_ERR_INVALID_ARG, TAG, "invalid AHB burst");
    ESP_RETURN_ON_FALSE_ISR(is_valid_core_mask(config->core_mask), ESP_ERR_INVALID_ARG, TAG, "invalid core mask");
    ESP_RETURN_ON_FALSE_ISR(is_valid_buffer_mem(config->buffer_mem), ESP_ERR_INVALID_ARG, TAG,
                            "invalid buffer memory");

    return ESP_OK;
}

static esp_err_t esp_riscv_trace_new(esp_riscv_trace_core_t core_id, const esp_riscv_trace_config_t *config,
                                     esp_riscv_trace_handle_t *ret_handle)
{
    esp_err_t ret = ESP_OK;

    ESP_RETURN_ON_ERROR_ISR(validate_trace_config(core_id, config, ret_handle), TAG, "invalid trace configuration");

    size_t trace_mem_size = 0;
    uint8_t *trace_mem = alloc_aligned_buffer(config->buffer_size, trace_buffer_caps(config->buffer_mem),
                                              &trace_mem_size);
    ESP_RETURN_ON_FALSE_ISR(trace_mem != NULL, ESP_ERR_NO_MEM, TAG, "failed to allocate trace buffer");

    esp_riscv_trace_handle_t handle = heap_caps_calloc(1, sizeof(struct esp_riscv_trace_context_t),
                                                       ESP_RISCV_TRACE_OBJ_CAPS);
    ESP_GOTO_ON_FALSE_ISR(handle != NULL, ESP_ERR_NO_MEM, err_alloc, TAG, "no mem for the trace handle");

    riscv_trace_hal_context_t hal_ctx;
    riscv_trace_hal_config_t hal_config = {
        .mem_start_addr = (uint32_t)trace_mem,
        .mem_end_addr = (uint32_t)trace_mem + trace_mem_size - sizeof(uint32_t),
        .full_address = (config->address_mode == ESP_RISCV_TRACE_ADDR_FULL),
        .mem_loop = (config->mem_mode == ESP_RISCV_TRACE_MEM_LOOP),
        .auto_restart = config->auto_restart,
        .stall_cpu = config->stall_cpu,
        .halt_enable = config->halt_enable,
        .reset_enable = config->reset_enable,
        .debug_trigger_enable = config->debug_trigger_enable,
        .resync_mode = config->resync_mode,
        .resync_threshold = config->resync_threshold,
        .ahb_burst = config->ahb_burst,
        .ahb_max_incr = config->ahb_max_incr,
        .intr_mask = config->intr_mask,
    };
    riscv_trace_hal_init(core_id, &hal_config, &hal_ctx);

    handle->hal = hal_ctx;
    handle->core_id = core_id;
    handle->buffer = trace_mem;
    handle->buffer_size = trace_mem_size;
    handle->state = ESP_RISCV_TRACE_STATE_CREATED;
    handle->auto_restart = config->auto_restart;
    *ret_handle = handle;

#if CONFIG_ESP_RISCV_TRACE_FILTER_OUT_ROM
    apply_default_rom_filter(handle);
#endif

    ESP_EARLY_LOGD(TAG, "RISC-V trace encoder initialized on core %d", core_id);

    return ESP_OK;

err_alloc:
    heap_caps_free(trace_mem);
    return ret;
}

static esp_err_t trace_start_locked(esp_riscv_trace_handle_t handle)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE_ISR(handle->state == ESP_RISCV_TRACE_STATE_CREATED ||
                            handle->state == ESP_RISCV_TRACE_STATE_STOPPED,
                            ESP_ERR_INVALID_STATE, TAG, "not startable from this state");
    ESP_RETURN_ON_ERROR_ISR(clear_trace_buffer(handle), TAG, "failed to sync cleared trace buffer");
    riscv_trace_hal_prepare_capture(&handle->hal);
    riscv_trace_hal_set_auto_restart(&handle->hal, handle->auto_restart);
    handle->state = ESP_RISCV_TRACE_STATE_STARTED;
    riscv_trace_hal_start(&handle->hal);
    esp_riscv_trace_snapshot_start(handle->core_id);

    return ret;
}

static esp_err_t trace_stop_locked(esp_riscv_trace_handle_t handle, uint32_t timeout_us)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE(handle->state == ESP_RISCV_TRACE_STATE_STARTED, ESP_ERR_INVALID_STATE, TAG,
                        "trace not started");

    bool flushed = riscv_trace_hal_stop(&handle->hal, timeout_us);
    if (flushed) {
        esp_err_t sync_ret = sync_trace_buffer(handle->buffer, handle->buffer_size,
                                               ESP_CACHE_MSYNC_FLAG_DIR_M2C | ESP_CACHE_MSYNC_FLAG_INVALIDATE);
        if (sync_ret != ESP_OK) {
            ESP_LOGE(TAG, "failed to sync trace buffer after stop");
            ret = sync_ret;
        }
    } else {
        ESP_LOGE(TAG, "timed out waiting for trace FIFO to empty");
        ret = ESP_ERR_TIMEOUT;
    }

    handle->state = ESP_RISCV_TRACE_STATE_STOPPED;

    uint32_t fifo_status = riscv_trace_hal_read_fifo_status(&handle->hal);
    uint32_t intr_status = riscv_trace_hal_read_intr_raw(&handle->hal);
    uint32_t base = (uint32_t)handle->buffer;
    uint32_t current = riscv_trace_hal_get_current_addr(&handle->hal);
    bool head_valid = (current >= base) && (current <= base + handle->buffer_size);
    esp_riscv_trace_snapshot_stop(handle->core_id, fifo_status, intr_status,
                                  head_valid ? (current - base) : 0, head_valid);

    return ret;
}

esp_err_t esp_riscv_trace_start(esp_riscv_trace_core_t core_id)
{
    esp_err_t ret;
    esp_riscv_trace_handle_t handle = handle_from_core(core_id);

    ESP_RETURN_ON_FALSE_ISR(handle != NULL, ESP_ERR_INVALID_STATE, TAG, "core %d trace not initialized",
                            (int)core_id);

    _lock_acquire(&handle->lock);
    ret = trace_start_locked(handle);
    _lock_release(&handle->lock);
    return ret;
}

esp_err_t esp_riscv_trace_stop(esp_riscv_trace_core_t core_id, uint32_t timeout_us)
{
    esp_err_t ret;
    esp_riscv_trace_handle_t handle = handle_from_core(core_id);

    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_STATE, TAG, "core %d trace not initialized", (int)core_id);

    _lock_acquire(&handle->lock);
    ret = trace_stop_locked(handle, timeout_us);
    _lock_release(&handle->lock);
    return ret;
}

static uint16_t IRAM_ATTR snapshot_freeze_state(esp_riscv_trace_state_t state)
{
    switch (state) {
    case ESP_RISCV_TRACE_STATE_STARTED: return ESP_RISCV_TRACE_SNAPSHOT_STATE_FROZEN;
    case ESP_RISCV_TRACE_STATE_STOPPED: return ESP_RISCV_TRACE_SNAPSHOT_STATE_STOPPED;
    default:                            return ESP_RISCV_TRACE_SNAPSHOT_STATE_READY;
    }
}

#define ESP_RISCV_TRACE_PANIC_FLUSH_TIMEOUT_US 2000
#define ESP_RISCV_TRACE_PANIC_FLUSH_STEP_US    10

void esp_panic_handler_inst_trace_stop(void)
{
    /* Runs at panic entry before other core is stalled, so guard against both cores stopping at once. */
    static uint32_t s_stopped;

    if (!esp_cpu_compare_and_set(&s_stopped, 0, 1)) {
        return;
    }

    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        esp_riscv_trace_handle_t handle = s_handle[core];
        if (handle == NULL) {
            continue;
        }
        riscv_trace_ll_set_restart_ena(handle->hal.dev, false);
        riscv_trace_ll_trigger_off(handle->hal.dev);
    }
}

void esp_riscv_trace_snapshot_finalize(void)
{
    static uint32_t s_finalized;

    if (!esp_cpu_compare_and_set(&s_finalized, 0, 1)) {
        return;
    }

    /* Encoders were stopped at panic entry. Wait for each FIFO to empty and
       record the final status. */
    esp_riscv_trace_snapshot_panic_core_t status[SOC_CPU_CORES_NUM] = {0};
    for (int core = 0; core < SOC_CPU_CORES_NUM; core++) {
        esp_riscv_trace_handle_t handle = s_handle[core];
        if (handle == NULL) {
            continue;
        }

        void *dev = handle->hal.dev;
        uint32_t waited_us = 0;
        uint32_t fifo_status = riscv_trace_ll_get_fifo_status(dev);
        while ((fifo_status & TRACE_FIFO_EMPTY_M) == 0) {
            if (waited_us >= ESP_RISCV_TRACE_PANIC_FLUSH_TIMEOUT_US) {
                break;
            }
            esp_rom_delay_us(ESP_RISCV_TRACE_PANIC_FLUSH_STEP_US);
            waited_us += ESP_RISCV_TRACE_PANIC_FLUSH_STEP_US;
            fifo_status = riscv_trace_ll_get_fifo_status(dev);
        }
        uint32_t base = (uint32_t)handle->buffer;
        uint32_t current = riscv_trace_ll_get_mem_current_addr(dev);
        uint32_t intr_status = riscv_trace_ll_get_intr_raw(dev);
        bool head_valid = (current >= base) && (current <= base + handle->buffer_size);

        status[core].present = true;
        status[core].capturing = (handle->state == ESP_RISCV_TRACE_STATE_STARTED);
        status[core].state = snapshot_freeze_state(handle->state);
        status[core].head_valid = head_valid;
        status[core].head_offset = head_valid ? (current - base) : 0;
        status[core].fifo_status_raw = fifo_status;
        status[core].intr_status_raw = intr_status;
        status[core].fifo_empty = (fifo_status & TRACE_FIFO_EMPTY_M) != 0;
        status[core].memory_full = (intr_status & TRACE_MEM_FULL_INTR_RAW_M) != 0;
        status[core].fifo_overflow = (intr_status & TRACE_FIFO_OVERFLOW_INTR_RAW_M) != 0;
    }

    esp_riscv_trace_snapshot_panic_write(status, SOC_CPU_CORES_NUM);
}

esp_err_t esp_riscv_trace_get_buffer(esp_riscv_trace_core_t core_id, uint8_t **buffer,
                                     size_t *capacity, size_t *head_offset)
{
    esp_err_t ret = ESP_OK;
    uint32_t base = 0;
    uint32_t current = 0;
    esp_riscv_trace_handle_t handle = handle_from_core(core_id);

    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_STATE, TAG, "core %d trace not initialized", (int)core_id);
    ESP_RETURN_ON_FALSE(buffer != NULL && capacity != NULL && head_offset != NULL,
                        ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    _lock_acquire(&handle->lock);
    ESP_GOTO_ON_FALSE(handle->state != ESP_RISCV_TRACE_STATE_STARTED, ESP_ERR_INVALID_STATE, out, TAG,
                      "buffer not coherent while started");

    base = (uint32_t)handle->buffer;
    current = riscv_trace_hal_get_current_addr(&handle->hal);
    ESP_GOTO_ON_FALSE(current >= base && current <= base + handle->buffer_size,
                      ESP_ERR_INVALID_STATE, out, TAG, "trace write pointer 0x%08x out of buffer range",
                      (unsigned)current);

    *buffer = handle->buffer;
    *capacity = handle->buffer_size;
    *head_offset = current - base;

out:
    _lock_release(&handle->lock);
    return ret;
}

esp_err_t esp_riscv_trace_get_status(esp_riscv_trace_core_t core_id, esp_riscv_trace_status_t *status)
{
    esp_err_t ret = ESP_OK;
    esp_riscv_trace_handle_t handle = handle_from_core(core_id);

    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_STATE, TAG, "core %d trace not initialized", (int)core_id);
    ESP_RETURN_ON_FALSE(status != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid argument");

    _lock_acquire(&handle->lock);
    uint32_t fifo_status = riscv_trace_hal_read_fifo_status(&handle->hal);
    uint32_t intr_status = riscv_trace_hal_read_intr_raw(&handle->hal);

    status->work_status = riscv_trace_hal_get_work_status(fifo_status);
    status->fifo_empty = riscv_trace_hal_fifo_is_empty(fifo_status);
    status->memory_full = riscv_trace_hal_memory_is_full(intr_status);
    status->fifo_overflowed = riscv_trace_hal_fifo_is_overflowed(intr_status);
    _lock_release(&handle->lock);

    return ret;
}

#if SOC_RISCV_TRACE_FILTER_SUPPORTED
esp_err_t esp_riscv_trace_set_filter(esp_riscv_trace_core_t core_id, const esp_riscv_trace_filter_config_t *config)
{
    esp_err_t ret = ESP_OK;
    esp_riscv_trace_handle_t handle = handle_from_core(core_id);

    ESP_RETURN_ON_FALSE(handle != NULL, ESP_ERR_INVALID_STATE, TAG, "core %d trace not initialized", (int)core_id);
    ESP_RETURN_ON_ERROR(validate_filter_config(config), TAG, "invalid filter configuration");

    riscv_trace_hal_filter_config_t hal_filter = { .enable = config->enable };
    if (config->enable) {
        hal_filter.match_comparators = config->match_comparators;
        hal_filter.match_privilege = config->match_privilege;
        hal_filter.match_ecause = config->match_ecause;
        hal_filter.match_interrupt = config->match_interrupt;
        if (config->match_comparators) {
            hal_filter.primary = (riscv_trace_hal_comparator_t) {
                .input = (uint32_t)config->primary.input,
                .function = (uint32_t)config->primary.function,
                .match_value = config->primary.match_value,
                .notify = config->primary.notify,
            };
            hal_filter.secondary = (riscv_trace_hal_comparator_t) {
                .input = (uint32_t)config->secondary.input,
                .function = (uint32_t)config->secondary.function,
                .match_value = config->secondary.match_value,
                .notify = config->secondary.notify,
            };
            hal_filter.match_mode = (uint32_t)config->mode;
        }
        if (config->match_privilege) {
            hal_filter.privilege = (uint32_t)config->privilege;
        }
        if (config->match_ecause) {
            hal_filter.ecause = config->ecause;
        }
        if (config->match_interrupt) {
            hal_filter.interrupt_itype2 = config->interrupt_itype2;
        }
    }
    _lock_acquire(&handle->lock);
    ESP_GOTO_ON_FALSE(handle->state != ESP_RISCV_TRACE_STATE_STARTED, ESP_ERR_INVALID_STATE, out, TAG,
                      "filter must be set while the core is not running");
    riscv_trace_hal_set_filter(&handle->hal, &hal_filter);
out:
    _lock_release(&handle->lock);
    return ret;
}
#else
esp_err_t esp_riscv_trace_set_filter(esp_riscv_trace_core_t core_id, const esp_riscv_trace_filter_config_t *config)
{
    (void)core_id;
    (void)config;
    return ESP_ERR_NOT_SUPPORTED;
}
#endif // SOC_RISCV_TRACE_FILTER_SUPPORTED

/* Default per-core configuration. Applications can override this function. */
esp_riscv_trace_config_t __attribute__((weak)) esp_riscv_trace_get_user_config(int core_id)
{
    (void)core_id;
    esp_riscv_trace_config_t config = ESP_RISCV_TRACE_DEFAULT_CONFIG();
    return config;
}

ESP_SYSTEM_INIT_FN(esp_riscv_trace_early_init, SECONDARY, ESP_SYSTEM_INIT_ALL_CORES, 160)
{
    int core_id = esp_cpu_get_core_id();
    esp_riscv_trace_config_t config = esp_riscv_trace_get_user_config(core_id);

    esp_riscv_trace_snapshot_early_init(core_id);

    /* Enable clock and reset the encoder before accessing registers. */
    PERIPH_RCC_ATOMIC() {
        riscv_trace_ll_enable_bus_clock(true);
        riscv_trace_ll_reset_register(core_id);
    }

    if (!is_valid_core_mask(config.core_mask)) {
        ESP_EARLY_LOGE(TAG, "invalid core mask");
        return ESP_ERR_INVALID_ARG;
    }

    if ((config.core_mask & BIT(core_id)) == 0) {
        return ESP_OK;
    }

    esp_err_t ret = esp_riscv_trace_new(core_id, &config, &s_handle[core_id]);
    if (ret != ESP_OK) {
        ESP_EARLY_LOGE(TAG, "early init failed on core %d: %s", core_id, esp_err_to_name(ret));
        return ret;
    }

    esp_riscv_trace_snapshot_write_core_desc(core_id, s_handle[core_id]);

#if CONFIG_ESP_RISCV_TRACE_AUTOSTART
    /* Each core starts its own encoder, so the order between cores does not matter. */
    esp_err_t start_ret = esp_riscv_trace_start(core_id);
    if (start_ret != ESP_OK) {
        ESP_EARLY_LOGW(TAG, "autostart failed on core %d: %s", core_id, esp_err_to_name(start_ret));
    }
#endif

    return ESP_OK;
}
