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

#include "bt_trace.h"
#include "allocator.h"
#include "gki_int.h"

/*******************************************************************************
**
** Function         gki_init_free_queue
**
** Description      Internal function called at startup to initialize a free
**                  queue. It is called once for each free queue.
**
** Returns          void
**
*******************************************************************************/
static void gki_init_free_queue (UINT8 id, UINT16 size, UINT16 total, void *p_mem)
{
    UINT16           i;
    UINT16           act_size;
    BUFFER_HDR_T    *hdr;
    BUFFER_HDR_T    *hdr1 = NULL;
    UINT32          *magic;
    INT32            tempsize = size;
    tGKI_COM_CB     *p_cb = &gki_cb.com;

    /* Ensure an even number of longwords */
    tempsize = (INT32)ALIGN_POOL(size);
    act_size = (UINT16)(tempsize + BUFFER_PADDING_SIZE);

    /* Remember pool start and end addresses */
    if (p_mem) {
        p_cb->pool_start[id] = (UINT8 *)p_mem;
        p_cb->pool_end[id]   = (UINT8 *)p_mem + (act_size * total);
    }

    p_cb->pool_size[id]  = act_size;

    p_cb->freeq[id].size      = (UINT16) tempsize;
    p_cb->freeq[id].total     = total;
    p_cb->freeq[id].cur_cnt   = 0;
    p_cb->freeq[id].max_cnt   = 0;

    /* Initialize  index table */
    if (p_mem) {
        hdr = (BUFFER_HDR_T *)p_mem;
        p_cb->freeq[id]._p_first = hdr;
        for (i = 0; i < total; i++) {
            hdr->q_id    = id;
            hdr->status  = BUF_STATUS_FREE;
            magic        = (UINT32 *)((UINT8 *)hdr + BUFFER_HDR_SIZE + tempsize);
            *magic       = MAGIC_NO;
            hdr1         = hdr;
            hdr          = (BUFFER_HDR_T *)((UINT8 *)hdr + act_size);
            hdr1->p_next = hdr;
        }
        hdr1->p_next = NULL;
        p_cb->freeq[id]._p_last = hdr1;
    }
}

void gki_buffer_cleanup(void)
{
    UINT8   i;
    tGKI_COM_CB *p_cb = &gki_cb.com;

    for (i = 0; i < GKI_NUM_FIXED_BUF_POOLS; i++) {
        if ( 0 < p_cb->freeq[i].max_cnt ) {
            osi_free(p_cb->pool_start[i]);

            p_cb->freeq[i].cur_cnt   = 0;
            p_cb->freeq[i].max_cnt   = 0;
            p_cb->freeq[i]._p_first   = NULL;
            p_cb->freeq[i]._p_last    = NULL;

            p_cb->pool_start[i] = NULL;
            p_cb->pool_end[i]   = NULL;
            p_cb->pool_size[i]  = 0;
        }
    }
}

/*******************************************************************************
**
** Function         gki_buffer_init
**
** Description      Called once internally by GKI at startup to initialize all
**                  buffers and free buffer pools.
**
** Returns          void
**
*******************************************************************************/
void gki_buffer_init(void)
{
    static const struct {
        uint16_t size;
        uint16_t count;
    } buffer_info[GKI_NUM_FIXED_BUF_POOLS] = {
        { GKI_BUF0_SIZE, GKI_BUF0_MAX },
        { GKI_BUF1_SIZE, GKI_BUF1_MAX },
        { GKI_BUF2_SIZE, GKI_BUF2_MAX },
        { GKI_BUF3_SIZE, GKI_BUF3_MAX },
        { GKI_BUF4_SIZE, GKI_BUF4_MAX },
        { GKI_BUF5_SIZE, GKI_BUF5_MAX },
        { GKI_BUF6_SIZE, GKI_BUF6_MAX },
        { GKI_BUF7_SIZE, GKI_BUF7_MAX },
        { GKI_BUF8_SIZE, GKI_BUF8_MAX },
        { GKI_BUF9_SIZE, GKI_BUF9_MAX },
    };

    tGKI_COM_CB *p_cb = &gki_cb.com;

    for (int i = 0; i < GKI_NUM_TOTAL_BUF_POOLS; i++) {
        p_cb->pool_start[i] = NULL;
        p_cb->pool_end[i]   = NULL;
        p_cb->pool_size[i]  = 0;

        p_cb->freeq[i]._p_first = 0;
        p_cb->freeq[i]._p_last  = 0;
        p_cb->freeq[i].size    = 0;
        p_cb->freeq[i].total   = 0;
        p_cb->freeq[i].cur_cnt = 0;
        p_cb->freeq[i].max_cnt = 0;
    }

    /* Use default from target.h */
    p_cb->pool_access_mask = GKI_DEF_BUFPOOL_PERM_MASK;

    for (int i = 0; i < GKI_NUM_FIXED_BUF_POOLS; ++i) {
        gki_init_free_queue(i, buffer_info[i].size, buffer_info[i].count, NULL);
    }
}

/*******************************************************************************
**
** Function         GKI_init_q
**
** Description      Called by an application to initialize a buffer queue.
**
** Returns          void
**
*******************************************************************************/
void GKI_init_q (BUFFER_Q *p_q)
{
    p_q->_p_first = p_q->_p_last = NULL;
    p_q->_count = 0;
}

/*******************************************************************************
**
** Function         GKI_getbuf_func
**
** Description      Called by an application to get a free buffer which
**                  is of size greater or equal to the requested size.
**
**                  Note: This routine only takes buffers from public pools.
**                        It will not use any buffers from pools
**                        marked GKI_RESTRICTED_POOL.
**
** Parameters       size - (input) number of bytes needed.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void *GKI_getbuf_func(UINT16 size)
{
    BUFFER_HDR_T *header = osi_malloc(size + BUFFER_HDR_SIZE);
    header->status  = BUF_STATUS_UNLINKED;
    header->p_next  = NULL;
    header->Type    = 0;
    header->size = size;

    return header + 1;
}

/*******************************************************************************
**
** Function         GKI_getpoolbuf_func
**
** Description      Called by an application to get a free buffer from
**                  a specific buffer pool.
**
**                  Note: If there are no more buffers available from the pool,
**                        the public buffers are searched for an available buffer.
**
** Parameters       pool_id - (input) pool ID to get a buffer out of.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void *GKI_getpoolbuf_func(UINT8 pool_id)
{
    return GKI_getbuf_func(gki_cb.com.pool_size[pool_id]);
}

/*******************************************************************************
**
** Function         GKI_freebuf
**
** Description      Called by an application to return a buffer to the free pool.
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          void
**
*******************************************************************************/
void GKI_freebuf (void *p_buf)
{
    osi_free((BUFFER_HDR_T *)p_buf - 1);
}

/*******************************************************************************
**
** Function         GKI_get_buf_size
**
** Description      Called by an application to get the size of a buffer.
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          the size of the buffer
**
*******************************************************************************/
UINT16 GKI_get_buf_size (void *p_buf)
{
    BUFFER_HDR_T *header = (BUFFER_HDR_T *)p_buf - 1;
    return header->size;
}

/*******************************************************************************
**
** Function         GKI_enqueue
**
** Description      Enqueue a buffer at the tail of the queue
**
** Parameters:      p_q  -  (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          void
**
*******************************************************************************/
void GKI_enqueue (BUFFER_Q *p_q, void *p_buf)
{
    BUFFER_HDR_T *p_hdr = (BUFFER_HDR_T *) ((UINT8 *) p_buf - BUFFER_HDR_SIZE);
    assert(p_hdr->status == BUF_STATUS_UNLINKED);

    GKI_disable();

    /* Since the queue is exposed (C vs C++), keep the pointers in exposed format */
    if (p_q->_p_last) {
        BUFFER_HDR_T *_p_last_hdr = (BUFFER_HDR_T *)((UINT8 *)p_q->_p_last - BUFFER_HDR_SIZE);
        _p_last_hdr->p_next = p_hdr;
    } else {
        p_q->_p_first = p_buf;
    }

    p_q->_p_last = p_buf;
    p_q->_count++;

    p_hdr->p_next = NULL;
    p_hdr->status = BUF_STATUS_QUEUED;

    GKI_enable();
}

/*******************************************************************************
**
** Function         GKI_dequeue
**
** Description      Dequeues a buffer from the head of a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void *GKI_dequeue (BUFFER_Q *p_q)
{
    BUFFER_HDR_T    *p_hdr;

    GKI_disable();

    if (!p_q || !p_q->_count) {
        GKI_enable();
        return (NULL);
    }

    p_hdr = (BUFFER_HDR_T *)((UINT8 *)p_q->_p_first - BUFFER_HDR_SIZE);

    /* Keep buffers such that GKI header is invisible
    */
    if (p_hdr->p_next) {
        p_q->_p_first = ((UINT8 *)p_hdr->p_next + BUFFER_HDR_SIZE);
    } else {
        p_q->_p_first = NULL;
        p_q->_p_last  = NULL;
    }

    p_q->_count--;

    p_hdr->p_next = NULL;
    p_hdr->status = BUF_STATUS_UNLINKED;

    GKI_enable();

    return ((UINT8 *)p_hdr + BUFFER_HDR_SIZE);
}

/*******************************************************************************
**
** Function         GKI_remove_from_queue
**
** Description      Dequeue a buffer from the middle of the queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void *GKI_remove_from_queue (BUFFER_Q *p_q, void *p_buf)
{
    BUFFER_HDR_T    *p_prev;
    BUFFER_HDR_T    *p_buf_hdr;

    GKI_disable();

    if (p_buf == p_q->_p_first) {
        GKI_enable();
        return (GKI_dequeue (p_q));
    }

    p_buf_hdr = (BUFFER_HDR_T *)((UINT8 *)p_buf - BUFFER_HDR_SIZE);
    p_prev    = (BUFFER_HDR_T *)((UINT8 *)p_q->_p_first - BUFFER_HDR_SIZE);

    for ( ; p_prev; p_prev = p_prev->p_next) {
        /* If the previous points to this one, move the pointers around */
        if (p_prev->p_next == p_buf_hdr) {
            p_prev->p_next = p_buf_hdr->p_next;

            /* If we are removing the last guy in the queue, update _p_last */
            if (p_buf == p_q->_p_last) {
                p_q->_p_last = p_prev + 1;
            }

            /* One less in the queue */
            p_q->_count--;

            /* The buffer is now unlinked */
            p_buf_hdr->p_next = NULL;
            p_buf_hdr->status = BUF_STATUS_UNLINKED;

            GKI_enable();
            return (p_buf);
        }
    }

    GKI_enable();
    return (NULL);
}

/*******************************************************************************
**
** Function         GKI_getfirst
**
** Description      Return a pointer to the first buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void *GKI_getfirst (BUFFER_Q *p_q)
{
    return (p_q->_p_first);
}

/*******************************************************************************
**
** Function         GKI_getlast
**
** Description      Return a pointer to the last buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void *GKI_getlast (BUFFER_Q *p_q)
{
    return (p_q->_p_last);
}

/*******************************************************************************
**
** Function         GKI_getnext
**
** Description      Return a pointer to the next buffer in a queue
**
** Parameters:      p_buf  - (input) pointer to the buffer to find the next one from.
**
** Returns          NULL if no more buffers in the queue, else next buffer address
**
*******************************************************************************/
void *GKI_getnext (void *p_buf)
{
    BUFFER_HDR_T    *p_hdr;

    p_hdr = (BUFFER_HDR_T *) ((UINT8 *) p_buf - BUFFER_HDR_SIZE);

    if (p_hdr->p_next) {
        return ((UINT8 *)p_hdr->p_next + BUFFER_HDR_SIZE);
    } else {
        return (NULL);
    }
}

/*******************************************************************************
**
** Function         GKI_queue_is_empty
**
** Description      Check the status of a queue.
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          TRUE if queue is empty, else FALSE
**
*******************************************************************************/
BOOLEAN GKI_queue_is_empty(BUFFER_Q *p_q)
{
    return ((BOOLEAN) (p_q->_count == 0));
}

UINT16 GKI_queue_length(BUFFER_Q *p_q)
{
    return p_q->_count;
}

/*******************************************************************************
**
** Function         GKI_poolcount
**
** Description      Called by an application to get the total number of buffers
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          the total number of buffers in the pool
**
*******************************************************************************/
UINT16 GKI_poolcount (UINT8 pool_id)
{
    if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) {
        return (0);
    }

    return (gki_cb.com.freeq[pool_id].total);
}

/*******************************************************************************
**
** Function         GKI_poolfreecount
**
** Description      Called by an application to get the number of free buffers
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          the number of free buffers in the pool
**
*******************************************************************************/
UINT16 GKI_poolfreecount (UINT8 pool_id)
{
    FREE_QUEUE_T  *Q;

    if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) {
        return (0);
    }

    Q  = &gki_cb.com.freeq[pool_id];

    return ((UINT16)(Q->total - Q->cur_cnt));
}

/*******************************************************************************
**
** Function         GKI_get_pool_bufsize
**
** Description      Called by an application to get the size of buffers in a pool
**
** Parameters       Pool ID.
**
** Returns          the size of buffers in the pool
**
*******************************************************************************/
UINT16 GKI_get_pool_bufsize (UINT8 pool_id)
{
    if (pool_id < GKI_NUM_TOTAL_BUF_POOLS) {
        return (gki_cb.com.freeq[pool_id].size);
    }

    return (0);
}

/*******************************************************************************
**
** Function         GKI_poolutilization
**
** Description      Called by an application to get the buffer utilization
**                  in the specified buffer pool.
**
** Parameters       pool_id - (input) pool ID to get the free count of.
**
** Returns          % of buffers used from 0 to 100
**
*******************************************************************************/
UINT16 GKI_poolutilization (UINT8 pool_id)
{
    FREE_QUEUE_T  *Q;

    if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS) {
        return (100);
    }

    Q  = &gki_cb.com.freeq[pool_id];

    if (Q->total == 0) {
        return (100);
    }

    return ((Q->cur_cnt * 100) / Q->total);
}
