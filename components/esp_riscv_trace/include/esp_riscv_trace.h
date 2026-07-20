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

/** @brief RISC-V core whose execution is traced. */
typedef enum {
    ESP_RISCV_TRACE_CORE_0 = 0,
    ESP_RISCV_TRACE_CORE_1 = 1,
} esp_riscv_trace_core_t;

/** @brief Instruction address encoding in the trace stream. */
typedef enum {
    ESP_RISCV_TRACE_ADDR_DELTA = 0, /*!< Differential addresses (hardware reset default) */
    ESP_RISCV_TRACE_ADDR_FULL  = 1, /*!< Full addresses */
} esp_riscv_trace_address_mode_t;

/** @brief Trace buffer memory mode. */
typedef enum {
    ESP_RISCV_TRACE_MEM_NON_LOOP = 0, /*!< Stop when buffer is full */
    ESP_RISCV_TRACE_MEM_LOOP,         /*!< Wrap around (hardware reset default) */
} esp_riscv_trace_mem_mode_t;

/** @brief Resynchronization mode. */
typedef enum {
    ESP_RISCV_TRACE_RESYNC_DISABLED = 0, /*!< No periodic resync (hardware reset default) */
    ESP_RISCV_TRACE_RESYNC_PACKET   = 2, /*!< Resync by packet count */
    ESP_RISCV_TRACE_RESYNC_CYCLE    = 3, /*!< Resync by cycle count */
} esp_riscv_trace_resync_mode_t;

/**
 * @brief AHB burst type used by the trace write master.
 *
 * These are the trace IP's custom hburst field codes, NOT the standard AMBA
 * HBURST encoding: 0=SINGLE, 1=INCR, 2=INCR4, 4=INCR8; values 3, 5, 6, 7 are
 * invalid. Do not "correct" 2/4 to the AMBA INCR4/INCR8 codes (3/5).
 */
typedef enum {
    ESP_RISCV_TRACE_AHB_SINGLE = 0, /*!< Single transfer (hardware reset default) */
    ESP_RISCV_TRACE_AHB_INCR   = 1, /*!< Incrementing transfer (length not defined) */
    ESP_RISCV_TRACE_AHB_INCR4  = 2, /*!< 4-beat incrementing transfer */
    ESP_RISCV_TRACE_AHB_INCR8  = 4, /*!< 8-beat incrementing transfer */
} esp_riscv_trace_ahb_burst_t;

/** @brief Where the driver allocates the trace buffer. */
typedef enum {
    ESP_RISCV_TRACE_BUFFER_INTERNAL = 0, /*!< Internal RAM / L2MEM (hardware reset default) */
    ESP_RISCV_TRACE_BUFFER_EXTERNAL = 1, /*!< External RAM / PSRAM */
} esp_riscv_trace_buffer_mem_t;

/** @brief Filter comparator input. */
typedef enum {
    ESP_RISCV_TRACE_FILTER_INPUT_IADDR = 0, /*!< Compare the instruction address (PC) */
    ESP_RISCV_TRACE_FILTER_INPUT_TVAL  = 1, /*!< Compare the trap value (tval) */
} esp_riscv_trace_filter_input_t;

/** @brief Filter comparator function. */
typedef enum {
    ESP_RISCV_TRACE_FILTER_COMPARATOR_EQ = 0, /*!< input == match_value */
    ESP_RISCV_TRACE_FILTER_COMPARATOR_NE = 1, /*!< input != match_value */
    ESP_RISCV_TRACE_FILTER_COMPARATOR_LT = 2, /*!< input <  match_value */
    ESP_RISCV_TRACE_FILTER_COMPARATOR_LE = 3, /*!< input <= match_value */
    ESP_RISCV_TRACE_FILTER_COMPARATOR_GT = 4, /*!< input >  match_value */
    ESP_RISCV_TRACE_FILTER_COMPARATOR_GE = 5, /*!< input >= match_value */
} esp_riscv_trace_filter_comparator_func_t;

/** @brief How the primary (P) and secondary (S) comparators combine. */
typedef enum {
    ESP_RISCV_TRACE_FILTER_MODE_PRIMARY = 0, /*!< Primary comparator only */
    ESP_RISCV_TRACE_FILTER_MODE_AND     = 1, /*!< P && S */
    ESP_RISCV_TRACE_FILTER_MODE_NAND    = 2, /*!< !(P && S) */
    ESP_RISCV_TRACE_FILTER_MODE_RANGE   = 3, /*!< Start when P matches, continue until S matches */
} esp_riscv_trace_filter_mode_t;

/** @brief Privilege level selected by the privilege qualifier. */
typedef enum {
    ESP_RISCV_TRACE_FILTER_PRIV_USER    = 0, /*!< User mode */
    ESP_RISCV_TRACE_FILTER_PRIV_MACHINE = 1, /*!< Machine mode */
} esp_riscv_trace_filter_priv_t;

/** @brief One filter comparator. */
typedef struct {
    esp_riscv_trace_filter_input_t input;              /*!< Input to compare (iaddr or tval) */
    esp_riscv_trace_filter_comparator_func_t function; /*!< Compare function */
    uint32_t match_value;                     /*!< 32-bit value compared against the input */
    bool notify;                              /*!< Emit a packet reporting the matching address */
} esp_riscv_trace_filter_comparator_t;

/**
 * @brief Filter (trace qualifier) configuration.
 *
 * The filter restricts which execution produces trace packets. With .enable = false the encoder
 * traces everything (the default). Apply via esp_riscv_trace_set_filter() before a capture.
 */
typedef struct {
    bool enable;                                   /*!< Master enable; false = trace everything */
    bool match_comparators;                        /*!< Gate matching on the comparators below */
    esp_riscv_trace_filter_comparator_t primary;   /*!< Primary (P) comparator */
    esp_riscv_trace_filter_comparator_t secondary; /*!< Secondary (S) comparator */
    esp_riscv_trace_filter_mode_t mode;            /*!< How P and S combine */
    bool match_privilege;                          /*!< Gate matching on the privilege level */
    esp_riscv_trace_filter_priv_t privilege;       /*!< Privilege level to match */
    bool match_ecause;                             /*!< Match from an exception cause */
    uint8_t ecause;                                /*!< Exception cause code (6-bit) */
    bool match_interrupt;                          /*!< Match from an interrupt trap */
    bool interrupt_itype2;                         /*!< true = match itype 2, false = itype 1 */
} esp_riscv_trace_filter_config_t;

/**
 * @brief Configuration for a trace encoder instance.
 *
 * @note On targets without SOC_RISCV_TRACE_HAS_CONFIG_REG (address_mode, stall_cpu,
 *       halt_enable, reset_enable, debug_trigger_enable) or without SOC_RISCV_TRACE_AHB_CONFIGURABLE
 *       (ahb_burst, ahb_max_incr), those fields are accepted but ignored. The encoder keeps its
 *       fixed default behavior.
 */
typedef struct {
    uint32_t core_mask;                         /*!< Bitmask of cores to trace (BIT(n) for core n) */
    size_t buffer_size;                         /*!< Trace buffer size in bytes (driver-allocated) */
    esp_riscv_trace_buffer_mem_t buffer_mem;    /*!< Where the driver allocates the buffer */
    esp_riscv_trace_address_mode_t address_mode; /*!< Instruction address encoding in the trace stream. */
    esp_riscv_trace_mem_mode_t mem_mode;         /*!< Trace buffer memory mode */
    bool auto_restart;                          /*!< Auto-restart encoder after a FIFO overflow */
    bool stall_cpu;                             /*!< Stall CPU instead of dropping packets on FIFO full */
    bool halt_enable;                           /*!< Trace through hart halt */
    bool reset_enable;                          /*!< Trace through hart reset */
    bool debug_trigger_enable;                  /*!< Enable Debug Module trigger input */
    esp_riscv_trace_resync_mode_t resync_mode;  /*!< Resynchronization mode */
    uint32_t resync_threshold;                  /*!< Resync threshold. (default 128) */
    esp_riscv_trace_ahb_burst_t ahb_burst;     /*!< AHB burst type used by the trace write master. */
    uint8_t ahb_max_incr;                       /*!< Max INCR burst beats. (default 0) */
    uint32_t intr_mask;                         /*!< Interrupt bits to enable for polling via get_intr_status() */
} esp_riscv_trace_config_t;

/**
 * @brief Default trace encoder configuration built from Kconfig values.
 *
 * Use this to get a ready-to-use configuration without filling every field by hand,
 * then override individual members afterwards if needed.
 *
 * Example:
 * @code{c}
 *     esp_riscv_trace_config_t config = ESP_RISCV_TRACE_DEFAULT_CONFIG();
 * @endcode
 */
#define ESP_RISCV_TRACE_DEFAULT_CONFIG() {                                  \
    .core_mask            = CONFIG_ESP_RISCV_TRACE_CORE_MASK,               \
    .buffer_size          = CONFIG_ESP_RISCV_TRACE_BUFFER_SIZE,            \
    .buffer_mem           = CONFIG_ESP_RISCV_TRACE_BUFFER_MEM,             \
    .address_mode         = CONFIG_ESP_RISCV_TRACE_ADDRESS_MODE,           \
    .mem_mode             = CONFIG_ESP_RISCV_TRACE_MEM_MODE,               \
    .auto_restart         = CONFIG_ESP_RISCV_TRACE_AUTO_RESTART,           \
    .stall_cpu            = CONFIG_ESP_RISCV_TRACE_STALL_CPU,              \
    .halt_enable          = CONFIG_ESP_RISCV_TRACE_HALT,                   \
    .reset_enable         = CONFIG_ESP_RISCV_TRACE_RESET,                  \
    .debug_trigger_enable = CONFIG_ESP_RISCV_TRACE_DEBUG_TRIGGER,          \
    .resync_mode          = CONFIG_ESP_RISCV_TRACE_RESYNC_MODE,            \
    .resync_threshold     = CONFIG_ESP_RISCV_TRACE_RESYNC_THRESHOLD,       \
    .ahb_burst            = CONFIG_ESP_RISCV_TRACE_AHB_BURST,              \
    .ahb_max_incr         = CONFIG_ESP_RISCV_TRACE_AHB_MAX_INCR,           \
    .intr_mask            = 0,                                             \
}

/** @brief Encoder work status. */
typedef enum {
    ESP_RISCV_TRACE_WORK_STATUS_IDLE    = 0,
    ESP_RISCV_TRACE_WORK_STATUS_WORKING = 1,
    ESP_RISCV_TRACE_WORK_STATUS_WAIT    = 2,
    ESP_RISCV_TRACE_WORK_STATUS_LOST    = 3,
} esp_riscv_trace_work_status_t;

/** @brief Live encoder status snapshot. */
typedef struct {
    esp_riscv_trace_work_status_t work_status;
    bool fifo_empty;
    bool memory_full;
    bool fifo_overflowed;
} esp_riscv_trace_status_t;

/**
 * @note RISC-V trace driver APIs are serialized per trace core and are intended
 *       for task context. Do not call them from ISR context.
 */

/**
 * @brief Start tracing on a specified core.
 *
 * @param core_id      Core whose trace to start
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if the core is not initialized
 */
esp_err_t esp_riscv_trace_start(esp_riscv_trace_core_t core_id);

/**
 * @brief Stop tracing on a specified core and wait for the FIFO to empty.
 *
 * @param core_id      Core whose trace to stop
 * @param timeout_us   Timeout in microseconds
 * @return ESP_OK on success, ESP_ERR_TIMEOUT if the FIFO is not empty within the timeout
 */
esp_err_t esp_riscv_trace_stop(esp_riscv_trace_core_t core_id, uint32_t timeout_us);

/**
 * @brief Get a core's trace buffer.
 *
 * @param core_id      Core whose buffer to retrieve
 * @param buffer       Out: buffer base address
 * @param capacity     Out: total buffer size in bytes
 * @param head_offset  Out: current write offset within the buffer [0, capacity]
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if the core is not initialized
 */
esp_err_t esp_riscv_trace_get_buffer(esp_riscv_trace_core_t core_id, uint8_t **buffer,
                                     size_t *capacity, size_t *head_offset);

/**
 * @brief Get the status of a specified core's trace encoder.
 *
 * @param core_id      Core whose status to retrieve
 * @param status       Out: status snapshot
 * @return ESP_OK on success, ESP_ERR_INVALID_STATE if the core is not initialized
 */
esp_err_t esp_riscv_trace_get_status(esp_riscv_trace_core_t core_id, esp_riscv_trace_status_t *status);

/**
 * @brief Apply a filter (trace qualifier) to a core's encoder.
 *
 * @param core_id  Core whose filter to configure
 * @param config   Filter configuration
 * @return ESP_ERR_NOT_SUPPORTED on targets without a filter unit
 */
esp_err_t esp_riscv_trace_set_filter(esp_riscv_trace_core_t core_id, const esp_riscv_trace_filter_config_t *config);

/**
 * @brief Provide the per-core configuration used by the startup auto-initialization.
 *
 * Called once per core during startup, so each trace encoder can be configured independently
 * (different buffer, address mode, resync, etc.). The default (weak) implementation returns
 * ESP_RISCV_TRACE_DEFAULT_CONFIG() for every core. Define your own strong version in your
 * application (typically switching on @p core_id) to override the configuration.
 *
 * Only the @p core_id bit of the returned .core_mask is checked for this call: clear it to skip
 * tracing this core at runtime even if it is selected by Kconfig.
 *
 * @param core_id  Core the returned configuration applies to
 * @return The configuration the startup auto-init uses for @p core_id.
 */
esp_riscv_trace_config_t esp_riscv_trace_get_user_config(int core_id);

#ifdef __cplusplus
}
#endif
