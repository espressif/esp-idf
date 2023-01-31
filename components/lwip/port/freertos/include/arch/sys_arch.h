/*
 * SPDX-FileCopyrightText: 2001-2003 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
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


typedef SemaphoreHandle_t sys_sem_t;
typedef SemaphoreHandle_t sys_mutex_t;
typedef TaskHandle_t sys_thread_t;

typedef struct sys_mbox_s {
  QueueHandle_t os_mbox;
  void *owner;
}* sys_mbox_t;

/** This is returned by _fromisr() sys functions to tell the outermost function
 * that a higher priority task was woken and the scheduler needs to be invoked.
 */
#define ERR_NEED_SCHED 123

void sys_delay_ms(uint32_t ms);
#define sys_msleep(ms) sys_delay_ms(ms)

#define LWIP_COMPAT_MUTEX 0

#if !LWIP_COMPAT_MUTEX
#define sys_mutex_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define sys_mutex_set_invalid( x ) ( ( *x ) = NULL )
#endif

#define sys_mbox_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )

/* Define the sys_mbox_set_invalid() to empty to support lock-free mbox in ESP LWIP.
 *
 * The basic idea about the lock-free mbox is that the mbox should always be valid unless
 * no socket APIs are using the socket and the socket is closed. ESP LWIP achieves this by
 * following two changes to official LWIP:
 * 1. Postpone the deallocation of mbox to netconn_free(), in other words, free the mbox when
 *    no one is using the socket.
 * 2. Define the sys_mbox_set_invalid() to empty if the mbox is not actually freed.

 * The second change is necessary. Consider a common scenario: the application task calls
 * recv() to receive packets from the socket, the sys_mbox_valid() returns true. Because there
 * is no lock for the mbox, the LWIP CORE can call sys_mbox_set_invalid() to set the mbox at
 * anytime and the thread-safe issue may happen.
 *
 * However, if the sys_mbox_set_invalid() is not called after sys_mbox_free(), e.g. in netconn_alloc(),
 * we need to initialize the mbox to invalid explicitly since sys_mbox_set_invalid() now is empty.
 */
#define sys_mbox_set_invalid( x )  *x = NULL

#define sys_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define sys_sem_set_invalid( x ) ( ( *x ) = NULL )

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
