/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BLE_LOG_TASK_REGISTRY_H__
#define __BLE_LOG_TASK_REGISTRY_H__

/* ------------------------------------ */
/*     BLE Log - Task-id Registry       */
/* ------------------------------------ */
/* Self-contained module (one .c/.h pair, like the UART redirection
 * writer): owns the name-to-id registry, its broadcast sequence and its
 * dedicated transport, so it never contends with the internal snapshot
 * transport or the pool. */

/* INCLUDE */
#include "ble_log_lbm_v2.h"

/* ------------------------- */
/*     Wire Format (v8)      */
/* ------------------------- */
/* Writer attribution: every ENCODE record names the task that formatted
 * it (one byte after the record source); the registry maps task names to
 * stable ids. The name width is a wire property of the binding record
 * below. */
#define BLE_LOG_TASK_ID_UNKNOWN                 UINT8_C(0xff)
#define BLE_LOG_TASK_NAME_LEN                   (16U)

/* The binding record inside an INTERNAL frame: every periodic window
 * broadcasts one frame that packs one record per registered entry, in id
 * order. The name is NUL-padded to the registry width plus one byte, so
 * a decoder can bind the id in place even when the task name fills the
 * whole field. */
typedef struct {
    uint8_t int_src_code;
    uint8_t task_id;
    uint8_t task_name[BLE_LOG_TASK_NAME_LEN + 1];
} __attribute__((packed)) ble_log_task_binding_t;

/* One binding frame must fit the dedicated transport in a single frame:
 * one submit per window, and the transport is recycled only when the
 * peripheral has read it. */
#define BLE_LOG_TASK_BINDING_FRAME_LEN          \
    (BLE_LOG_FRAME_OVERHEAD + sizeof(uint32_t) + \
     CONFIG_BLE_LOG_TASK_ID_MAX * sizeof(ble_log_task_binding_t))
/* Word-aligned by construction (SPI builds require aligned transfers). */
#define BLE_LOG_TASK_BINDING_TRANS_SIZE         \
    ((BLE_LOG_TASK_BINDING_FRAME_LEN + 3U) & ~3U)

_Static_assert(sizeof(ble_log_task_binding_t) == 19,
               "Unexpected task-binding record layout");

/* --------------------------- */
/*     Internal Interfaces     */
/* --------------------------- */
/* Module lifetime, driven by the LBM layer (ble_log_lbm_init/deinit):
 * init allocates the dedicated transport and wipes the registry and its
 * sequence for a fresh epoch; begin_deinit closes the gate and waits for
 * in-flight publishes; deinit frees the transport. */
bool ble_log_task_registry_init(void);
void ble_log_task_registry_begin_deinit(void);
void ble_log_task_registry_deinit(void);

/* Resolves the calling task's name to its stable registry id, registering
 * the name on a miss. ISR-safe: an ISR caller reads only (a miss returns
 * the unknown id). Every ENCODE record must resolve before its claim so
 * the registry already holds the id the record carries. */
uint8_t ble_log_task_id_current(void);

/* Periodic window output: broadcasts one INTERNAL frame packing one
 * fixed-layout binding record per registered entry, in id order, on the
 * dedicated registry transport. Runs on the shared ESP timer task and
 * must never block: a busy transport (previous broadcast still in DMA)
 * skips this window and the next one rebroadcasts. Best-effort control
 * traffic, never counted in the per-source written/lost stats, and
 * carried on its own sequence (see ble_log_task_registry.c) so a skipped
 * window cannot look like a lost snapshot. */
void ble_log_task_bindings_publish(void);

#if CONFIG_BLE_LOG_PRPH_TEST
/* Test-only: wipes the task registry so test cases stay order-independent.
 * Call between cases, with no writers in flight. */
void ble_log_test_task_registry_reset(void);
#endif

#endif /* __BLE_LOG_TASK_REGISTRY_H__ */
