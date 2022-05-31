/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
//
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_app_trace_util.h"
#include "sdkconfig.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Locks /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if ESP_APPTRACE_PRINT_LOCK
static esp_apptrace_lock_t s_log_lock = {.irq_stat = 0, .portmux = portMUX_INITIALIZER_UNLOCKED};
#endif

int esp_apptrace_log_lock(void)
{
#if ESP_APPTRACE_PRINT_LOCK
    esp_apptrace_tmo_t tmo;
    esp_apptrace_tmo_init(&tmo, ESP_APPTRACE_TMO_INFINITE);
    int ret = esp_apptrace_lock_take(&s_log_lock, &tmo);
    return ret;
#else
    return 0;
#endif
}

void esp_apptrace_log_unlock(void)
{
 #if ESP_APPTRACE_PRINT_LOCK
    esp_apptrace_lock_give(&s_log_lock);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// TIMEOUT /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo)
{
    if (tmo->tmo != (int64_t)-1) {
        tmo->elapsed = esp_timer_get_time() - tmo->start;
        if (tmo->elapsed >= tmo->tmo) {
            return ESP_ERR_TIMEOUT;
        }
    }
    return ESP_OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////// LOCK ////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

esp_err_t esp_apptrace_lock_take(esp_apptrace_lock_t *lock, esp_apptrace_tmo_t *tmo)
{
    int res;

    while (1) {
        //Todo: Replace the current locking mechanism and int_state with portTRY_ENTER_CRITICAL() instead.
        // do not overwrite lock->int_state before we actually acquired the mux
#if CONFIG_FREERTOS_SMP
        unsigned int_state = portDISABLE_INTERRUPTS();
#else
        unsigned int_state = portSET_INTERRUPT_MASK_FROM_ISR();
#endif
        bool success = spinlock_acquire(&lock->mux, 0);
        if (success) {
            lock->int_state = int_state;
            return ESP_OK;
        }
#if CONFIG_FREERTOS_SMP
        portRESTORE_INTERRUPTS(int_state);
#else
        portCLEAR_INTERRUPT_MASK_FROM_ISR(int_state);
#endif
        // we can be preempted from this place till the next call (above) to portSET_INTERRUPT_MASK_FROM_ISR()
        res = esp_apptrace_tmo_check(tmo);
        if (res != ESP_OK) {
            break;
        }
    }
    return res;
}

esp_err_t esp_apptrace_lock_give(esp_apptrace_lock_t *lock)
{
    // save lock's irq state value for this CPU
    unsigned int_state = lock->int_state;
    // after call to the following func we can not be sure that lock->int_state
    // is not overwritten by other CPU who has acquired the mux just after we released it. See esp_apptrace_lock_take().
    spinlock_release(&lock->mux);
#if CONFIG_FREERTOS_SMP
    portRESTORE_INTERRUPTS(int_state);
#else
    portCLEAR_INTERRUPT_MASK_FROM_ISR(int_state);
#endif
    return ESP_OK;
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

uint32_t esp_apptrace_rb_write_size_get(esp_apptrace_rb_t *rb)
{
    uint32_t size = 0;
    if (rb->rd <= rb->wr) {
        // |?R......W??|
        size = rb->size - rb->wr;
        if (size && rb->rd == 0) {
            size--;
        }
    } else {
        // |?W......R??|
        size = rb->rd - rb->wr - 1;
    }
    return size;
}
