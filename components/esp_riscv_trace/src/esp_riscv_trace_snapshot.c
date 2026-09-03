/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Maintains the trace snapshot consumed by the coredump writer. */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "hal/riscv_trace_hal.h"
#include "hal/efuse_hal.h"
#include "esp_app_desc.h"
#include "esp_riscv_trace.h"
#include "esp_riscv_trace_priv.h"
#include "esp_riscv_trace_snapshot.h"

/* Per-core snapshot descriptors, reached through g_esp_riscv_trace_snapshot.cores_addr. */
static esp_riscv_trace_snapshot_core_desc_t s_core_desc[SOC_CPU_CORES_NUM];

/* Target-wide encoder parameters, reached through g_esp_riscv_trace_snapshot.encoder_params_addr. */
static esp_riscv_trace_encoder_params_t s_encoder_params;

/* Target ELF SHA-256 hash, reached through g_esp_riscv_trace_snapshot.app_elf_sha256_addr. */
static uint8_t s_app_elf_sha256[32];

esp_riscv_trace_snapshot_desc_t g_esp_riscv_trace_snapshot = {
    .abi_major = ESP_RISCV_TRACE_SNAPSHOT_ABI_MAJOR,
    .abi_minor = ESP_RISCV_TRACE_SNAPSHOT_ABI_MINOR,
    .snapshot_desc_size = ESP_RISCV_TRACE_SNAPSHOT_DESC_SIZE,
    .core_desc_size = ESP_RISCV_TRACE_SNAPSHOT_CORE_DESC_SIZE,
    .cores_addr = (uint32_t)(uintptr_t)s_core_desc,
    .core_count = SOC_CPU_CORES_NUM,
    .encoder_params_addr = (uint32_t)(uintptr_t) &s_encoder_params,
    .encoder_params_size = ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE,
    .app_elf_sha256_addr = (uint32_t)(uintptr_t)s_app_elf_sha256,
};

static portMUX_TYPE s_snapshot_lock = portMUX_INITIALIZER_UNLOCKED;
static uint8_t s_active_capture_cores;

/*
 * Writers set an odd write_seq before updates and an even one afterward.
 * Hardware fences keep descriptor stores inside that interval.
 */
static void snapshot_write_begin(void)
{
    portENTER_CRITICAL(&s_snapshot_lock);
    g_esp_riscv_trace_snapshot.write_seq++;
    __asm__ __volatile__("fence rw, rw" ::: "memory");
}

static void snapshot_write_end(void)
{
    __asm__ __volatile__("fence rw, rw" ::: "memory");
    g_esp_riscv_trace_snapshot.write_seq++;
    portEXIT_CRITICAL(&s_snapshot_lock);
}

static uint16_t snapshot_memory_mode_from_hw(bool loop)
{
    return loop ? ESP_RISCV_TRACE_MEMORY_MODE_LOOP : ESP_RISCV_TRACE_MEMORY_MODE_LINEAR;
}

static uint16_t snapshot_resync_mode_from_hw(uint32_t hw_mode)
{
    switch (hw_mode) {
    case RISCV_TRACE_RESYNC_DISABLED: return ESP_RISCV_TRACE_RESYNC_MODE_DISABLED;
    case RISCV_TRACE_RESYNC_PACKET:   return ESP_RISCV_TRACE_RESYNC_MODE_PACKET;
    case RISCV_TRACE_RESYNC_CYCLE:    return ESP_RISCV_TRACE_RESYNC_MODE_CYCLE;
    default:                          return ESP_RISCV_TRACE_RESYNC_MODE_UNKNOWN;
    }
}

static uint16_t snapshot_address_mode_from_hal(riscv_trace_hal_context_t *hal)
{
#if SOC_RISCV_TRACE_HAS_CONFIG_REG
    return riscv_trace_hal_get_full_address(hal) ? ESP_RISCV_TRACE_ADDRESS_MODE_FULL
           : ESP_RISCV_TRACE_ADDRESS_MODE_DELTA;
#else
    (void)hal;
    /* Without a config register the encoder is fixed in delta mode. */
    return ESP_RISCV_TRACE_ADDRESS_MODE_DELTA;
#endif
}

static void fill_encoder_params(esp_riscv_trace_encoder_params_t *params)
{
    memset(params, 0, sizeof(*params));
    params->params_version = ESP_RISCV_TRACE_ENCODER_PARAMS_VERSION;
    params->params_size = ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE;
    params->ecause_width_p = SOC_RISCV_TRACE_ECAUSE_WIDTH;
#if SOC_RISCV_TRACE_FILTER_SUPPORTED
    params->filter_excint_p = 1;
    params->filter_privilege_p = 1;
    params->filter_tval_p = 1;
#endif
    params->iaddress_lsb_p = 1;
    params->iaddress_width_p = 32;
    params->iretire_width_p = 1;
    params->itype_width_p = 3;
    params->nocontext_p = 1;
    params->notime_p = 1;
    params->privilege_width_p = SOC_RISCV_TRACE_PRIV_WIDTH;
    params->retires_p = 1;
    params->taken_branches_p = 1;
}

static void write_target_identity(void)
{
    g_esp_riscv_trace_snapshot.target_id = (uint16_t)CONFIG_IDF_FIRMWARE_CHIP_ID;
    g_esp_riscv_trace_snapshot.chip_revision = (uint16_t)efuse_hal_chip_revision();

    const esp_app_desc_t *desc = esp_app_get_description();
    memcpy(s_app_elf_sha256, desc->app_elf_sha256, sizeof(s_app_elf_sha256));
    g_esp_riscv_trace_snapshot.app_elf_sha256_size = (uint8_t)sizeof(s_app_elf_sha256);
}

void esp_riscv_trace_snapshot_early_init(int core_id)
{
    snapshot_write_begin();
    s_core_desc[core_id].core_id = (uint16_t)core_id;

    /* Core 0 writes the shared data. */
    if (core_id == 0) {
        write_target_identity();
        fill_encoder_params(&s_encoder_params);
        g_esp_riscv_trace_snapshot.magic = ESP_RISCV_TRACE_SNAPSHOT_MAGIC;
    }
    snapshot_write_end();
}

void esp_riscv_trace_snapshot_write_core_desc(int core_id, const esp_riscv_trace_handle_t handle)
{
    esp_riscv_trace_snapshot_core_desc_t *desc = &s_core_desc[core_id];
    riscv_trace_hal_context_t *hal = &handle->hal;

    snapshot_write_begin();
    desc->buffer_addr = (uint32_t)(uintptr_t)handle->buffer;
    desc->capacity = (uint32_t)handle->buffer_size;
    desc->packet_format = SOC_RISCV_TRACE_PACKET_FORMAT_VER;
    desc->memory_mode = snapshot_memory_mode_from_hw(riscv_trace_hal_get_mem_loop(hal));
    desc->address_mode = snapshot_address_mode_from_hal(hal);
    desc->resync_mode = snapshot_resync_mode_from_hw(riscv_trace_hal_get_resync_mode(hal));
    desc->resync_threshold = riscv_trace_hal_get_resync_threshold(hal);
    desc->state = ESP_RISCV_TRACE_SNAPSHOT_STATE_READY;
    desc->head_valid = 0;
    snapshot_write_end();
}

void esp_riscv_trace_snapshot_start(int core_id)
{
    snapshot_write_begin();
    if (s_active_capture_cores == 0) {
        g_esp_riscv_trace_snapshot.capture_reason = ESP_RISCV_TRACE_CAPTURE_REASON_UNKNOWN;
    }
    s_active_capture_cores++;

    esp_riscv_trace_snapshot_core_desc_t *desc = &s_core_desc[core_id];
    desc->state = ESP_RISCV_TRACE_SNAPSHOT_STATE_CAPTURING;
    desc->head_offset = 0;
    desc->head_valid = 0;
    desc->fifo_status_raw = 0;
    desc->intr_status_raw = 0;
    desc->fifo_empty = 0;
    desc->memory_full = 0;
    desc->fifo_overflow = 0;
    snapshot_write_end();
}

void esp_riscv_trace_snapshot_stop(int core_id, uint32_t fifo_status, uint32_t intr_status,
                                   uint32_t head_offset, bool head_valid)
{
    snapshot_write_begin();
    esp_riscv_trace_snapshot_core_desc_t *desc = &s_core_desc[core_id];
    desc->state = ESP_RISCV_TRACE_SNAPSHOT_STATE_STOPPED;
    desc->fifo_status_raw = fifo_status;
    desc->intr_status_raw = intr_status;
    desc->fifo_empty = riscv_trace_hal_fifo_is_empty(fifo_status) ? 1 : 0;
    desc->memory_full = riscv_trace_hal_memory_is_full(intr_status) ? 1 : 0;
    desc->fifo_overflow = riscv_trace_hal_fifo_is_overflowed(intr_status) ? 1 : 0;
    desc->head_offset = head_valid ? head_offset : 0;
    desc->head_valid = head_valid ? 1 : 0;

    if (s_active_capture_cores > 0) {
        s_active_capture_cores--;
    }
    if (s_active_capture_cores == 0) {
        g_esp_riscv_trace_snapshot.capture_reason = ESP_RISCV_TRACE_CAPTURE_REASON_EXPLICIT_STOP;
    }
    snapshot_write_end();
}

/*
 * The panic handler has already stalled the other cores
 * (esp_cpu_stall), so this core is the sole writer. Taking s_snapshot_lock
 * could deadlock against a stalled core that still holds it, so update
 * lock-free. Force write_seq odd (not ++) to discard any interrupted
 * normal writer, then close it even.
 */
static void panic_write_begin(void)
{
    g_esp_riscv_trace_snapshot.write_seq |= 1u;
    __asm__ __volatile__("fence rw, rw" ::: "memory");
}

static void panic_write_end(void)
{
    __asm__ __volatile__("fence rw, rw" ::: "memory");
    g_esp_riscv_trace_snapshot.write_seq++;
}

void esp_riscv_trace_snapshot_panic_write(const esp_riscv_trace_snapshot_panic_core_t *cores, int count)
{
    panic_write_begin();

    bool any_capturing = false;
    for (int core_id = 0; core_id < count; core_id++) {
        const esp_riscv_trace_snapshot_panic_core_t *cs = &cores[core_id];
        /* Replace state possibly left by an interrupted normal writer. */
        if (!cs->present) {
            continue;
        }

        esp_riscv_trace_snapshot_core_desc_t *desc = &s_core_desc[core_id];
        desc->state = cs->state;
        desc->fifo_status_raw = cs->fifo_status_raw;
        desc->intr_status_raw = cs->intr_status_raw;
        desc->fifo_empty = cs->fifo_empty ? 1 : 0;
        desc->memory_full = cs->memory_full ? 1 : 0;
        desc->fifo_overflow = cs->fifo_overflow ? 1 : 0;
        desc->head_offset = cs->head_valid ? cs->head_offset : 0;
        desc->head_valid = cs->head_valid ? 1 : 0;

        if (cs->capturing) {
            any_capturing = true;
        }
    }

    if (any_capturing) {
        g_esp_riscv_trace_snapshot.capture_reason = ESP_RISCV_TRACE_CAPTURE_REASON_PANIC;
    }

    panic_write_end();
}
