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

#ifndef _GKI_COMMON_H_
#define _GKI_COMMON_H_

#include "gki.h"

typedef struct _buffer_hdr {
    struct _buffer_hdr *p_next;   /* next buffer in the queue */
    UINT8   q_id;                 /* id of the queue */
    UINT8   status;               /* FREE, UNLINKED or QUEUED */
    UINT8   Type;
    UINT16  size;
} BUFFER_HDR_T;

typedef struct _free_queue {
    BUFFER_HDR_T *_p_first;      /* first buffer in the queue */
    BUFFER_HDR_T *_p_last;       /* last buffer in the queue */
    UINT16       size;             /* size of the buffers in the pool */
    UINT16       total;            /* toatal number of buffers */
    UINT16       cur_cnt;          /* number of  buffers currently allocated */
    UINT16       max_cnt;          /* maximum number of buffers allocated at any time */
} FREE_QUEUE_T;

/* Put all GKI variables into one control block
*/
typedef struct {
    /* Define the buffer pool management variables
    */
    FREE_QUEUE_T    freeq[GKI_NUM_TOTAL_BUF_POOLS];

    UINT16   pool_buf_size[GKI_NUM_TOTAL_BUF_POOLS];

    /* Define the buffer pool start addresses
    */
    UINT8   *pool_start[GKI_NUM_TOTAL_BUF_POOLS];   /* array of pointers to the start of each buffer pool */
    UINT8   *pool_end[GKI_NUM_TOTAL_BUF_POOLS];     /* array of pointers to the end of each buffer pool */
    UINT16   pool_size[GKI_NUM_TOTAL_BUF_POOLS];    /* actual size of the buffers in a pool */

    /* Define the buffer pool access control variables */
    UINT16      pool_access_mask;                   /* Bits are set if the corresponding buffer pool is a restricted pool */
} tGKI_COM_CB;

/* Internal GKI function prototypes
*/
void      gki_buffer_init(void);
void      gki_buffer_cleanup(void);

#endif /*_GKI_COMMON_H_*/
