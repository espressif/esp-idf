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
///////////////////////////////// TIMEOUT /////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// TODO: get actual clock from PLL config
#define ESP_APPTRACE_CPUTICKS2US(_t_)       ((_t_)/(XT_CLOCK_FREQ/1000000))
#define ESP_APPTRACE_US2CPUTICKS(_t_)       ((_t_)*(XT_CLOCK_FREQ/1000000))

esp_err_t esp_apptrace_tmo_check(esp_apptrace_tmo_t *tmo)
{
    if (tmo->tmo != ESP_APPTRACE_TMO_INFINITE) {
        unsigned cur = portGET_RUN_TIME_COUNTER_VALUE();
        if (tmo->start <= cur) {
            tmo->elapsed = ESP_APPTRACE_CPUTICKS2US(cur - tmo->start);
        } else {
            tmo->elapsed = ESP_APPTRACE_CPUTICKS2US(0xFFFFFFFF - tmo->start + cur);
        }
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
    lock->int_state = portENTER_CRITICAL_NESTED();

    unsigned now = ESP_APPTRACE_CPUTICKS2US(portGET_RUN_TIME_COUNTER_VALUE()); // us
    unsigned end = tmo->start + tmo->tmo;
    if (now > end) {
        goto timeout;
    }
    unsigned remaining = end - now; // us

    bool success = vPortCPUAcquireMutexTimeout(&lock->mux, ESP_APPTRACE_US2CPUTICKS(remaining));
    if (success) {
        return ESP_OK;
    }

 timeout:
    portEXIT_CRITICAL_NESTED(lock->int_state);
    return ESP_ERR_TIMEOUT;
}

esp_err_t esp_apptrace_lock_give(esp_apptrace_lock_t *lock)
{
    vPortCPUReleaseMutex(&lock->mux);
    portEXIT_CRITICAL_NESTED(lock->int_state);
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
