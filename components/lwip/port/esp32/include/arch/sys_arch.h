/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
 
#ifndef __SYS_ARCH_H__
#define __SYS_ARCH_H__

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "arch/vfs_lwip.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef xSemaphoreHandle sys_sem_t;
typedef xSemaphoreHandle sys_mutex_t;
typedef xTaskHandle sys_thread_t;

typedef struct sys_mbox_s {
  xQueueHandle os_mbox;
  void *owner;
}* sys_mbox_t;


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
#define sys_mbox_set_invalid( x ) 

#define sys_sem_valid( x ) ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define sys_sem_set_invalid( x ) ( ( *x ) = NULL )

void sys_delay_ms(uint32_t ms);
sys_sem_t* sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);
sys_sem_t* sys_thread_sem_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_ARCH_H__ */

