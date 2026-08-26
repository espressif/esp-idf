/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2025 Espressif Systems (Shanghai) CO LTD
 */
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef StaticSemaphore_t sys_sem_t;
typedef SemaphoreHandle_t sys_mutex_t;
typedef TaskHandle_t sys_thread_t;

/**
 * @brief Mailbox structure using static allocation to prevent heap fragmentation
 *
 * This structure uses FreeRTOS static queue allocation instead of dynamic allocation.
 * The buffer[] field is a flexible array member that points to the queue storage
 * allocated immediately after this structure.
 */
typedef struct sys_mbox_s {
  StaticQueue_t os_mbox;  /**< FreeRTOS static queue structure */
  uint8_t buffer[];       /**< Flexible array member for queue storage */
}* sys_mbox_t;

/** This is returned by _fromisr() sys functions to tell the outermost function
 * that a higher priority task was woken and the scheduler needs to be invoked.
 */
#define ERR_NEED_SCHED 123

void sys_delay_ms(uint32_t ms);
#define sys_msleep(ms) sys_delay_ms(ms)

#define LWIP_COMPAT_MUTEX 0

#if !LWIP_COMPAT_MUTEX
#define sys_mutex_valid_val(mutex)   ((mutex) != NULL)
#define sys_mutex_valid(mutex)       (((mutex) != NULL) && sys_mutex_valid_val(*(mutex)))
#define sys_mutex_set_invalid(mutex) ((*(mutex)) = NULL)
#endif

#define sys_mbox_valid(mbox)       (*(mbox) != NULL)
#define sys_mbox_set_invalid(mbox) (*(mbox) = NULL)

/**
 * @brief Check if a static semaphore is valid (initialized)
 *
 * For static semaphores, we check if the structure is zero-initialized.
 * A zero-initialized StaticSemaphore_t indicates an uninitialized semaphore.
 */
#define sys_sem_valid(sema)       ((memcmp((sema), &(StaticSemaphore_t){0}, sizeof(StaticSemaphore_t)) == 0) ? pdFALSE : pdTRUE)

/**
 * @brief Mark a static semaphore as invalid (uninitialized)
 *
 * Sets the entire StaticSemaphore_t structure to zero, indicating it's uninitialized.
 */
#define sys_sem_set_invalid(sema) (memset((sema), 0, sizeof(StaticSemaphore_t)))

void sys_delay_ms(uint32_t ms);
sys_sem_t* sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);
sys_sem_t* sys_thread_sem_get(void);

typedef enum {
    LWIP_CORE_LOCK_QUERY_HOLDER,
    LWIP_CORE_LOCK_MARK_HOLDER,
    LWIP_CORE_LOCK_UNMARK_HOLDER,
    LWIP_CORE_MARK_TCPIP_TASK,
    LWIP_CORE_IS_TCPIP_INITIALIZED,
} sys_thread_core_lock_t;

bool
sys_thread_tcpip(sys_thread_core_lock_t type);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_ARCH_H__ */
