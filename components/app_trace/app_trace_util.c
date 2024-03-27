/*
 * SPDX-FileCopyrightText: 2017-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
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
    esp_err_t ret;

    while (1) {
        // Try enter a critical section (i.e., take the spinlock) with 0 timeout
        if (portTRY_ENTER_CRITICAL(&(lock->mux), 0) == pdTRUE) {
            return ESP_OK;
        }
        // Failed to enter the critical section, so interrupts are still enabled. Check if we have timed out.
        ret = esp_apptrace_tmo_check(tmo);
        if (ret != ESP_OK) {
            break;  // Timed out, exit now
        }
        // Haven't timed out, try again
    }
    return ret;
}

esp_err_t esp_apptrace_lock_give(esp_apptrace_lock_t *lock)
{
    portEXIT_CRITICAL(&(lock->mux));
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
