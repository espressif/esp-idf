/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include "soc/soc_caps.h"
#ifdef __XTENSA__
#include "xtensa_api.h"
#include "xt_utils.h"
#elif __riscv
#include "riscv/rv_utils.h"
#endif
#include "esp_intr_alloc.h"
#include "esp_err.h"
#include "esp_attr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CPU cycle count type
 *
 * This data type represents the CPU's clock cycle count
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
 *
 * Each particular CPU interrupt has an associated descriptor describing that
 * particular interrupt's characteristics. Call esp_cpu_intr_get_desc() to get
 * the descriptors of a particular interrupt.
 */
typedef struct {
    int priority;               /**< Priority of the interrupt if it has a fixed priority, (-1) if the priority is configurable. */
    esp_cpu_intr_type_t type;   /**< Whether the interrupt is an edge or level type interrupt, ESP_CPU_INTR_TYPE_NA if the type is configurable. */
    uint32_t flags;             /**< Flags indicating extra details. */
} esp_cpu_intr_desc_t;

/**
 * @brief Interrupt descriptor flags of esp_cpu_intr_desc_t
 */
#define ESP_CPU_INTR_DESC_FLAG_SPECIAL      0x01    /**< The interrupt is a special interrupt (e.g., a CPU timer interrupt) */
#define ESP_CPU_INTR_DESC_FLAG_RESVD        0x02    /**< The interrupt is reserved for internal use */

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

/* --------------------------------------------------- CPU Control -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

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

/* -------------------------------------------------- CPU Registers ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Get the current core's ID
 *
 * This function will return the ID of the current CPU (i.e., the CPU that calls
 * this function).
 *
 * @return The current core's ID [0..SOC_CPU_CORES_NUM - 1]
 */
FORCE_INLINE_ATTR __attribute__((pure)) int esp_cpu_get_core_id(void)
{
    //Note: Made "pure" to optimize for single core target
#ifdef __XTENSA__
    return (int)xt_utils_get_core_id();
#else
    return (int)rv_utils_get_core_id();
#endif
}

/**
 * @brief Read the current stack pointer address
 *
 * @return Stack pointer address
 */
FORCE_INLINE_ATTR void *esp_cpu_get_sp(void)
{
#ifdef __XTENSA__
    return xt_utils_get_sp();
#else
    return rv_utils_get_sp();
#endif
}

/**
 * @brief Get the current CPU core's cycle count
 *
 * Each CPU core maintains an internal counter (i.e., cycle count) that increments
 * every CPU clock cycle.
 *
 * @return Current CPU's cycle count, 0 if not supported.
 */
FORCE_INLINE_ATTR esp_cpu_cycle_count_t esp_cpu_get_cycle_count(void)
{
#ifdef __XTENSA__
    return (esp_cpu_cycle_count_t)xt_utils_get_cycle_count();
#else
    return (esp_cpu_cycle_count_t)rv_utils_get_cycle_count();
#endif
}

/**
 * @brief Set the current CPU core's cycle count
 *
 * Set the given value into the internal counter that increments every
 * CPU clock cycle.
 *
 * @param cycle_count CPU cycle count
 */
FORCE_INLINE_ATTR void esp_cpu_set_cycle_count(esp_cpu_cycle_count_t cycle_count)
{
#ifdef __XTENSA__
    xt_utils_set_cycle_count((uint32_t)cycle_count);
#else
    rv_utils_set_cycle_count((uint32_t)cycle_count);
#endif
}

/**
 * @brief Convert a program counter (PC) value to address
 *
 * If the architecture does not store the true virtual address in the CPU's PC
 * or return addresses, this function will convert the PC value to a virtual
 * address. Otherwise, the PC is just returned
 *
 * @param pc PC value
 * @return Virtual address
 */
FORCE_INLINE_ATTR __attribute__((pure)) void *esp_cpu_pc_to_addr(uint32_t pc)
{
#ifdef __XTENSA__
    // Xtensa stores window rotation in PC[31:30]
    return (void *)((pc & 0x3fffffffU) | 0x40000000U);
#else
    return (void *)pc;
#endif
}

/* ------------------------------------------------- CPU Interrupts ----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// ---------------- Interrupt Descriptors ------------------

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

// --------------- Interrupt Configuration -----------------

/**
 * @brief Set the base address of the current CPU's Interrupt Vector Table (IVT)
 *
 * @param ivt_addr Interrupt Vector Table's base address
 */
FORCE_INLINE_ATTR void esp_cpu_intr_set_ivt_addr(const void *ivt_addr)
{
#ifdef __XTENSA__
    xt_utils_set_vecbase((uint32_t)ivt_addr);
#else
    rv_utils_set_mtvec((uint32_t)ivt_addr);
#endif
}

#if SOC_INT_CLIC_SUPPORTED
/**
 * @brief Set the base address of the current CPU's Interrupt Vector Table (MTVT)
 *
 * @param mtvt_addr Interrupt Vector Table's base address
 *
 * @note The MTVT table is only applicable when CLIC is supported
 */
FORCE_INLINE_ATTR void esp_cpu_intr_set_mtvt_addr(const void *mtvt_addr)
{
    rv_utils_set_mtvt((uint32_t)mtvt_addr);
}
#endif  //#if SOC_INT_CLIC_SUPPORTED

#if SOC_CPU_SUPPORT_WFE
/**
 * @brief Disable the WFE (wait for event) feature for CPU.
 */
FORCE_INLINE_ATTR void rv_utils_disable_wfe_mode(void)
{
    rv_utils_wfe_mode_enable(false);
}
#endif

#if SOC_CPU_HAS_FLEXIBLE_INTC
/**
 * @brief Set the interrupt type of a particular interrupt
 *
 * Set the interrupt type (Level or Edge) of a particular interrupt on the
 * current CPU.
 *
 * @param intr_num Interrupt number (from 0 to 31)
 * @param intr_type The interrupt's type
 */
FORCE_INLINE_ATTR void esp_cpu_intr_set_type(int intr_num, esp_cpu_intr_type_t intr_type)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    enum intr_type type = (intr_type == ESP_CPU_INTR_TYPE_LEVEL) ? INTR_TYPE_LEVEL : INTR_TYPE_EDGE;
    esprv_int_set_type(intr_num, type);
}

/**
 * @brief Get the current configured type of a particular interrupt
 *
 * Get the currently configured type (i.e., level or edge) of a particular
 * interrupt on the current CPU.
 *
 * @param intr_num Interrupt number (from 0 to 31)
 * @return Interrupt type
 */
FORCE_INLINE_ATTR esp_cpu_intr_type_t esp_cpu_intr_get_type(int intr_num)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    enum intr_type type = esprv_int_get_type(intr_num);
    return (type == INTR_TYPE_LEVEL) ? ESP_CPU_INTR_TYPE_LEVEL : ESP_CPU_INTR_TYPE_EDGE;
}

/**
 * @brief Set the priority of a particular interrupt
 *
 * Set the priority of a particular interrupt on the current CPU.
 *
 * @param intr_num Interrupt number (from 0 to 31)
 * @param intr_priority The interrupt's priority
 */
FORCE_INLINE_ATTR void esp_cpu_intr_set_priority(int intr_num, int intr_priority)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    esprv_int_set_priority(intr_num, intr_priority);
}

/**
 * @brief Get the current configured priority of a particular interrupt
 *
 * Get the currently configured priority of a particular interrupt on the
 * current CPU.
 *
 * @param intr_num Interrupt number (from 0 to 31)
 * @return Interrupt's priority
 */
FORCE_INLINE_ATTR int esp_cpu_intr_get_priority(int intr_num)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    return esprv_int_get_priority(intr_num);
}
#endif // SOC_CPU_HAS_FLEXIBLE_INTC

/**
 * @brief Check if a particular interrupt already has a handler function
 *
 * Check if a particular interrupt on the current CPU already has a handler
 * function assigned.
 *
 * @note This function simply checks if the IVT of the current CPU already has
 *       a handler assigned.
 * @param intr_num Interrupt number (from 0 to 31)
 * @return True if the interrupt has a handler function, false otherwise.
 */
FORCE_INLINE_ATTR bool esp_cpu_intr_has_handler(int intr_num)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    bool has_handler;
#ifdef __XTENSA__
    has_handler = xt_int_has_handler(intr_num, esp_cpu_get_core_id());
#else
    has_handler = intr_handler_get(intr_num) != NULL;
#endif
    return has_handler;
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
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
#ifdef __XTENSA__
    xt_set_interrupt_handler(intr_num, (xt_handler)handler, handler_arg);
#else
    intr_handler_set(intr_num, (intr_handler_t)handler, handler_arg);
#endif
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
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
    void *handler_arg;
#ifdef __XTENSA__
    handler_arg = xt_get_interrupt_handler_arg(intr_num);
#else
    handler_arg = intr_handler_get_arg(intr_num);
#endif
    return handler_arg;
}

// ------------------ Interrupt Control --------------------

/**
 * @brief Enable particular interrupts on the current CPU
 *
 * @param intr_mask Bit mask of the interrupts to enable
 */
FORCE_INLINE_ATTR void esp_cpu_intr_enable(uint32_t intr_mask)
{
#ifdef __XTENSA__
    xt_ints_on(intr_mask);
#else
    rv_utils_intr_enable(intr_mask);
#endif
}

/**
 * @brief Disable particular interrupts on the current CPU
 *
 * @param intr_mask Bit mask of the interrupts to disable
 */
FORCE_INLINE_ATTR void esp_cpu_intr_disable(uint32_t intr_mask)
{
#ifdef __XTENSA__
    xt_ints_off(intr_mask);
#else
    rv_utils_intr_disable(intr_mask);
#endif
}

/**
 * @brief Get the enabled interrupts on the current CPU
 *
 * @return Bit mask of the enabled interrupts
 */
FORCE_INLINE_ATTR uint32_t esp_cpu_intr_get_enabled_mask(void)
{
#ifdef __XTENSA__
    return xt_utils_intr_get_enabled_mask();
#else
    return rv_utils_intr_get_enabled_mask();
#endif
}

/**
 * @brief Acknowledge an edge interrupt
 *
 * @param intr_num Interrupt number (from 0 to 31)
 */
FORCE_INLINE_ATTR void esp_cpu_intr_edge_ack(int intr_num)
{
    assert(intr_num >= 0 && intr_num < SOC_CPU_INTR_NUM);
#ifdef __XTENSA__
    xthal_set_intclear((unsigned) (1 << intr_num));
#else
    rv_utils_intr_edge_ack((unsigned) intr_num);
#endif
}

/* -------------------------------------------------- Memory Ports -----------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Configure the CPU to disable access to invalid memory regions
 */
void esp_cpu_configure_region_protection(void);

/* ---------------------------------------------------- Debugging ------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

// --------------- Breakpoints/Watchpoints -----------------

#if SOC_CPU_BREAKPOINTS_NUM > 0

/**
 * @brief Set and enable a hardware breakpoint on the current CPU
 *
 * @note This function is meant to be called by the panic handler to set a
 * breakpoint for an attached debugger during a panic.
 * @note Overwrites previously set breakpoint with same breakpoint number.
 * @param bp_num Hardware breakpoint number [0..SOC_CPU_BREAKPOINTS_NUM - 1]
 * @param bp_addr Address to set a breakpoint on
 * @return ESP_OK if breakpoint is set. Failure otherwise
 */
esp_err_t esp_cpu_set_breakpoint(int bp_num, const void *bp_addr);

/**
 * @brief Clear a hardware breakpoint on the current CPU
 *
 * @note Clears a breakpoint regardless of whether it was previously set
 * @param bp_num Hardware breakpoint number [0..SOC_CPU_BREAKPOINTS_NUM - 1]
 * @return ESP_OK if breakpoint is cleared. Failure otherwise
 */
esp_err_t esp_cpu_clear_breakpoint(int bp_num);

#endif // SOC_CPU_BREAKPOINTS_NUM > 0

/**
 * @brief Set and enable a hardware watchpoint on the current CPU
 *
 * Set and enable a hardware watchpoint on the current CPU, specifying the
 * memory range and trigger operation. Watchpoints will break/panic the CPU when
 * the CPU accesses (according to the trigger type) on a certain memory range.
 *
 * @note Overwrites previously set watchpoint with same watchpoint number.
 *       On RISC-V chips, this API uses method0(Exact matching) and method1(NAPOT matching) according to the
 *       riscv-debug-spec-0.13 specification for address matching.
 *       If the watch region size is 1byte, it uses exact matching (method 0).
 *       If the watch region size is larger than 1byte, it uses NAPOT matching (method 1). This mode requires
 *       the watching region start address to be aligned to the watching region size.
 *
 * @param wp_num Hardware watchpoint number [0..SOC_CPU_WATCHPOINTS_NUM - 1]
 * @param wp_addr Watchpoint's base address, must be naturally aligned to the size of the region
 * @param size Size of the region to watch. Must be one of 2^n and in the range of [1 ... SOC_CPU_WATCHPOINT_MAX_REGION_SIZE]
 * @param trigger Trigger type
 * @return ESP_ERR_INVALID_ARG on invalid arg, ESP_OK otherwise
 */
esp_err_t esp_cpu_set_watchpoint(int wp_num, const void *wp_addr, size_t size, esp_cpu_watchpoint_trigger_t trigger);

/**
 * @brief Clear a hardware watchpoint on the current CPU
 *
 * @note Clears a watchpoint regardless of whether it was previously set
 * @param wp_num Hardware watchpoint number [0..SOC_CPU_WATCHPOINTS_NUM - 1]
 * @return ESP_OK if watchpoint was cleared. Failure otherwise.
 */
esp_err_t esp_cpu_clear_watchpoint(int wp_num);

// ---------------------- Debugger -------------------------

/**
 * @brief Check if the current CPU has a debugger attached
 *
 * @return True if debugger is attached, false otherwise
 */
FORCE_INLINE_ATTR bool esp_cpu_dbgr_is_attached(void)
{
#ifdef __XTENSA__
    return xt_utils_dbgr_is_attached();
#else
    return rv_utils_dbgr_is_attached();
#endif
}

/**
 * @brief Trigger a call to the current CPU's attached debugger
 */
FORCE_INLINE_ATTR void esp_cpu_dbgr_break(void)
{
#ifdef __XTENSA__
    xt_utils_dbgr_break();
#else
    rv_utils_dbgr_break();
#endif
}

// ---------------------- Instructions -------------------------

/**
 * @brief Given the return address, calculate the address of the preceding call instruction
 * This is typically used to answer the question "where was the function called from?"
 * @param return_address  The value of the return address register.
 *                        Typically set to the value of __builtin_return_address(0).
 * @return Address of the call instruction preceding the return address.
 */
FORCE_INLINE_ATTR intptr_t esp_cpu_get_call_addr(intptr_t return_address)
{
    /* Both Xtensa and RISC-V have 2-byte instructions, so to get this right we
     * should decode the preceding instruction as if it is 2-byte, check if it is a call,
     * else treat it as 3 or 4 byte one. However for the cases where this function is
     * used, being off by one instruction is usually okay, so this is kept simple for now.
     */
#ifdef __XTENSA__
    return return_address - 3;
#else
    return return_address - 4;
#endif
}

/* ------------------------------------------------------ Misc ---------------------------------------------------------
 *
 * ------------------------------------------------------------------------------------------------------------------ */

/**
 * @brief Atomic compare-and-set operation
 *
 * @param addr Address of atomic variable
 * @param compare_value Value to compare the atomic variable to
 * @param new_value New value to set the atomic variable to
 * @return Whether the atomic variable was set or not
 */
bool esp_cpu_compare_and_set(volatile uint32_t *addr, uint32_t compare_value, uint32_t new_value);

#if SOC_BRANCH_PREDICTOR_SUPPORTED
/**
 * @brief Enable branch prediction
 */
FORCE_INLINE_ATTR void esp_cpu_branch_prediction_enable(void)
{
    rv_utils_en_branch_predictor();
}

/**
 * @brief Disable branch prediction
 */
FORCE_INLINE_ATTR void esp_cpu_branch_prediction_disable(void)
{
    rv_utils_dis_branch_predictor();
}
#endif  //#if SOC_BRANCH_PREDICTOR_SUPPORTED

#ifdef __cplusplus
}
#endif
