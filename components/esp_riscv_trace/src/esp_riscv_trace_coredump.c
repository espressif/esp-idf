/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Writes the frozen snapshot as one ELF note and one PT_LOAD per stable core. */

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "esp_attr.h"
#include "esp_memory_utils.h"
#include "hal/cache_hal.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_core_dump_extension.h"
#include "esp_riscv_trace_snapshot.h"
#include "esp_riscv_trace_priv.h"
#include "esp_log.h"

static const char *TAG = "esp_riscv_trace_coredump";

#define ESP_RISCV_TRACE_NOTE_NAME        "ESP_RISCV_TRACE"
#define ESP_RISCV_TRACE_NOTE_TYPE        680
#define ESP_RISCV_TRACE_NOTE_HEADER_SIZE 80
#define ESP_RISCV_TRACE_NOTE_RECORD_SIZE 44

/* Record has no PT_LOAD segment. */
#define ESP_RISCV_TRACE_NOTE_NO_SEGMENT  0xFFFFFFFF

static inline void put_u16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
}

static inline void put_u32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static bool range_readable(uint32_t addr, uint32_t size)
{
    if (addr == 0 || size == 0 || addr + size < addr) {
        return false;
    }
    const void *first = (const void *)(uintptr_t)addr;
    const void *last = (const void *)(uintptr_t)(addr + size - 1);
    /* Both addresses must fall in one region. */
    bool internal = esp_ptr_internal(first) && esp_ptr_internal(last);
    bool psram = esp_ptr_external_ram(first) && esp_ptr_external_ram(last);
    bool drom = esp_ptr_in_drom(first) && esp_ptr_in_drom(last);
    return internal || psram || drom;
}

static bool core_buffer_stable(const esp_riscv_trace_snapshot_core_desc_t *cd)
{
    if (cd->state != ESP_RISCV_TRACE_SNAPSHOT_STATE_STOPPED && cd->state != ESP_RISCV_TRACE_SNAPSHOT_STATE_FROZEN) {
        return false;
    }
    if (!cd->head_valid) {
        return false;
    }
    /* Bytes up to head_offset are already committed to memory. A non-empty
     * FIFO at freeze time only means the tail may be truncated. */
    if (!cd->fifo_empty) {
        ESP_DRAM_LOGD(TAG, "core %d FIFO not empty at freeze (tail may be truncated, head=%u/%u)",
                      (int)cd->core_id, (unsigned)cd->head_offset, (unsigned)cd->capacity);
    }
    return range_readable(cd->buffer_addr, cd->capacity);
}

/* Reserved for every CPU. add_note writes only the filled header and records. */
static uint8_t s_note[ESP_RISCV_TRACE_NOTE_HEADER_SIZE + SOC_CPU_CORES_NUM * ESP_RISCV_TRACE_NOTE_RECORD_SIZE];

static void serialize_note_header(uint8_t *hdr, const esp_riscv_trace_snapshot_desc_t *snap, uint8_t core_count)
{
    memset(hdr, 0, ESP_RISCV_TRACE_NOTE_HEADER_SIZE);
    memcpy(hdr + 20, (const void *)(uintptr_t)snap->app_elf_sha256_addr, 32);
    memcpy(hdr + 52, (const void *)(uintptr_t)snap->encoder_params_addr, ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE);

    put_u32(hdr + 0, snap->magic);
    put_u32(hdr + 4, snap->write_seq);
    put_u16(hdr + 8, snap->target_id);
    put_u16(hdr + 10, snap->chip_revision);
    hdr[12] = (uint8_t)snap->abi_major;
    hdr[13] = (uint8_t)snap->abi_minor;
    hdr[14] = ESP_RISCV_TRACE_NOTE_HEADER_SIZE;
    hdr[15] = ESP_RISCV_TRACE_NOTE_RECORD_SIZE;
    hdr[16] = core_count;
    hdr[17] = (uint8_t)snap->capture_reason;
    hdr[18] = 32;
    hdr[19] = ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE;
}

static void serialize_note_record(uint8_t *rec, const esp_riscv_trace_snapshot_core_desc_t *cd,
                                  uint32_t segment_index)
{
    memset(rec, 0, ESP_RISCV_TRACE_NOTE_RECORD_SIZE);

    bool present = segment_index != ESP_RISCV_TRACE_NOTE_NO_SEGMENT;
    rec[0] = (uint8_t)cd->core_id;
    rec[1] = (uint8_t)cd->state;
    rec[2] = (uint8_t)cd->memory_mode;
    rec[3] = (uint8_t)cd->packet_format;
    rec[4] = (uint8_t)cd->address_mode;
    rec[5] = (uint8_t)cd->resync_mode;
    rec[6] = 0; /* Unused ABI slot. */
    rec[7] = cd->head_valid ? 1 : 0;
    rec[8] = cd->fifo_empty ? 1 : 0;
    rec[9] = cd->memory_full ? 1 : 0;
    rec[10] = cd->fifo_overflow ? 1 : 0;
    put_u32(rec + 12, segment_index);
    put_u32(rec + 16, present ? cd->buffer_addr : 0);
    put_u32(rec + 20, present ? cd->capacity : 0);
    put_u32(rec + 24, present ? cd->capacity : 0);
    put_u32(rec + 28, cd->head_offset);
    put_u32(rec + 32, cd->resync_threshold);
    put_u32(rec + 36, cd->fifo_status_raw);
    put_u32(rec + 40, cd->intr_status_raw);
}

static bool validate_snapshot_for_coredump(const esp_riscv_trace_snapshot_desc_t *snap,
                                           uint16_t *core_count)
{
    if (snap->magic != ESP_RISCV_TRACE_SNAPSHOT_MAGIC) {
        return false;
    }
    if (snap->abi_major != ESP_RISCV_TRACE_SNAPSHOT_ABI_MAJOR ||
            snap->snapshot_desc_size != ESP_RISCV_TRACE_SNAPSHOT_DESC_SIZE ||
            snap->core_desc_size != ESP_RISCV_TRACE_SNAPSHOT_CORE_DESC_SIZE) {
        return false;
    }
    /* Odd write_seq means a writer is still updating the snapshot. */
    if (snap->write_seq & 1u) {
        return false;
    }
    if (snap->cores_addr == 0 || snap->core_count == 0) {
        return false;
    }

    uint16_t used = snap->core_count < SOC_CPU_CORES_NUM ? snap->core_count : SOC_CPU_CORES_NUM;
    if (!range_readable(snap->cores_addr, (uint32_t)used * sizeof(esp_riscv_trace_snapshot_core_desc_t))) {
        return false;
    }

    *core_count = used;
    return true;
}

static void esp_riscv_trace_coredump_write(core_dump_sink_t *sink)
{
    /* Flush the encoder FIFOs and finalize the snapshot before reading it. */
    esp_riscv_trace_snapshot_finalize();

    const esp_riscv_trace_snapshot_desc_t *snap = &g_esp_riscv_trace_snapshot;

    uint16_t core_count;
    if (!validate_snapshot_for_coredump(snap, &core_count)) {
        return;
    }

    const esp_riscv_trace_snapshot_core_desc_t *cores =
        (const esp_riscv_trace_snapshot_core_desc_t *)(uintptr_t)snap->cores_addr;

    uint32_t seg_index[SOC_CPU_CORES_NUM];
    const esp_riscv_trace_snapshot_core_desc_t *records[SOC_CPU_CORES_NUM];
    int count = 0;

    for (uint16_t i = 0; i < core_count; i++) {
        const esp_riscv_trace_snapshot_core_desc_t *cd = &cores[i];
        if (cd->state == ESP_RISCV_TRACE_SNAPSHOT_STATE_UNAVAILABLE) {
            continue;
        }
        seg_index[count] = ESP_RISCV_TRACE_NOTE_NO_SEGMENT;
        if (core_buffer_stable(cd)) {
            if (esp_core_dump_sink_is_data_stage(sink)) {
                /* Only the data stage reads the buffer. */
                cache_hal_invalidate_addr(cd->buffer_addr, cd->capacity);
            }
            if (esp_core_dump_sink_add_segment(sink, cd->buffer_addr, (const void *)(uintptr_t)cd->buffer_addr,
                                               cd->capacity, &seg_index[count]) != ESP_OK) {
                return;
            }
        }
        records[count] = cd;
        count++;
    }

    if (count == 0) {
        return;
    }

    serialize_note_header(s_note, snap, (uint8_t)count);
    for (int i = 0; i < count; i++) {
        serialize_note_record(
            s_note + ESP_RISCV_TRACE_NOTE_HEADER_SIZE + (size_t)i * ESP_RISCV_TRACE_NOTE_RECORD_SIZE,
            records[i], seg_index[i]);
    }
    uint32_t note_size = ESP_RISCV_TRACE_NOTE_HEADER_SIZE + (uint32_t)count * ESP_RISCV_TRACE_NOTE_RECORD_SIZE;
    esp_core_dump_sink_add_note(sink, ESP_RISCV_TRACE_NOTE_NAME, ESP_RISCV_TRACE_NOTE_TYPE, s_note, note_size);
}

ESP_COREDUMP_REGISTER_EXTRA(esp_riscv_trace_coredump_write);

void esp_riscv_trace_coredump_include_func(void)
{
    // Hook to force the linker to include this file
}
