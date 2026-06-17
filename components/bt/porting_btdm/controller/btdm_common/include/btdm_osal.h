/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BTDM_OSAL_H_
#define _BTDM_OSAL_H_

#include "sdkconfig.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum btdm_osal_error {
    BTDM_OSAL_OK = 0,
    BTDM_OSAL_ENOMEM = 1,
    BTDM_OSAL_EINVAL = 2,
    BTDM_OSAL_INVALID_PARM = 3,
    BTDM_OSAL_MEM_NOT_ALIGNED = 4,
    BTDM_OSAL_BAD_MUTEX = 5,
    BTDM_OSAL_TIMEOUT = 6,
    BTDM_OSAL_ERR_IN_ISR = 7,  /* Function cannot be called from ISR */
    BTDM_OSAL_ERR_PRIV = 8,    /* Privileged access error */
    BTDM_OSAL_NOT_STARTED = 9, /* OS must be started to call this function, but isn't */
    BTDM_OSAL_ENOENT = 10,     /* No such thing */
    BTDM_OSAL_EBUSY = 11,      /* Resource busy */
    BTDM_OSAL_ERROR = 12,      /* Generic Error */
};

enum btdm_osal_malloc_flag {
    BTDM_OSAL_MALLOC_F_INTERNAL = 0,
    BTDM_OSAL_MALLOC_F_NORMAL,
};
typedef enum btdm_osal_malloc_flag btdm_osal_malloc_flag_t;

typedef struct {
    uint16_t evt_count;
    uint16_t evtq_count;
    uint16_t co_count;
    uint16_t sem_count;
    uint16_t mutex_count;
} btdm_osal_elem_num_t;

struct btdm_osal_event {
    void *event;
};

struct btdm_osal_eventq {
    void *eventq;
};

struct btdm_osal_callout {
    void *co;
};

struct btdm_osal_mutex {
    void *mutex;
};

struct btdm_osal_sem {
    void *sem;
};

/*
 * Type definitions for BTDM OSAL
 */
typedef uint32_t btdm_osal_time_t;
typedef int btdm_osal_error_t;
typedef void btdm_osal_event_fn(struct btdm_osal_event *ev);
typedef void btdm_osal_task_fn(void *arg);
typedef void btdm_osal_intr_fn(void *arg);
typedef void *btdm_osal_task_handle_t;
typedef void *btdm_osal_intr_handle_t;

/*
 * Function declarations for BTDM OSAL
 */
void wr_btdm_osal_eventq_init(struct btdm_osal_eventq *evq);
void wr_btdm_osal_eventq_deinit(struct btdm_osal_eventq *evq);
struct btdm_osal_event *wr_btdm_osal_eventq_get(struct btdm_osal_eventq *evq, btdm_osal_time_t tmo);
void wr_btdm_osal_eventq_put(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev);
void wr_btdm_osal_eventq_put_to_front(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev);
void wr_btdm_osal_eventq_remove(struct btdm_osal_eventq *evq, struct btdm_osal_event *ev);
bool wr_btdm_osal_eventq_is_empty(struct btdm_osal_eventq *evq);

void wr_btdm_osal_event_run(struct btdm_osal_event *ev);
void wr_btdm_osal_event_init(struct btdm_osal_event *ev, btdm_osal_event_fn *fn, void *arg);
void wr_btdm_osal_event_deinit(struct btdm_osal_event *ev);
void wr_btdm_osal_event_reset(struct btdm_osal_event *ev);
bool wr_btdm_osal_event_is_queued(struct btdm_osal_event *ev);
void *wr_btdm_osal_event_get_arg(struct btdm_osal_event *ev);
void wr_btdm_osal_event_set_arg(struct btdm_osal_event *ev, void *arg);

btdm_osal_error_t wr_btdm_osal_mutex_init(struct btdm_osal_mutex *mu);
btdm_osal_error_t wr_btdm_osal_mutex_deinit(struct btdm_osal_mutex *mu);
btdm_osal_error_t wr_btdm_osal_mutex_pend(struct btdm_osal_mutex *mu, btdm_osal_time_t timeout);
btdm_osal_error_t wr_btdm_osal_mutex_release(struct btdm_osal_mutex *mu);

btdm_osal_error_t wr_btdm_osal_sem_init(struct btdm_osal_sem *sem, uint16_t tokens);
btdm_osal_error_t wr_btdm_osal_sem_deinit(struct btdm_osal_sem *sem);
btdm_osal_error_t wr_btdm_osal_sem_pend(struct btdm_osal_sem *sem, btdm_osal_time_t timeout);
btdm_osal_error_t wr_btdm_osal_sem_release(struct btdm_osal_sem *sem);
uint16_t wr_btdm_osal_sem_get_count(struct btdm_osal_sem *sem);

int wr_btdm_osal_callout_init(struct btdm_osal_callout *co, struct btdm_osal_eventq *evq,
                              btdm_osal_event_fn *ev_cb, void *ev_arg);
void wr_btdm_osal_callout_deinit(struct btdm_osal_callout *co);
btdm_osal_error_t wr_btdm_osal_callout_reset(struct btdm_osal_callout *co, btdm_osal_time_t ticks);
void wr_btdm_osal_callout_mem_reset(struct btdm_osal_callout *co);
void wr_btdm_osal_callout_stop(struct btdm_osal_callout *co);
bool wr_btdm_osal_callout_is_active(struct btdm_osal_callout *co);
btdm_osal_time_t wr_btdm_osal_callout_get_ticks(struct btdm_osal_callout *co);
btdm_osal_time_t wr_btdm_osal_callout_remaining_ticks(struct btdm_osal_callout *co,
                                                      btdm_osal_time_t time);
void wr_btdm_osal_callout_set_arg(struct btdm_osal_callout *co, void *arg);

btdm_osal_time_t wr_btdm_osal_time_get(void);
btdm_osal_error_t wr_btdm_osal_time_ms_to_ticks(uint32_t ms, btdm_osal_time_t *out_ticks);
btdm_osal_error_t wr_btdm_osal_time_ticks_to_ms(btdm_osal_time_t ticks, uint32_t *out_ms);
btdm_osal_time_t wr_btdm_osal_time_ms_to_ticks32(uint32_t ms);
uint32_t wr_btdm_osal_time_ticks_to_ms32(btdm_osal_time_t ticks);

uint32_t wr_btdm_osal_hw_enter_critical(void);
void wr_btdm_osal_hw_exit_critical(uint32_t ctx);
uint8_t wr_btdm_osal_hw_is_in_critical(void);

uint32_t wr_btdm_osal_get_time_forever(void);

int wr_btdm_osal_task_create(btdm_osal_task_fn *fn, const char *name, uint32_t stack_size,
                             void *arg, uint32_t priority, btdm_osal_task_handle_t *task_handle,
                             uint32_t core_id);
void wr_btdm_osal_task_delete(btdm_osal_task_handle_t task_handle);

int wr_btdm_osal_intr_alloc(int src, int flags, btdm_osal_intr_fn *fn, void *arg,
                            btdm_osal_intr_handle_t *intr_handle);
int wr_btdm_osal_intr_free(btdm_osal_intr_handle_t intr_handle);

void *wr_btdm_osal_malloc(uint32_t size, btdm_osal_malloc_flag_t flags);
void wr_btdm_osal_free(void *ptr);

#if !CONFIG_BT_CTRL_MULTI_LINK_ENABLED
void *wr_btdm_osal_mmgmt_block_malloc(uint32_t size);
void wr_btdm_osal_mmgmt_block_free(void *ptr);
void wr_btdm_osal_mmgmt_block_copy(void *dst, const void *src, uint16_t size);
#endif /* !CONFIG_BT_CTRL_MULTI_LINK_ENABLED */

// void * wr_btdm_osal_ets_delay_us(uint32_t us);
int wr_btdm_osal_read_efuse_mac(uint8_t *mac);
void wr_btdm_osal_srand(uint32_t seed);
int wr_btdm_osal_rand(void);

int btdm_osal_elem_mempool_init(btdm_osal_elem_num_t *elem_num);

void btdm_osal_elem_mempool_deinit(void);


#define BTDM_OSAL_WR_FUNC(f)        wr_ ## f

#define btdm_osal_eventq_init                       BTDM_OSAL_WR_FUNC(btdm_osal_eventq_init)
#define btdm_osal_eventq_deinit                     BTDM_OSAL_WR_FUNC(btdm_osal_eventq_deinit)
#define btdm_osal_eventq_get                        BTDM_OSAL_WR_FUNC(btdm_osal_eventq_get)
#define btdm_osal_eventq_put                        BTDM_OSAL_WR_FUNC(btdm_osal_eventq_put)
#define btdm_osal_eventq_put_to_front               BTDM_OSAL_WR_FUNC(btdm_osal_eventq_put_to_front)
#define btdm_osal_eventq_remove                     BTDM_OSAL_WR_FUNC(btdm_osal_eventq_remove)
#define btdm_osal_eventq_is_empty                   BTDM_OSAL_WR_FUNC(btdm_osal_eventq_is_empty)
#define btdm_osal_event_run                         BTDM_OSAL_WR_FUNC(btdm_osal_event_run)
#define btdm_osal_event_init                        BTDM_OSAL_WR_FUNC(btdm_osal_event_init)
#define btdm_osal_event_deinit                      BTDM_OSAL_WR_FUNC(btdm_osal_event_deinit)
#define btdm_osal_event_reset                       BTDM_OSAL_WR_FUNC(btdm_osal_event_reset)
#define btdm_osal_event_is_queued                   BTDM_OSAL_WR_FUNC(btdm_osal_event_is_queued)
#define btdm_osal_event_get_arg                     BTDM_OSAL_WR_FUNC(btdm_osal_event_get_arg)
#define btdm_osal_event_set_arg                     BTDM_OSAL_WR_FUNC(btdm_osal_event_set_arg)
#define btdm_osal_mutex_init                        BTDM_OSAL_WR_FUNC(btdm_osal_mutex_init)
#define btdm_osal_mutex_deinit                      BTDM_OSAL_WR_FUNC(btdm_osal_mutex_deinit)
#define btdm_osal_mutex_pend                        BTDM_OSAL_WR_FUNC(btdm_osal_mutex_pend)
#define btdm_osal_mutex_release                     BTDM_OSAL_WR_FUNC(btdm_osal_mutex_release)
#define btdm_osal_sem_init                          BTDM_OSAL_WR_FUNC(btdm_osal_sem_init)
#define btdm_osal_sem_deinit                        BTDM_OSAL_WR_FUNC(btdm_osal_sem_deinit)
#define btdm_osal_sem_pend                          BTDM_OSAL_WR_FUNC(btdm_osal_sem_pend)
#define btdm_osal_sem_release                       BTDM_OSAL_WR_FUNC(btdm_osal_sem_release)
#define btdm_osal_sem_get_count                     BTDM_OSAL_WR_FUNC(btdm_osal_sem_get_count)
#define btdm_osal_callout_init                      BTDM_OSAL_WR_FUNC(btdm_osal_callout_init)
#define btdm_osal_callout_deinit                    BTDM_OSAL_WR_FUNC(btdm_osal_callout_deinit)
#define btdm_osal_callout_reset                     BTDM_OSAL_WR_FUNC(btdm_osal_callout_reset)
#define btdm_osal_callout_mem_reset                 BTDM_OSAL_WR_FUNC(btdm_osal_callout_mem_reset)
#define btdm_osal_callout_stop                      BTDM_OSAL_WR_FUNC(btdm_osal_callout_stop)
#define btdm_osal_callout_is_active                 BTDM_OSAL_WR_FUNC(btdm_osal_callout_is_active)
#define btdm_osal_callout_get_ticks                 BTDM_OSAL_WR_FUNC(btdm_osal_callout_get_ticks)
#define btdm_osal_callout_remaining_ticks           BTDM_OSAL_WR_FUNC(btdm_osal_callout_remaining_ticks)
#define btdm_osal_callout_set_arg                   BTDM_OSAL_WR_FUNC(btdm_osal_callout_set_arg)
#define btdm_osal_time_get                          BTDM_OSAL_WR_FUNC(btdm_osal_time_get)
#define btdm_osal_time_ms_to_ticks                  BTDM_OSAL_WR_FUNC(btdm_osal_time_ms_to_ticks)
#define btdm_osal_time_ticks_to_ms                  BTDM_OSAL_WR_FUNC(btdm_osal_time_ticks_to_ms)
#define btdm_osal_time_ms_to_ticks32                BTDM_OSAL_WR_FUNC(btdm_osal_time_ms_to_ticks32)
#define btdm_osal_time_ticks_to_ms32                BTDM_OSAL_WR_FUNC(btdm_osal_time_ticks_to_ms32)
#define btdm_osal_hw_enter_critical                 BTDM_OSAL_WR_FUNC(btdm_osal_hw_enter_critical)
#define btdm_osal_hw_exit_critical                  BTDM_OSAL_WR_FUNC(btdm_osal_hw_exit_critical)
#define btdm_osal_hw_is_in_critical                 BTDM_OSAL_WR_FUNC(btdm_osal_hw_is_in_critical)
#define btdm_osal_get_time_forever                  BTDM_OSAL_WR_FUNC(btdm_osal_get_time_forever)
#define btdm_osal_task_create                       BTDM_OSAL_WR_FUNC(btdm_osal_task_create)
#define btdm_osal_task_delete                       BTDM_OSAL_WR_FUNC(btdm_osal_task_delete)
#define btdm_osal_intr_alloc                        BTDM_OSAL_WR_FUNC(btdm_osal_intr_alloc)
#define btdm_osal_intr_free                         BTDM_OSAL_WR_FUNC(btdm_osal_intr_free)
#define btdm_osal_malloc                            BTDM_OSAL_WR_FUNC(btdm_osal_malloc)
#define btdm_osal_free                              BTDM_OSAL_WR_FUNC(btdm_osal_free)
#define btdm_osal_mmgmt_block_malloc                BTDM_OSAL_WR_FUNC(btdm_osal_mmgmt_block_malloc)
#define btdm_osal_mmgmt_block_free                  BTDM_OSAL_WR_FUNC(btdm_osal_mmgmt_block_free)
#define btdm_osal_mmgmt_block_copy                  BTDM_OSAL_WR_FUNC(btdm_osal_mmgmt_block_copy)
#define btdm_osal_ets_delay_us                      BTDM_OSAL_WR_FUNC(btdm_osal_ets_delay_us)
#define btdm_osal_read_efuse_mac                    BTDM_OSAL_WR_FUNC(btdm_osal_read_efuse_mac)
#define btdm_osal_rand                              BTDM_OSAL_WR_FUNC(btdm_osal_rand)

#ifdef __cplusplus
}
#endif

#endif /* _BTDM_OSAL_H_ */
