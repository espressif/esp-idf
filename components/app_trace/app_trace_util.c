// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_app_trace_util.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LOCK ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define ESP_TEST_CPUTICKS2US(_t_)       ((_t_)/(XT_CLOCK_FREQ/1000000))

esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo)
{
    unsigned cur, elapsed;

    if (tmo->tmo != 0xFFFFFFFF) {
        cur = portGET_RUN_TIME_COUNTER_VALUE();
        if (tmo->start <= cur) {
            elapsed = cur - tmo->start;
        } else {
            elapsed = 0xFFFFFFFF - tmo->start + cur;
        }
        if (ESP_TEST_CPUTICKS2US(elapsed) >= tmo->tmo) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

esp_err_t esp_apptrace_lock_take(esp_apptrace_lock_t *lock, uint32_t tmo)
{
    uint32_t res;
#if CONFIG_SYSVIEW_ENABLE
    uint32_t recCnt;
#endif
    esp_apptrace_tmo_t sleeping_tmo;

    esp_apptrace_tmo_init(&sleeping_tmo, tmo);
    while (1) {
        res = (xPortGetCoreID() << portMUX_VAL_SHIFT) | portMUX_MAGIC_VAL;
        // first disable IRQs on this CPU, this will prevent current task from been
        // preempted by higher prio tasks, otherwise deadlock can happen:
        // when lower prio task took mux and then preempted by higher prio one which also tries to
        // get mux with INFINITE timeout
        unsigned int irq_stat = portENTER_CRITICAL_NESTED();
        // Now try to lock mux
        uxPortCompareSet(&lock->portmux.mux, portMUX_FREE_VAL, &res);
        if (res == portMUX_FREE_VAL) {
            // do not enable IRQs, we will held them disabled until mux is unlocked
            // we do not need to flush cache region for mux->irq_stat because it is used
            // to hold and restore IRQ state only for CPU which took mux, other CPUs will not use this value
            lock->irq_stat = irq_stat;
            break;
        }
#if CONFIG_SYSVIEW_ENABLE
        else if (((res & portMUX_VAL_MASK) >> portMUX_VAL_SHIFT) == xPortGetCoreID()) {
            recCnt = (res & portMUX_CNT_MASK) >> portMUX_CNT_SHIFT;
            recCnt++;
            // ets_printf("Recursive lock: recCnt=%d\n", recCnt);
            lock->portmux.mux = portMUX_MAGIC_VAL | (recCnt << portMUX_CNT_SHIFT) | (xPortGetCoreID() << portMUX_VAL_SHIFT);
            break;
        }
#endif
        // if mux is locked by other task/ISR enable IRQs and let other guys work
        portEXIT_CRITICAL_NESTED(irq_stat);

        int err = esp_apptrace_tmo_check(&sleeping_tmo);
        if (err != ESP_OK) {
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t esp_apptrace_lock_give(esp_apptrace_lock_t *lock)
{
    esp_err_t ret = ESP_OK;
    uint32_t res = 0;
    unsigned int irq_stat;
#if CONFIG_SYSVIEW_ENABLE
    uint32_t recCnt;
#endif
    res = portMUX_FREE_VAL;

    // first of all save a copy of IRQ status for this locker because uxPortCompareSet will unlock mux and tasks/ISRs
    // from other core can overwrite mux->irq_stat
    irq_stat = lock->irq_stat;
    uxPortCompareSet(&lock->portmux.mux, (xPortGetCoreID() << portMUX_VAL_SHIFT) | portMUX_MAGIC_VAL, &res);

    if ( ((res & portMUX_VAL_MASK) >> portMUX_VAL_SHIFT) == xPortGetCoreID() ) {
#if CONFIG_SYSVIEW_ENABLE
        //Lock is valid, we can return safely. Just need to check if it's a recursive lock; if so we need to decrease the refcount.
        if ( ((res & portMUX_CNT_MASK) >> portMUX_CNT_SHIFT) != 0) {
            //We locked this, but the reccount isn't zero. Decrease refcount and continue.
            recCnt = (res & portMUX_CNT_MASK) >> portMUX_CNT_SHIFT;
            recCnt--;
            lock->portmux.mux = portMUX_MAGIC_VAL | (recCnt << portMUX_CNT_SHIFT) | (xPortGetCoreID() << portMUX_VAL_SHIFT);
        }
#endif
    } else if ( res == portMUX_FREE_VAL ) {
        ret = ESP_FAIL; // should never get here
    } else {
        ret = ESP_FAIL;  // should never get here
    }
    // restore local interrupts
    portEXIT_CRITICAL_NESTED(irq_stat);
    return ret;
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////// RING BUFFER ////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint8_t *esp_apptrace_rb_produce(esp_apptrace_rb_t *rb, uint32_t size)
{
    uint8_t *ptr = rb->data + rb->wr;
    // check for avalable space
    if (rb->rd <= rb->wr) {
        // |?R......W??|
        if (rb->wr + size >= rb->size) {
            if (rb->rd == 0) {
                return NULL; // cannot wrap wr
            }
            if (rb->wr + size == rb->size) {
                rb->wr = 0;
            } else {
                // check if we can wrap wr earlier to get space for requested size
                if (size > rb->rd - 1) {
                    return NULL; // cannot wrap wr
                }
                // shrink buffer a bit, full size will be restored at rd wrapping
                rb->cur_size = rb->wr;
                rb->wr = 0;
                ptr = rb->data;
                if (rb->rd == rb->cur_size) {
                    rb->rd = 0;
                    if (rb->cur_size < rb->size) {
                        rb->cur_size = rb->size;
                    }
                }
                rb->wr += size;
            }
        } else {
            rb->wr += size;
        }
    } else {
        // |?W......R??|
        if (size > rb->rd - rb->wr - 1) {
            return NULL;
        }
        rb->wr += size;
    }
    return ptr;
}

uint8_t *esp_apptrace_rb_consume(esp_apptrace_rb_t *rb, uint32_t size)
{
    uint8_t *ptr = rb->data + rb->rd;
    if (rb->rd <= rb->wr) {
        // |?R......W??|
        if (rb->rd + size > rb->wr) {
            return NULL;
        }
        rb->rd += size;
    } else {
        // |?W......R??|
        if (rb->rd + size > rb->cur_size) {
            return NULL;
        } else if (rb->rd + size == rb->cur_size) {
            // restore full size usage
            if (rb->cur_size < rb->size) {
                rb->cur_size = rb->size;
            }
            rb->rd = 0;
        } else {
            rb->rd += size;
        }
    }
    return ptr;
}

uint32_t esp_apptrace_rb_read_size_get(esp_apptrace_rb_t *rb)
{
    uint32_t size = 0;
    if (rb->rd <= rb->wr) {
        // |?R......W??|
        size = rb->wr - rb->rd;
    } else {
        // |?W......R??|
        size = rb->cur_size - rb->rd;
    }
    return size;
}
