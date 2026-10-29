/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "soc/soc_caps.h"
#include "esp_err.h"
#include "esp_riscv_trace_snapshot.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Magic "RVOC" (RISC-V trace OpenOCD control) and ABI version. */
#define ESP_RISCV_TRACE_OCD_MAGIC           (0x434F5652)
#define ESP_RISCV_TRACE_OCD_ABI_VERSION     1

/* ABI 1 structure sizes. */
#define ESP_RISCV_TRACE_OCD_HDR_SIZE        20
#define ESP_RISCV_TRACE_OCD_CORE_SIZE       40

/* Offset of ack_seq, the only word OpenOCD writes. */
#define ESP_RISCV_TRACE_OCD_ACK_SEQ_OFFSET  32

/* Capture mode requested by OpenOCD. */
typedef enum {
    ESP_RISCV_TRACE_OCD_MODE_NONE   = 0,
    ESP_RISCV_TRACE_OCD_MODE_STREAM = 1,
    ESP_RISCV_TRACE_OCD_MODE_LOOP   = 2,
} esp_riscv_trace_ocd_mode_t;

/* Per-core capture state. */
typedef enum {
    ESP_RISCV_TRACE_OCD_STATE_IDLE      = 0,
    ESP_RISCV_TRACE_OCD_STATE_STREAMING = 1,
    ESP_RISCV_TRACE_OCD_STATE_LOOPING   = 2,
    ESP_RISCV_TRACE_OCD_STATE_STOPPED   = 3,
    ESP_RISCV_TRACE_OCD_STATE_OVERRUN   = 4,
    ESP_RISCV_TRACE_OCD_STATE_ERROR     = 5,
} esp_riscv_trace_ocd_state_t;

/* Per-core error code. */
typedef enum {
    ESP_RISCV_TRACE_OCD_ERR_NONE         = 0,
    ESP_RISCV_TRACE_OCD_ERR_OVERRUN      = 1,
    ESP_RISCV_TRACE_OCD_ERR_FIFO_OVF     = 2,
    ESP_RISCV_TRACE_OCD_ERR_HEAD_INVALID = 3,
    ESP_RISCV_TRACE_OCD_ERR_FIFO_TIMEOUT = 4,
} esp_riscv_trace_ocd_error_t;

/*
 * Object graph in target memory.
 *
 * debug stubs table[ESP_DBG_STUB_ENTRY_RISCV_TRACE]
 *   `-- g_esp_riscv_trace_ocd (esp_riscv_trace_ocd_t)
 *         |-- snapshot --> g_esp_riscv_trace_snapshot (esp_riscv_trace_snapshot_desc_t)
 *         |                  `-- cores_addr --> esp_riscv_trace_snapshot_core_desc_t[core_count]
 *         |                        `-- buffer_addr --> trace buffer
 *         |-- start, stop --> functions OpenOCD runs on a halted core
 *         `-- cores[core_id] (esp_riscv_trace_ocd_core_t)
 *               `-- bank_addr[0], bank_addr[1] --> two halves of the same trace buffer
 *
 * Streaming handshake for one core.
 *
 * The two banks are filled in turn. Each completed bank gets the next number
 * (1, 2, 3, ...), and seq is the number of the newest completed bank.
 *
 *   1. Firmware fills bank b and publishes it with bank_len[b] = bytes and bank_seq[b] = ++seq.
 *   2. OpenOCD reads every bank whose number is above ack_seq, oldest first, then writes
 *      the newest number it read to ack_seq.
 *   3. Firmware writes into bank b again only when ack_seq >= bank_seq[b]. If OpenOCD
 *      has not read it yet, firmware stops the capture with OVERRUN.
 *
 * For example, with seq = 5 and ack_seq = 4 the bank numbered 5 is waiting for OpenOCD, and the
 * other bank (numbered 4) is already read and can be refilled.
 */

/* Per-core state. Firmware writes every field except ack_seq. */
typedef struct {
    uint32_t bank_addr[2];  /*!< Target address of each ping-pong bank */
    uint32_t bank_size;     /*!< Usable bytes per bank */
    uint32_t bank_len[2];   /*!< Valid bytes of each bank's last completion */
    uint32_t bank_seq[2];   /*!< seq when each bank last completed. 0 = never */
    uint32_t seq;           /*!< Last completed seq */
    uint32_t ack_seq;       /*!< Highest seq consumed by OpenOCD */
    uint8_t  mode;          /*!< esp_riscv_trace_ocd_mode_t */
    uint8_t  state;         /*!< esp_riscv_trace_ocd_state_t */
    uint8_t  error;         /*!< esp_riscv_trace_ocd_error_t */
    uint8_t  active_bank;   /*!< Bank the encoder writes now */
} esp_riscv_trace_ocd_core_t;

static_assert(sizeof(esp_riscv_trace_ocd_core_t) == ESP_RISCV_TRACE_OCD_CORE_SIZE,
              "OpenOCD core entry must be 40 bytes");
static_assert(offsetof(esp_riscv_trace_ocd_core_t, ack_seq) == ESP_RISCV_TRACE_OCD_ACK_SEQ_OFFSET,
              "OpenOCD writes ack_seq at offset 32");

/* Descriptor header. abi_version is the layout contract with OpenOCD. */
typedef struct {
    uint32_t magic;                                   /*!< ESP_RISCV_TRACE_OCD_MAGIC */
    uint8_t  abi_version;                             /*!< ESP_RISCV_TRACE_OCD_ABI_VERSION */
    uint8_t  core_count;                              /*!< Number of entries in cores[] */
    esp_riscv_trace_snapshot_desc_t *snapshot;        /*!< Snapshot root */
    esp_err_t (*start)(uint32_t core_mask, int mode); /*!< Starts all cores in core_mask, or none */
    esp_err_t (*stop)(uint32_t core_mask);            /*!< Stops the OpenOCD captures in core_mask */
} esp_riscv_trace_ocd_hdr_t;

static_assert(sizeof(esp_riscv_trace_ocd_hdr_t) == ESP_RISCV_TRACE_OCD_HDR_SIZE,
              "OpenOCD descriptor header must be 20 bytes");

/* Root descriptor. Registered at ESP_DBG_STUB_ENTRY_RISCV_TRACE. */
typedef struct {
    esp_riscv_trace_ocd_hdr_t hdr;
    esp_riscv_trace_ocd_core_t cores[SOC_CPU_CORES_NUM];
} esp_riscv_trace_ocd_t;

extern esp_riscv_trace_ocd_t g_esp_riscv_trace_ocd;

#ifdef __cplusplus
}
#endif
