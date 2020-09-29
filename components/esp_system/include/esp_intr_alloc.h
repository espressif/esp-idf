// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef __ESP_INTR_ALLOC_H__
#define __ESP_INTR_ALLOC_H__

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Intr_Alloc
  * @{
  */


/** @brief Interrupt allocation flags
 *
 * These flags can be used to specify which interrupt qualities the
 * code calling esp_intr_alloc* needs.
 *
 */

//Keep the LEVELx values as they are here; they match up with (1<<level)
#define ESP_INTR_FLAG_LEVEL1		(1<<1)	///< Accept a Level 1 interrupt vector (lowest priority)
#define ESP_INTR_FLAG_LEVEL2		(1<<2)	///< Accept a Level 2 interrupt vector
#define ESP_INTR_FLAG_LEVEL3		(1<<3)	///< Accept a Level 3 interrupt vector
#define ESP_INTR_FLAG_LEVEL4		(1<<4)	///< Accept a Level 4 interrupt vector
#define ESP_INTR_FLAG_LEVEL5		(1<<5)	///< Accept a Level 5 interrupt vector
#define ESP_INTR_FLAG_LEVEL6		(1<<6)	///< Accept a Level 6 interrupt vector
#define ESP_INTR_FLAG_NMI			(1<<7)	///< Accept a Level 7 interrupt vector (highest priority)
#define ESP_INTR_FLAG_SHARED		(1<<8)	///< Interrupt can be shared between ISRs
#define ESP_INTR_FLAG_EDGE			(1<<9)	///< Edge-triggered interrupt
#define ESP_INTR_FLAG_IRAM			(1<<10)	///< ISR can be called if cache is disabled
#define ESP_INTR_FLAG_INTRDISABLED	(1<<11)	///< Return with this interrupt disabled

#define ESP_INTR_FLAG_LOWMED	(ESP_INTR_FLAG_LEVEL1|ESP_INTR_FLAG_LEVEL2|ESP_INTR_FLAG_LEVEL3) ///< Low and medium prio interrupts. These can be handled in C.
#define ESP_INTR_FLAG_HIGH		(ESP_INTR_FLAG_LEVEL4|ESP_INTR_FLAG_LEVEL5|ESP_INTR_FLAG_LEVEL6|ESP_INTR_FLAG_NMI) ///< High level interrupts. Need to be handled in assembly.

#define ESP_INTR_FLAG_LEVELMASK	(ESP_INTR_FLAG_LEVEL1|ESP_INTR_FLAG_LEVEL2|ESP_INTR_FLAG_LEVEL3| \
								 ESP_INTR_FLAG_LEVEL4|ESP_INTR_FLAG_LEVEL5|ESP_INTR_FLAG_LEVEL6| \
								 ESP_INTR_FLAG_NMI) ///< Mask for all level flags


/** @addtogroup Intr_Alloc_Pseudo_Src
  * @{
  */

/**
 * The esp_intr_alloc* functions can allocate an int for all ETS_*_INTR_SOURCE interrupt sources that
 * are routed through the interrupt mux. Apart from these sources, each core also has some internal
 * sources that do not pass through the interrupt mux. To allocate an interrupt for these sources,
 * pass these pseudo-sources to the functions.
 */
#define ETS_INTERNAL_TIMER0_INTR_SOURCE		-1 ///< Platform timer 0 interrupt source
#define ETS_INTERNAL_TIMER1_INTR_SOURCE		-2 ///< Platform timer 1 interrupt source
#define ETS_INTERNAL_TIMER2_INTR_SOURCE		-3 ///< Platform timer 2 interrupt source
#define ETS_INTERNAL_SW0_INTR_SOURCE		-4 ///< Software int source 1
#define ETS_INTERNAL_SW1_INTR_SOURCE		-5 ///< Software int source 2
#define ETS_INTERNAL_PROFILING_INTR_SOURCE	-6 ///< Int source for profiling

/**@}*/

/** Provides SystemView with positive IRQ IDs, otherwise scheduler events are not shown properly
 */
#define ETS_INTERNAL_INTR_SOURCE_OFF		(-ETS_INTERNAL_PROFILING_INTR_SOURCE)

/** Enable interrupt by interrupt number */
#define ESP_INTR_ENABLE(inum)  esp_intr_enable_source(inum)

/** Disable interrupt by interrupt number */
#define ESP_INTR_DISABLE(inum) esp_intr_disable_source(inum)

/** Function prototype for interrupt handler function */
typedef void (*intr_handler_t)(void *arg);

/** Interrupt handler associated data structure */
typedef struct intr_handle_data_t intr_handle_data_t;

/** Handle to an interrupt handler */
typedef intr_handle_data_t* intr_handle_t ;

/**
 * @brief Mark an interrupt as a shared interrupt
 *
 * This will mark a certain interrupt on the specified CPU as
 * an interrupt that can be used to hook shared interrupt handlers
 * to.
 *
 * @param intno The number of the interrupt (0-31)
 * @param cpu CPU on which the interrupt should be marked as shared (0 or 1)
 * @param is_in_iram Shared interrupt is for handlers that reside in IRAM and
 *                   the int can be left enabled while the flash cache is disabled.
 *
 * @return ESP_ERR_INVALID_ARG if cpu or intno is invalid
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_mark_shared(int intno, int cpu, bool is_in_iram);

/**
 * @brief Reserve an interrupt to be used outside of this framework
 *
 * This will mark a certain interrupt on the specified CPU as
 * reserved, not to be allocated for any reason.
 *
 * @param intno The number of the interrupt (0-31)
 * @param cpu CPU on which the interrupt should be marked as shared (0 or 1)
 *
 * @return ESP_ERR_INVALID_ARG if cpu or intno is invalid
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_reserve(int intno, int cpu);

/**
 * @brief Allocate an interrupt with the given parameters.
 *
 * This finds an interrupt that matches the restrictions as given in the flags
 * parameter, maps the given interrupt source to it and hooks up the given
 * interrupt handler (with optional argument) as well. If needed, it can return
 * a handle for the interrupt as well.
 *
 * The interrupt will always be allocated on the core that runs this function.
 *
 * If ESP_INTR_FLAG_IRAM flag is used, and handler address is not in IRAM or
 * RTC_FAST_MEM, then ESP_ERR_INVALID_ARG is returned.
 *
 * @param source The interrupt source. One of the ETS_*_INTR_SOURCE interrupt mux
 *               sources, as defined in soc/soc.h, or one of the internal
 *               ETS_INTERNAL_*_INTR_SOURCE sources as defined in this header.
 * @param flags An ORred mask of the ESP_INTR_FLAG_* defines. These restrict the
 *               choice of interrupts that this routine can choose from. If this value
 *               is 0, it will default to allocating a non-shared interrupt of level
 *               1, 2 or 3. If this is ESP_INTR_FLAG_SHARED, it will allocate a shared
 *               interrupt of level 1. Setting ESP_INTR_FLAG_INTRDISABLED will return
 *               from this function with the interrupt disabled.
 * @param handler The interrupt handler. Must be NULL when an interrupt of level >3
 *               is requested, because these types of interrupts aren't C-callable.
 * @param arg    Optional argument for passed to the interrupt handler
 * @param ret_handle Pointer to an intr_handle_t to store a handle that can later be
 *               used to request details or free the interrupt. Can be NULL if no handle
 *               is required.
 *
 * @return ESP_ERR_INVALID_ARG if the combination of arguments is invalid.
 *         ESP_ERR_NOT_FOUND No free interrupt found with the specified flags
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_alloc(int source, int flags, intr_handler_t handler, void *arg, intr_handle_t *ret_handle);


/**
 * @brief Allocate an interrupt with the given parameters.
 *
 *
 * This essentially does the same as esp_intr_alloc, but allows specifying a register and mask
 * combo. For shared interrupts, the handler is only called if a read from the specified
 * register, ANDed with the mask, returns non-zero. By passing an interrupt status register
 * address and a fitting mask, this can be used to accelerate interrupt handling in the case
 * a shared interrupt is triggered; by checking the interrupt statuses first, the code can
 * decide which ISRs can be skipped
 *
 * @param source The interrupt source. One of the ETS_*_INTR_SOURCE interrupt mux
 *               sources, as defined in soc/soc.h, or one of the internal
 *               ETS_INTERNAL_*_INTR_SOURCE sources as defined in this header.
 * @param flags An ORred mask of the ESP_INTR_FLAG_* defines. These restrict the
 *               choice of interrupts that this routine can choose from. If this value
 *               is 0, it will default to allocating a non-shared interrupt of level
 *               1, 2 or 3. If this is ESP_INTR_FLAG_SHARED, it will allocate a shared
 *               interrupt of level 1. Setting ESP_INTR_FLAG_INTRDISABLED will return
 *               from this function with the interrupt disabled.
 * @param intrstatusreg The address of an interrupt status register
 * @param intrstatusmask A mask. If a read of address intrstatusreg has any of the bits
 *               that are 1 in the mask set, the ISR will be called. If not, it will be
 *               skipped.
 * @param handler The interrupt handler. Must be NULL when an interrupt of level >3
 *               is requested, because these types of interrupts aren't C-callable.
 * @param arg    Optional argument for passed to the interrupt handler
 * @param ret_handle Pointer to an intr_handle_t to store a handle that can later be
 *               used to request details or free the interrupt. Can be NULL if no handle
 *               is required.
 *
 * @return ESP_ERR_INVALID_ARG if the combination of arguments is invalid.
 *         ESP_ERR_NOT_FOUND No free interrupt found with the specified flags
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_alloc_intrstatus(int source, int flags, uint32_t intrstatusreg, uint32_t intrstatusmask, intr_handler_t handler, void *arg, intr_handle_t *ret_handle);


/**
 * @brief Disable and free an interrupt.
 *
 * Use an interrupt handle to disable the interrupt and release the resources associated with it.
 * If the current core is not the core that registered this interrupt, this routine will be assigned to
 * the core that allocated this interrupt, blocking and waiting until the resource is successfully released.
 *
 * @note
 * When the handler shares its source with other handlers, the interrupt status
 * bits it's responsible for should be managed properly before freeing it. see
 * ``esp_intr_disable`` for more details. Please do not call this function in ``esp_ipc_call_blocking``.
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 *
 * @return ESP_ERR_INVALID_ARG the handle is NULL
 *         ESP_FAIL failed to release this handle
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_free(intr_handle_t handle);


/**
 * @brief Get CPU number an interrupt is tied to
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 *
 * @return The core number where the interrupt is allocated
 */
int esp_intr_get_cpu(intr_handle_t handle);

/**
 * @brief Get the allocated interrupt for a certain handle
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 *
 * @return The interrupt number
 */
int esp_intr_get_intno(intr_handle_t handle);

/**
 * @brief Disable the interrupt associated with the handle
 *
 * @note
 * 1. For local interrupts (ESP_INTERNAL_* sources), this function has to be called on the
 * CPU the interrupt is allocated on. Other interrupts have no such restriction.
 * 2. When several handlers sharing a same interrupt source, interrupt status bits, which are
 * handled in the handler to be disabled, should be masked before the disabling, or handled
 * in other enabled interrupts properly. Miss of interrupt status handling will cause infinite
 * interrupt calls and finally system crash.
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 *
 * @return ESP_ERR_INVALID_ARG if the combination of arguments is invalid.
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_disable(intr_handle_t handle);

/**
 * @brief Enable the interrupt associated with the handle
 *
 * @note For local interrupts (ESP_INTERNAL_* sources), this function has to be called on the
 *       CPU the interrupt is allocated on. Other interrupts have no such restriction.
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 *
 * @return ESP_ERR_INVALID_ARG if the combination of arguments is invalid.
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_enable(intr_handle_t handle);

/**
 * @brief Set the "in IRAM" status of the handler.
 *
 * @note Does not work on shared interrupts.
 *
 * @param handle The handle, as obtained by esp_intr_alloc or esp_intr_alloc_intrstatus
 * @param is_in_iram Whether the handler associated with this handle resides in IRAM.
 *                   Handlers residing in IRAM can be called when cache is disabled.
 *
 * @return ESP_ERR_INVALID_ARG if the combination of arguments is invalid.
 *         ESP_OK otherwise
 */
esp_err_t esp_intr_set_in_iram(intr_handle_t handle, bool is_in_iram);

/**
 * @brief Disable interrupts that aren't specifically marked as running from IRAM
 */
void esp_intr_noniram_disable(void);

/**
 * @brief Re-enable interrupts disabled by esp_intr_noniram_disable
 */
void esp_intr_noniram_enable(void);

/**
 * @brief enable the interrupt source based on its number
 * @param inum interrupt number from 0 to 31
 */ 
void esp_intr_enable_source(int inum);

/**
 * @brief disable the interrupt source based on its number
 * @param inum interrupt number from 0 to 31
 */ 
void esp_intr_disable_source(int inum);

/**@}*/


#ifdef __cplusplus
}
#endif

#endif
