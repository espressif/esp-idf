/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef _GKI_H_
#define _GKI_H_

#include "bt_target.h"
#include "bt_types.h"
#include "gki_common.h"
#include "gki_int.h"
#include "allocator.h"

#define ALIGN_POOL(pl_size)  ( (((pl_size) + 3) / sizeof(UINT32)) * sizeof(UINT32))
#define BUFFER_HDR_SIZE     (sizeof(BUFFER_HDR_T))                  /* Offset past header */
#define BUFFER_PADDING_SIZE (sizeof(BUFFER_HDR_T) + sizeof(UINT32)) /* Header + Magic Number */
#define MAGIC_NO            0xDDBADDBA

#define BUF_STATUS_FREE     0
#define BUF_STATUS_UNLINKED 1
#define BUF_STATUS_QUEUED   2

/* Timer list entry callback type
*/
typedef void (TIMER_CBACK)(void *p_tle);
#ifndef TIMER_PARAM_TYPE
#define TIMER_PARAM_TYPE    UINT32
#endif
/* Define a timer list entry
*/
typedef struct _tle {
    struct _tle  *p_next;
    struct _tle  *p_prev;
    TIMER_CBACK  *p_cback;
    INT32         ticks;
    INT32         ticks_initial;
    TIMER_PARAM_TYPE   param;
    TIMER_PARAM_TYPE   data;
    UINT16        event;
    UINT8         in_use;
} TIMER_LIST_ENT;

/***********************************************************************
** This queue is a general purpose buffer queue, for application use.
*/
typedef struct {
    void    *_p_first;
    void    *_p_last;
    UINT16   _count;
} BUFFER_Q;

#define GKI_PUBLIC_POOL         0       /* General pool accessible to GKI_getbuf() */
#define GKI_RESTRICTED_POOL     1       /* Inaccessible pool to GKI_getbuf() */

/***********************************************************************
** Function prototypes
*/

/* To get and release buffers, change owner and get size
*/
void   *GKI_getbuf_func(UINT16);
void   *GKI_getpoolbuf_func(UINT8);
void    GKI_freebuf(void *);
UINT16  GKI_get_buf_size (void *);
void   *GKI_getpoolbuf (UINT8);
UINT16  GKI_poolcount (UINT8);
UINT16  GKI_poolfreecount (UINT8);
UINT16  GKI_poolutilization (UINT8);

#ifdef CONFIG_BLUEDROID_MEM_DEBUG

#define GKI_getbuf(_size)                                            \
({                                                                  \
    BUFFER_HDR_T *header = osi_malloc((_size) + BUFFER_HDR_SIZE);      \
    header->status  = BUF_STATUS_UNLINKED;                          \
    header->p_next  = NULL;                                         \
    header->Type    = 0;                                            \
    header->size = (_size);                                          \
    (void *)(header + 1);                                                   \
})

#define GKI_getpoolbuf(_pool_id)                                     \
({                                                                  \
    (void *)GKI_getbuf(gki_cb.com.pool_size[(_pool_id)]);                    \
})
           
#else
#define GKI_getbuf          GKI_getbuf_func
#define GKI_getpoolbuf      GKI_getpoolbuf_func

#endif /* CONFIG_BLUEDROID_MEM_DEBUG */

/* User buffer queue management
*/
void   *GKI_dequeue  (BUFFER_Q *);
void    GKI_enqueue (BUFFER_Q *, void *);
void   *GKI_getfirst (BUFFER_Q *);
void   *GKI_getlast (BUFFER_Q *);
void   *GKI_getnext (void *);
void    GKI_init_q (BUFFER_Q *);
UINT16  GKI_queue_length(BUFFER_Q *);
BOOLEAN GKI_queue_is_empty(BUFFER_Q *);
void   *GKI_remove_from_queue (BUFFER_Q *, void *);
UINT16  GKI_get_pool_bufsize (UINT8);

/* Timer management
*/
void    GKI_delay(UINT32);

/* Disable Interrupts, Enable Interrupts
*/
void    GKI_enable(void);
void    GKI_disable(void);

/* os timer operation */
UINT32 GKI_get_os_tick_count(void);

#endif /*_GKI_H_*/
