/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/*
Note: This is a compatibility header. Call the interfaces in esp_cpu.h instead
*/

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INTDESC_NORMAL = 0,
    INTDESC_RESVD,
    INTDESC_SPECIAL,
} int_desc_flag_t;

typedef enum {
    INTTP_LEVEL = ESP_CPU_INTR_TYPE_LEVEL,
    INTTP_EDGE = ESP_CPU_INTR_TYPE_EDGE,
    INTTP_NA = ESP_CPU_INTR_TYPE_NA,
} int_type_t;

typedef struct {
    int level;
    int_type_t type;
    int_desc_flag_t cpuflags[SOC_CPU_CORES_NUM];
} int_desc_t;

typedef void (*interrupt_handler_t)(void *arg);

// ---------------- Interrupt Descriptors ------------------

/**
 * @brief Gets the interrupt type given an interrupt number.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @return interrupt type
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) int_type_t interrupt_controller_hal_desc_type(int interrupt_number)
{
    esp_cpu_intr_desc_t intr_desc;
    esp_cpu_intr_get_desc(esp_cpu_get_core_id(), interrupt_number, &intr_desc);
    return (int_type_t)intr_desc.type;
}

/**
 * @brief Gets the interrupt level given an interrupt number.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @return interrupt level bitmask
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) int interrupt_controller_hal_desc_level(int interrupt_number)
{
    esp_cpu_intr_desc_t intr_desc;
    esp_cpu_intr_get_desc(esp_cpu_get_core_id(), interrupt_number, &intr_desc);
    return intr_desc.priority;
}

/**
 * @brief Gets the cpu flags given the interrupt number and target cpu.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param cpu_number CPU number between 0 and SOC_CPU_CORES_NUM - 1
 * @return flags for that interrupt number
 */
FORCE_INLINE_ATTR __attribute__((deprecated))
int_desc_flag_t interrupt_controller_hal_desc_flags(int interrupt_number, int cpu_number)
{
    esp_cpu_intr_desc_t intr_desc;
    esp_cpu_intr_get_desc(cpu_number, interrupt_number, &intr_desc);
    int_desc_flag_t ret;
    if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_SPECIAL) {
        ret = INTDESC_SPECIAL;
    } else if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_RESVD) {
        ret = INTDESC_RESVD;
    } else {
        ret = INTDESC_NORMAL;
    }
    return ret;
}

/**
 * @brief Gets the interrupt type given an interrupt number.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @return interrupt type
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) int_type_t interrupt_controller_hal_get_type(int interrupt_number)
{
    return interrupt_controller_hal_desc_type(interrupt_number);
}

/**
 * @brief Gets the interrupt level given an interrupt number.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @return interrupt level bitmask
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) int interrupt_controller_hal_get_level(int interrupt_number)
{
    return interrupt_controller_hal_desc_level(interrupt_number);
}

/**
 * @brief Gets the cpu flags given the interrupt number and target cpu.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param cpu_number CPU number between 0 and SOC_CPU_CORES_NUM - 1
 * @return flags for that interrupt number
 */
FORCE_INLINE_ATTR __attribute__((deprecated))
uint32_t interrupt_controller_hal_get_cpu_desc_flags(int interrupt_number, int cpu_number)
{
    return (uint32_t)interrupt_controller_hal_desc_flags(interrupt_number, cpu_number);
}

// --------------- Interrupt Configuration -----------------

#if SOC_CPU_HAS_FLEXIBLE_INTC
/**
 * @brief Set the type of an interrupt in the controller.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param type interrupt type as edge or level triggered
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void interrupt_controller_hal_set_int_type(int intr, int_type_t type)
{
    esp_cpu_intr_set_type(intr, (esp_cpu_intr_type_t)type);
}

/**
 * @brief Sets the interrupt level int the interrupt controller.
 *
 * @param interrupt_number Interrupt number 0 to 31
 * @param level priority between 1 (lowest) to 7 (highest)
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void interrupt_controller_hal_set_int_level(int intr, int level)
{
    esp_cpu_intr_set_priority(intr, level);
}
#endif // SOC_CPU_HAS_FLEXIBLE_INTC

/**
 * @brief checks if given interrupt number has a valid handler
 *
 * @param intr interrupt number ranged from 0 to 31
 * @param cpu this argument is ignored
 * @return true for valid handler, false otherwise
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) bool interrupt_controller_hal_has_handler(int intr, int cpu)
{
    (void) cpu;
    return esp_cpu_intr_has_handler(intr);
}

/**
 * @brief sets interrupt handler and optional argument of a given interrupt number
 *
 * @param intr interrupt number ranged from 0 to 31
 * @param handler handler invoked when an interrupt occurs
 * @param arg optional argument to pass to the handler
 */
FORCE_INLINE_ATTR __attribute__((deprecated))
void interrupt_controller_hal_set_int_handler(uint8_t intr, interrupt_handler_t handler, void *arg)
{
    esp_cpu_intr_set_handler(intr, (esp_cpu_intr_handler_t)handler, arg);
}

/**
 * @brief Gets argument passed to handler of a given interrupt number
 *
 * @param intr interrupt number ranged from 0 to 31
 *
 * @return argument used by handler of passed interrupt number
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void *interrupt_controller_hal_get_int_handler_arg(uint8_t intr)
{
    return esp_cpu_intr_get_handler_arg(intr);
}

// ------------------ Interrupt Control --------------------

/**
 * @brief enable interrupts specified by the mask
 *
 * @param mask bitmask of interrupts that needs to be enabled
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void interrupt_controller_hal_enable_interrupts(uint32_t mask)
{
    esp_cpu_intr_enable(mask);
}

/**
 * @brief disable interrupts specified by the mask
 *
 * @param mask bitmask of interrupts that needs to be disabled
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void interrupt_controller_hal_disable_interrupts(uint32_t mask)
{
    esp_cpu_intr_disable(mask);
}

/**
 * @brief Read the current interrupt mask.
 *
 * @return The bitmask of current interrupts
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) uint32_t interrupt_controller_hal_read_interrupt_mask(void)
{
    return esp_cpu_intr_get_enabled_mask();
}

/**
 * @brief Acknowledge an edge-trigger interrupt by clearing its pending flag
 *
 * @param intr interrupt number ranged from 0 to 31
 */
FORCE_INLINE_ATTR __attribute__((deprecated)) void interrupt_controller_hal_edge_int_acknowledge(int intr)
{
    esp_cpu_intr_edge_ack(intr);
}

#ifdef __cplusplus
}
#endif
