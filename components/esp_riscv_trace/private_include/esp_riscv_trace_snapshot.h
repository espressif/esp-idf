/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
 * RISC-V trace snapshot target-memory ABI v1.0
 */

#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Magic "RVTS" and ABI version. */
#define ESP_RISCV_TRACE_SNAPSHOT_MAGIC      ((uint32_t)0x53545652)
#define ESP_RISCV_TRACE_SNAPSHOT_ABI_MAJOR  ((uint16_t)1)
#define ESP_RISCV_TRACE_SNAPSHOT_ABI_MINOR  ((uint16_t)0)

/* ABI 1.0 structure sizes. */
#define ESP_RISCV_TRACE_SNAPSHOT_DESC_SIZE      32
#define ESP_RISCV_TRACE_SNAPSHOT_CORE_DESC_SIZE 36
#define ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE     28
#define ESP_RISCV_TRACE_ENCODER_PARAMS_VERSION  1

/* Reason the capture stopped. */
typedef enum {
    ESP_RISCV_TRACE_CAPTURE_REASON_UNKNOWN       = 0,
    ESP_RISCV_TRACE_CAPTURE_REASON_EXPLICIT_STOP = 1,
    ESP_RISCV_TRACE_CAPTURE_REASON_PANIC         = 2,
} esp_riscv_trace_capture_reason_t;

/* Trace packet format. */
typedef enum {
    ESP_RISCV_TRACE_PACKET_FORMAT_UNKNOWN = 0,
    ESP_RISCV_TRACE_PACKET_FORMAT_PT10    = 100, /**< Processor Trace v1.0 */
    ESP_RISCV_TRACE_PACKET_FORMAT_ET20    = 200, /**< Efficient Trace v2.0 */
} esp_riscv_trace_packet_format_t;

/* Per-core snapshot state. */
typedef enum {
    ESP_RISCV_TRACE_SNAPSHOT_STATE_UNAVAILABLE  = 0,
    ESP_RISCV_TRACE_SNAPSHOT_STATE_READY        = 1,
    ESP_RISCV_TRACE_SNAPSHOT_STATE_CAPTURING    = 2,
    ESP_RISCV_TRACE_SNAPSHOT_STATE_STOPPED      = 3,
    ESP_RISCV_TRACE_SNAPSHOT_STATE_FROZEN       = 4,
} esp_riscv_trace_snapshot_state_t;

/* Trace-buffer mode. */
typedef enum {
    ESP_RISCV_TRACE_MEMORY_MODE_UNKNOWN = 0,
    ESP_RISCV_TRACE_MEMORY_MODE_LINEAR  = 1,
    ESP_RISCV_TRACE_MEMORY_MODE_LOOP    = 2,
} esp_riscv_trace_snapshot_memory_mode_t;

/* Instruction-address encoding. */
typedef enum {
    ESP_RISCV_TRACE_ADDRESS_MODE_UNKNOWN = 0,
    ESP_RISCV_TRACE_ADDRESS_MODE_DELTA   = 1,
    ESP_RISCV_TRACE_ADDRESS_MODE_FULL    = 2,
} esp_riscv_trace_snapshot_address_mode_t;

/* Hardware resynchronization mode reported for a core. */
typedef enum {
    ESP_RISCV_TRACE_RESYNC_MODE_UNKNOWN  = 0,
    ESP_RISCV_TRACE_RESYNC_MODE_DISABLED = 1,
    ESP_RISCV_TRACE_RESYNC_MODE_PACKET   = 2,
    ESP_RISCV_TRACE_RESYNC_MODE_CYCLE    = 3,
} esp_riscv_trace_snapshot_resync_mode_t;

/*
 * Snapshot object graph in target memory:
 *
 * g_esp_riscv_trace_snapshot (esp_riscv_trace_snapshot_desc_t)
 *   |-- cores_addr --> esp_riscv_trace_snapshot_core_desc_t[core_count]
 *   |     `-- buffer_addr --> raw trace bytes
 *   `-- encoder_params_addr --> esp_riscv_trace_encoder_params_t (shared by all cores)
 */

/* Trace encoder parameters from the target TRM. */
typedef struct {
    uint8_t params_version;      /*!< 1 */
    uint8_t params_size;         /*!< 28 */
    uint8_t arch_p;              /*!< Architecture version */
    uint8_t bpred_size_p;        /*!< Branch prediction mode */
    uint8_t cache_size_p;        /*!< Jump target cache mode */
    uint8_t call_counter_size_p; /*!< Implicit return mode */
    uint8_t ctype_width_p;       /*!< Width of the ctype bus */
    uint8_t context_width_p;     /*!< Width of the context bus */
    uint8_t ecause_width_p;      /*!< Width of the exception cause */
    uint8_t ecause_choice_p;     /*!< Multiple ecause choice */
    uint8_t f0s_width_p;         /*!< Format 0 packets */
    uint8_t filter_context_p;    /*!< Filtering on context */
    uint8_t filter_excint_p;     /*!< Filtering on exception cause or interrupt */
    uint8_t filter_privilege_p;  /*!< Filtering on privilege */
    uint8_t filter_tval_p;       /*!< Filtering on trap value */
    uint8_t iaddress_lsb_p;      /*!< Compressed instructions (address is PC >> 1) */
    uint8_t iaddress_width_p;    /*!< Instruction bus width */
    uint8_t iretire_width_p;     /*!< Width of the iretire bus */
    uint8_t ilastsize_width_p;   /*!< Width of ilastsize */
    uint8_t itype_width_p;       /*!< Width of the itype bus */
    uint8_t nocontext_p;         /*!< Exclude context from te_inst packets */
    uint8_t notime_p;            /*!< Exclude time from te_inst packets */
    uint8_t privilege_width_p;   /*!< Width of the privilege field */
    uint8_t retires_p;           /*!< Max instructions retired per block */
    uint8_t return_stack_size_p; /*!< Implicit return mode */
    uint8_t sijump_p;            /*!< Sequentially inferable jump mode */
    uint8_t taken_branches_p;    /*!< Instructions retired per cycle */
    uint8_t impdef_width_p;      /*!< Implementation-defined field */
} esp_riscv_trace_encoder_params_t;

static_assert(sizeof(esp_riscv_trace_encoder_params_t) == ESP_RISCV_TRACE_ENCODER_PARAMS_SIZE,
              "encoder params block must be 28 bytes");

/* Core configuration and capture state. */
typedef struct {
    uint32_t buffer_addr;         /*!< Target address of the trace buffer */
    uint32_t capacity;            /*!< Trace-buffer size in bytes */
    uint32_t head_offset;         /*!< Next hardware write offset */
    uint32_t resync_threshold;    /*!< Resync interval reported by hardware. Zero when not applicable */
    uint32_t fifo_status_raw;     /*!< Unmodified hardware status register */
    uint32_t intr_status_raw;     /*!< Unmodified hardware interrupt status */
    uint8_t  core_id;             /*!< Traced hart ID */
    uint8_t  state;               /*!< esp_riscv_trace_snapshot_state_t */
    uint8_t  memory_mode;         /*!< esp_riscv_trace_snapshot_memory_mode_t */
    uint8_t  packet_format;       /*!< esp_riscv_trace_packet_format_t */
    uint8_t  address_mode;        /*!< esp_riscv_trace_snapshot_address_mode_t */
    uint8_t  resync_mode;         /*!< esp_riscv_trace_snapshot_resync_mode_t */
    uint8_t  reserved0;           /*!< Unused. Stays zero */
    uint8_t  head_valid;          /*!< 1 when head_offset is the hardware write position */
    uint8_t  fifo_empty;          /*!< 1 when the encoder FIFO was empty after stop or freeze */
    uint8_t  memory_full;         /*!< 1 when the memory-full interrupt was set */
    uint8_t  fifo_overflow;       /*!< 1 when the FIFO-overflow interrupt was set */
    uint8_t  reserved1;           /*!< Keeps the descriptor 4-byte aligned */
} esp_riscv_trace_snapshot_core_desc_t;

static_assert(sizeof(esp_riscv_trace_snapshot_core_desc_t) == ESP_RISCV_TRACE_SNAPSHOT_CORE_DESC_SIZE,
              "core descriptor must be 36 bytes");

/* Snapshot root descriptor. */
typedef struct {
    uint32_t magic;                /*!< ESP_RISCV_TRACE_SNAPSHOT_MAGIC. Must be written last */
    uint32_t write_seq;            /*!< Odd while the snapshot is changing */
    uint32_t cores_addr;           /*!< Target address of the core array */
    uint32_t app_elf_sha256_addr;  /*!< Target address of the raw ELF SHA-256 bytes */
    uint32_t encoder_params_addr;  /*!< Target address of the shared encoder-parameter block */
    uint16_t target_id;            /*!< ESP image chip ID */
    uint16_t chip_revision;        /*!< Major times 100 plus minor */
    uint8_t  abi_major;            /*!< 1 */
    uint8_t  abi_minor;            /*!< 0 */
    uint8_t  snapshot_desc_size;   /*!< 32 */
    uint8_t  core_desc_size;       /*!< 36 */
    uint8_t  core_count;           /*!< Number of contiguous core entries */
    uint8_t  capture_reason;       /*!< esp_riscv_trace_capture_reason_t */
    uint8_t  app_elf_sha256_size;  /*!< Zero or 32 */
    uint8_t  encoder_params_size;  /*!< Encoder-parameter block size in bytes */
} esp_riscv_trace_snapshot_desc_t;

static_assert(sizeof(esp_riscv_trace_snapshot_desc_t) == ESP_RISCV_TRACE_SNAPSHOT_DESC_SIZE,
              "snapshot descriptor must be 32 bytes");
extern esp_riscv_trace_snapshot_desc_t g_esp_riscv_trace_snapshot;

#ifdef __cplusplus
}
#endif
