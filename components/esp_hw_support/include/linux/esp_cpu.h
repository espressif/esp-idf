/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Linux/macOS host-only version of esp_cpu.h.
 *
 * This header provides stub/minimal implementations of the esp_cpu_*
 * API for host builds.  It is placed in an include directory that takes
 * precedence over the generic esp_cpu.h for CONFIG_IDF_TARGET_LINUX.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "esp_err.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CPU cycle count type
 */
typedef uint32_t esp_cpu_cycle_count_t;

/**
 * @brief CPU interrupt type
 */
typedef enum {
    ESP_CPU_INTR_TYPE_LEVEL = 0,
    ESP_CPU_INTR_TYPE_EDGE,
    ESP_CPU_INTR_TYPE_NA,
} esp_cpu_intr_type_t;

/**
 * @brief CPU interrupt descriptor
 */
typedef struct {
    int priority;
    esp_cpu_intr_type_t type;
    uint32_t flags;
} esp_cpu_intr_desc_t;

#define ESP_CPU_INTR_DESC_FLAG_SPECIAL      0x01
#define ESP_CPU_INTR_DESC_FLAG_RESVD        0x02

/**
 * @brief CPU interrupt handler type
 */
typedef void (*esp_cpu_intr_handler_t)(void *arg);

/**
 * @brief CPU watchpoint trigger type
 */
typedef enum {
    ESP_CPU_WATCHPOINT_LOAD,
    ESP_CPU_WATCHPOINT_STORE,
    ESP_CPU_WATCHPOINT_ACCESS,
} esp_cpu_watchpoint_trigger_t;

/* --------------------------------------------------- CPU Control ----------------------------------------------------- */

/**
 * @brief Stall a CPU core
 *
 * @param core_id  The core's ID
 */
void esp_cpu_stall(int core_id);

/**
 * @brief Resume a previously stalled CPU core
 *
 * @param core_id The core's ID
 */
void esp_cpu_unstall(int core_id);

/**
 * @brief Reset a CPU core
 *
 * @param core_id The core's ID
 */
void esp_cpu_reset(int core_id);

/**
 * @brief Wait for Interrupt
 *
 * This function causes the current CPU core to execute its Wait For Interrupt
 * (WFI or equivalent) instruction. After executing this function, the CPU core
 * will stop execution until an interrupt occurs.
 */
void esp_cpu_wait_for_intr(void);

/* -------------------------------------------------- CPU Registers ---------------------------------------------------- */

/**
 * @brief Get the current core's ID
 *
 * @return Always 0 on Linux host (single core)
 */
FORCE_INLINE_ATTR __attribute__((pure)) int esp_cpu_get_core_id(void)
{
    return 0;
}

/**
 * @brief Get the current CPU core's privilege level
 *
 * @return -1 (not applicable on host)
 */
FORCE_INLINE_ATTR __attribute__((always_inline)) int esp_cpu_get_curr_privilege_level(void)
{
    return -1;
}

/**
 * @brief Read the current stack pointer address
 */
FORCE_INLINE_ATTR void *esp_cpu_get_sp(void)
{
    void *sp;
#if defined(__x86_64__)
    __asm__ __volatile__ ("mov %%rsp, %0" : "=r"(sp));
#elif defined(__aarch64__)
    __asm__ __volatile__ ("mov %0, sp" : "=r"(sp));
#else
    sp = __builtin_frame_address(0);
#endif
    return sp;
}

/**
 * @brief Get the current CPU core's cycle count
 *
 * @return Always 0 (no hardware cycle counter on host)
 */
FORCE_INLINE_ATTR esp_cpu_cycle_count_t esp_cpu_get_cycle_count(void)
{
    return 0;
}

/**
 * @brief Set the current CPU core's cycle count (no-op on host)
 */
FORCE_INLINE_ATTR void esp_cpu_set_cycle_count(esp_cpu_cycle_count_t cycle_count)
{
    (void)cycle_count;
}

/**
 * @brief Convert a program counter (PC) value to address
 */
FORCE_INLINE_ATTR __attribute__((pure)) void *esp_cpu_pc_to_addr(uint32_t pc)
{
    return (void *)(uintptr_t)pc;
}

/**
 * @brief Set the current CPU core's thread pointer (no-op on host)
 */
FORCE_INLINE_ATTR void esp_cpu_set_threadptr(void *threadptr)
{
    (void)threadptr;
}

/**
 * @brief Get the current CPU core's thread pointer
 *
 * @return NULL on host
 */
FORCE_INLINE_ATTR void *esp_cpu_get_threadptr(void)
{
    return NULL;
}

/* ------------------------------------------------- CPU Interrupts ---------------------------------------------------- */

/**
 * @brief Get a CPU interrupt's descriptor
 *
 * Each CPU interrupt has a descriptor describing the interrupt's capabilities
 * and restrictions. This function gets the descriptor of a particular interrupt
 * on a particular CPU.
 *
 * @param[in] core_id The core's ID
 * @param[in] intr_num Interrupt number
 * @param[out] intr_desc_ret The interrupt's descriptor
 */
void esp_cpu_intr_get_desc(int core_id, int intr_num, esp_cpu_intr_desc_t *intr_desc_ret);

/**
 * @brief Set the base address of the current CPU's Interrupt Vector Table (IVT)
 *
 * @param ivt_addr Interrupt Vector Table's base address
 */

FORCE_INLINE_ATTR void esp_cpu_intr_set_ivt_addr(const void *ivt_addr)
{
    (void)ivt_addr;
}

/**
 * @brief Check if a particular interrupt already has a handler function
 *
 * @note This function simply checks if the IVT of the current CPU already has
 *       a handler assigned.
 * @param intr_num Interrupt number (from 0 to 31)
 * @return True if the interrupt has a handler function, false otherwise.
 */
FORCE_INLINE_ATTR bool esp_cpu_intr_has_handler(int intr_num)
{
    (void)intr_num;
    return false;
}

/**
 * @brief Set the handler function of a particular interrupt
 *
 * Assign a handler function (i.e., ISR) to a particular interrupt on the
 * current CPU.
 *
 * @note This function simply sets the handler function (in the IVT) and does
 *       not actually enable the interrupt.
 * @param intr_num Interrupt number (from 0 to 31)
 * @param handler Handler function
 * @param handler_arg Argument passed to the handler function
 */
FORCE_INLINE_ATTR void esp_cpu_intr_set_handler(int intr_num, esp_cpu_intr_handler_t handler, void *handler_arg)
{
    (void)intr_num;
    (void)handler;
    (void)handler_arg;
}

/**
 * @brief Get a handler function's argument of
 *
 * Get the argument of a previously assigned handler function on the current CPU.
 *
 * @param intr_num Interrupt number (from 0 to 31)
 * @return The the argument passed to the handler function
 */
FORCE_INLINE_ATTR void *esp_cpu_intr_get_handler_arg(int intr_num)
{
    (void)intr_num;
    return NULL;
}

/**
 * @brief Enable particular interrupts on the current CPU
 *
 * @param intr_mask Bit mask of the interrupts to enable
 */
FORCE_INLINE_ATTR void esp_cpu_intr_enable(uint32_t intr_mask)
{
    (void)intr_mask;
}

/**
 * @brief Disable particular interrupts on the current CPU
 *
 * @param intr_mask Bit mask of the interrupts to disable
 */
FORCE_INLINE_ATTR void esp_cpu_intr_disable(uint32_t intr_mask)
{
    (void)intr_mask;
}

/**
 * @brief Get the enabled interrupts on the current CPU
 *
 * @return Bit mask of the enabled interrupts
 */
FORCE_INLINE_ATTR uint32_t esp_cpu_intr_get_enabled_mask(void)
{
    return 0;
}

/**
 * @brief Acknowledge an edge interrupt
 *
 * @param intr_num Interrupt number (from 0 to 31)
 */
FORCE_INLINE_ATTR void esp_cpu_intr_edge_ack(int intr_num)
{
    (void)intr_num;
}

/* -------------------------------------------------- Memory Ports ----------------------------------------------------- */

/**
 * @brief Configure the CPU to disable access to invalid memory regions
 */
void esp_cpu_configure_region_protection(void);

/* ---------------------------------------------------- Debugging ------------------------------------------------------ */

/**
 * @brief Set and enable a hardware watchpoint on the current CPU
 *
 * @param wp_num Hardware watchpoint number
 * @param wp_addr Watchpoint's base address
 * @param size Size of the region to watch
 * @param trigger Trigger type
 * @return ESP_ERR_NOT_SUPPORTED on host
 */
esp_err_t esp_cpu_set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger);

/**
 * @brief Clear a hardware watchpoint on the current CPU
 *
 * @param wp_num Hardware watchpoint number
 * @return ESP_ERR_NOT_SUPPORTED on host
 */
esp_err_t esp_cpu_clear_watchpoint(int wp_num);

/**
 * @brief Check if the current CPU has a debugger attached
 *
 * @return True if debugger is attached, false otherwise
 */

FORCE_INLINE_ATTR bool esp_cpu_dbgr_is_attached(void)
{
    return false;
}

/**
 * @brief Trigger a call to the current CPU's attached debugger
 */
FORCE_INLINE_ATTR void esp_cpu_dbgr_break(void)
{
    __builtin_trap();
}

/**
 * @brief Given the return address, calculate the address of the preceding call instruction
 * This is typically used to answer the question "where was the function called from?"
 * @param return_address  The value of the return address register.
 *                        Typically set to the value of __builtin_return_address(0).
 * @return Address of the call instruction preceding the return address.
 */
FORCE_INLINE_ATTR intptr_t esp_cpu_get_call_addr(intptr_t return_address)
{
#if defined(__x86_64__)
    return return_address - 5;
#else
    return return_address - 4;
#endif
}

/* ------------------------------------------------------ Misc --------------------------------------------------------- */

/**
 * @brief Atomic compare-and-set operation
 *
 * @param addr Address of atomic variable
 * @param compare_value Value to compare the atomic variable to
 * @param new_value New value to set the atomic variable to
 * @return Whether the atomic variable was set or not
 */
bool esp_cpu_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value);

#ifdef __cplusplus
}
#endif
