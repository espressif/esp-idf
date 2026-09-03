/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Attachment interface for adding extra ELF program segments and notes to a
 * coredump. A data-owning component registers a write callback with
 * ESP_COREDUMP_REGISTER_EXTRA(); the coredump writer walks that list on every
 * ELF pass. Each callback must add the same segments and notes in the same
 * order on every pass, and must not allocate.
 */

/** @brief Opaque coredump attachment sink handed to extra-write callbacks. */
typedef struct core_dump_sink_s core_dump_sink_t;

/**
 * @brief Add one PT_LOAD segment covering [vaddr, vaddr+size).
 *
 * @param sink       Sink passed to the callback.
 * @param vaddr      Target address recorded in the program header.
 * @param data       Source bytes read during the data pass. Must stay valid and
 *                   readable for the whole coredump write.
 * @param size       Segment byte count. Must be a multiple of 4 (the ELF writer
 *                   reads a 4-byte-aligned length).
 * @param out_index  Receives the zero-based program-header index, or NULL.
 * @return ESP_OK, or an error that makes the coredump writer abort the attachment.
 */
esp_err_t esp_core_dump_sink_add_segment(core_dump_sink_t *sink, uint32_t vaddr,
                                         const void *data, uint32_t size, uint32_t *out_index);

/**
 * @brief Add one ELF note to the shared attachment PT_NOTE segment.
 *
 * @param sink       Sink passed to the callback.
 * @param name       Note name (a terminating null is added and counted).
 * @param type       Note type.
 * @param desc       Note description bytes. Must stay valid for the whole write.
 * @param desc_size  Description byte count.
 * @return ESP_OK, or an error that makes the coredump writer abort the attachment.
 */
esp_err_t esp_core_dump_sink_add_note(core_dump_sink_t *sink, const char *name,
                                      uint32_t type, const void *desc, uint32_t desc_size);

/**
 * @brief True when the ELF writer is in ELF_STAGE_PLACE_DATA.
 *
 * Earlier stages only count sizes and write headers.
 */
bool esp_core_dump_sink_is_data_stage(const core_dump_sink_t *sink);

/** @brief Extra-content callback invoked once per ELF write pass. */
typedef void (*esp_core_dump_extra_cb_t)(core_dump_sink_t *sink);

/**
 * @brief Register @p fn as a coredump extra-content callback.
 *
 * The linker collects every registration into one array that the ELF writer
 * walks. Place this in the same translation unit as @p fn.
 */
#define ESP_COREDUMP_REGISTER_EXTRA(fn) \
    static const esp_core_dump_extra_cb_t _esp_coredump_extra_##fn \
    __attribute__((used, section(".esp_coredump_extra"))) = (fn)

#ifdef __cplusplus
}
#endif
