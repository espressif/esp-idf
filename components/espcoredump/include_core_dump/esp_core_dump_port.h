/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ESP_CORE_DUMP_PORT_H_
#define ESP_CORE_DUMP_PORT_H_

/**
 * @file
 * @brief Core dump port interface.
 *
 * This file contains all the functions required by the core dump component to
 * get the information related to the board or the SoC itself. Currently, the
 * implementations of this interface, located in `src/port/[arch]`, support
 * both Xtensa and RISC-V architecture.
 */

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_app_format.h"
#include "esp_core_dump_types.h"
#include "esp_core_dump_port_impl.h"
#include "esp_core_dump.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get the architecture ID.
 *
 * @return Architecture ID, as described by ELF format.
 */
uint16_t esp_core_dump_get_arch_id(void);

/**
 * @brief Initialize the port module. This function is also in charge of
 * initializing the extra information, if any.
 *
 * @param info Pointer to the panic information. It contains the execution
 *             frame.
 * @param isr_context A flag indicating whether the crash happened within an ISR context.
 *                    Set to 1 if the crash occurred in an ISR, and 0 otherwise.
 */
void esp_core_dump_port_init(panic_info_t *info, bool isr_context);

/**
 * @brief Reset fake stacks allocator, if any.
 *
 * @note This function is called if we want to free all the previously
 * allocated "fake" stacks, used in broken tasks.
 */
void esp_core_dump_reset_fake_stacks(void);

/**
 * @brief Get ISR stack end address.
 *
 * @return End address of the ISR stack.
 */
uint32_t esp_core_dump_get_isr_stack_end(void);


/**
 * @brief Get the top of the ISR stack.
 *
 * @return Pointer to the top of the ISR stack.
 */
uint8_t* esp_core_dump_get_isr_stack_top(void);


/**
 * @brief Check the stack defined by address given.
 *
 * @param task Task to check the stack of.
 *
 * @return true is the stack is sane, false else.
 */
bool esp_core_dump_check_stack(core_dump_task_header_t *task);


/**
 * @brief Check if the memory segment is sane.
 *
 * @param addr Address of the memory segment to check.
 * @param sz Size of the memory segment to check.
 *
 * @return true if the memory segment is sane, false else.
 */
bool esp_core_dump_mem_seg_is_sane(uint32_t addr, uint32_t sz);


/**
 * @brief Get the stack of a task.
 *
 * @param task_snapshot Pointer to the task snapshot.
 * @param stk_vaddr Pointer which will be set to the stack's virtual address.
 *                  Must **not** be NULL.
 * @param stk_paddr Pointer which will be set to the stack's physical
 *                  address. Must **not** be NULL.
 *
 * @return Size, in bytes, of the stack.
 */
uint32_t esp_core_dump_get_stack(core_dump_task_header_t* task_snapshot,
                                 uint32_t* stk_vaddr, uint32_t* stk_paddr);


/**
 * @brief Check the task passed as a parameter.
 *
 * @note The goal of this function is to check whether the task passed is the
 *       task that crashed or not. If this is the case and if it didn't crash
 *       within an ISR, its stack pointer will be set to the panic frame,
 *       containing all the registers values when the error occured. This
 *       function also checks if the TCB address is sane or not.
 *
 * @param task Pointer to the frame exception generated when the panic occured.
 *
 * @return True if the TCB is sane, false else.
 */
bool esp_core_dump_check_task(core_dump_task_header_t *task);


/**
 * @brief Get a dump of the task's registers.
 *
 * @note In practice, this function is used to fill the ELF file with the
 *       PR_STATUS sections for all the existing tasks. This structure
 *       contains the CPU registers value when the exception occured.
 *
 * @param task     Task to dump the registers from.
 * @param reg_dump Pointer that will be filled with the registers dump.
 *                 Must **not** be NULL.
 *
 * @return Size, in bytes, of the returned registers duump.
 */
uint32_t esp_core_dump_get_task_regs_dump(core_dump_task_header_t *task,
                                          void **reg_dump);

/**
 * @brief Transmit the crashed task handle.
 *
 * @param handle Crashed task handle.
 *
 * @note This function is used to give information about the crashed task to
 * the port module. It can be ignored if not needed.
 */
void esp_core_dump_port_set_crashed_tcb(uint32_t handle);

/**
 * @brief Retrieve the extra information.
 *
 * @param info Pointer that will be filled with the extra information.
 *             Can be NULL, in that case, this function is used to get the
 *             extra information size.
 *
 * @return Size, in bytes, of the extra information.
 */
uint32_t esp_core_dump_get_extra_info(void **info);

#if CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF

/**
 * @brief Parse extra information into summary
 *
 * @param summary Pointer to core dump summary structure
 * @param ei_data Pointer to data of EXTRA_INFO note read from flash
 */
void esp_core_dump_summary_parse_extra_info(esp_core_dump_summary_t *summary, void *ei_data);

/**
 * @brief Parse exception registers into summary
 *
 * @param summary Pointer to core dump summary structure
 * @param stack_data Pointer to data of crashed task's stack read from flash
 */
void esp_core_dump_summary_parse_exc_regs(esp_core_dump_summary_t *summary, void *stack_data);

/**
 * @brief Parse backtrace into bt_info
 *
 * @param bt_info Pointer to store backtrace info
 * @param vaddr Pointer to crashed task's stack vaddr
 * @param paddr Pointe to crashed task's stack paddr
 * @param stack_size Stack size
 */
void esp_core_dump_summary_parse_backtrace_info(esp_core_dump_bt_info_t *bt_info, const void *vaddr,
                                                const void *paddr, uint32_t stack_size);
#endif /* CONFIG_ESP_COREDUMP_ENABLE_TO_FLASH && CONFIG_ESP_COREDUMP_DATA_FORMAT_ELF */

#ifdef __cplusplus
}
#endif

#endif
