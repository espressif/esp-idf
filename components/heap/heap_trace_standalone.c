// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include <string.h>
#include <sdkconfig.h>

#define HEAP_TRACE_SRCFILE /* don't warn on inclusion here */
#include "esp_heap_trace.h"
#undef HEAP_TRACE_SRCFILE

#include "esp_attr.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define STACK_DEPTH CONFIG_HEAP_TRACING_STACK_DEPTH

#if CONFIG_HEAP_TRACING_STANDALONE

static portMUX_TYPE trace_mux = portMUX_INITIALIZER_UNLOCKED;
static bool tracing;
static heap_trace_mode_t mode;

/* Buffer used for records, starting at offset 0
*/
static heap_trace_record_t *buffer;
static size_t total_records;

/* Count of entries logged in the buffer.

   Maximum total_records
*/
static size_t count;

/* Actual number of allocations logged */
static size_t total_allocations;

/* Actual number of frees logged */
static size_t total_frees;

/* Has the buffer overflowed and lost trace entries? */
static bool has_overflowed = false;

esp_err_t heap_trace_init_standalone(heap_trace_record_t *record_buffer, size_t num_records)
{
    if (tracing) {
        return ESP_ERR_INVALID_STATE;
    }
    buffer = record_buffer;
    total_records = num_records;
    memset(buffer, 0, num_records * sizeof(heap_trace_record_t));
    return ESP_OK;
}

esp_err_t heap_trace_start(heap_trace_mode_t mode_param)
{
    if (buffer == NULL || total_records == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    portENTER_CRITICAL(&trace_mux);

    tracing = false;
    mode = mode_param;
    count = 0;
    total_allocations = 0;
    total_frees = 0;
    has_overflowed = false;
    heap_trace_resume();

    portEXIT_CRITICAL(&trace_mux);
    return ESP_OK;
}

static esp_err_t set_tracing(bool enable)
{
    if (tracing == enable) {
        return ESP_ERR_INVALID_STATE;
    }
    tracing = enable;
    return ESP_OK;
}

esp_err_t heap_trace_stop(void)
{
    return set_tracing(false);
}

esp_err_t heap_trace_resume(void)
{
    return set_tracing(true);
}

size_t heap_trace_get_count(void)
{
    return count;
}

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *record)
{
    if (record == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    esp_err_t result = ESP_OK;

    portENTER_CRITICAL(&trace_mux);
    if (index >= count) {
        result = ESP_ERR_INVALID_ARG; /* out of range for 'count' */
    } else {
        memcpy(record, &buffer[index], sizeof(heap_trace_record_t));
    }
    portEXIT_CRITICAL(&trace_mux);
    return result;
}


void heap_trace_dump(void)
{
    size_t delta_size = 0;
    size_t delta_allocs = 0;
    printf("%u allocations trace (%u entry buffer)\n",
           count, total_records);
    size_t start_count = count;
    for (int i = 0; i < count; i++) {
        heap_trace_record_t *rec = &buffer[i];

        if (rec->address != NULL) {
            printf("%d bytes (@ %p) allocated CPU %d ccount 0x%08x caller ",
                   rec->size, rec->address, rec->ccount & 1, rec->ccount & ~3);
            for (int j = 0; j < STACK_DEPTH && rec->alloced_by[j] != 0; j++) {
                printf("%p%s", rec->alloced_by[j],
                       (j < STACK_DEPTH - 1) ? ":" : "");
            }

            if (mode != HEAP_TRACE_ALL || STACK_DEPTH == 0 || rec->freed_by[0] == NULL) {
                delta_size += rec->size;
                delta_allocs++;
                printf("\n");
            } else {
                printf("\nfreed by ");
                for (int j = 0; j < STACK_DEPTH; j++) {
                    printf("%p%s", rec->freed_by[j],
                           (j < STACK_DEPTH - 1) ? ":" : "\n");
                }
            }
        }
    }
    if (mode == HEAP_TRACE_ALL) {
        printf("%u bytes alive in trace (%u/%u allocations)\n",
               delta_size, delta_allocs, heap_trace_get_count());
    } else {
        printf("%u bytes 'leaked' in trace (%u allocations)\n", delta_size, delta_allocs);
    }
    printf("total allocations %u total frees %u\n", total_allocations, total_frees);
    if (start_count != count) { // only a problem if trace isn't stopped before dumping
        printf("(NB: New entries were traced while dumping, so trace dump may have duplicate entries.)\n");
    }
    if (has_overflowed) {
        printf("(NB: Buffer has overflowed, so trace data is incomplete.)\n");
    }
}

/* Add a new allocation to the heap trace records */
static IRAM_ATTR void record_allocation(const heap_trace_record_t *record)
{
    if (!tracing || record->address == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);
    if (tracing) {
        if (count == total_records) {
            has_overflowed = true;
            /* Move the whole buffer back one slot.

               This is a bit slow, compared to treating this buffer as a ringbuffer and rotating a head pointer.

               However, ringbuffer code gets tricky when we remove elements in mid-buffer (for leak trace mode) while
               trying to keep track of an item count that may overflow.
            */
            memmove(&buffer[0], &buffer[1], sizeof(heap_trace_record_t) * (total_records -1));
            count--;
        }
        // Copy new record into place
        memcpy(&buffer[count], record, sizeof(heap_trace_record_t));
        count++;
        total_allocations++;
    }
    portEXIT_CRITICAL(&trace_mux);
}

// remove a record, used when freeing
static void remove_record(int index);

/* record a free event in the heap trace log

   For HEAP_TRACE_ALL, this means filling in the freed_by pointer.
   For HEAP_TRACE_LEAKS, this means removing the record from the log.
*/
static IRAM_ATTR void record_free(void *p, void **callers)
{
    if (!tracing || p == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);
    if (tracing && count > 0) {
        total_frees++;
        /* search backwards for the allocation record matching this free */
        int i;
        for (i = count - 1; i >= 0; i--) {
            if (buffer[i].address == p) {
                break;
            }
        }

        if (i >= 0) {
            if (mode == HEAP_TRACE_ALL) {
                memcpy(buffer[i].freed_by, callers, sizeof(void *) * STACK_DEPTH);
            } else { // HEAP_TRACE_LEAKS
                // Leak trace mode, once an allocation is freed we remove it from the list
                remove_record(i);
            }
        }
    }
    portEXIT_CRITICAL(&trace_mux);
}

/* remove the entry at 'index' from the ringbuffer of saved records */
static IRAM_ATTR void remove_record(int index)
{
    if (index < count - 1) {
        // Remove the buffer entry from the list
        memmove(&buffer[index], &buffer[index+1],
                sizeof(heap_trace_record_t) * (total_records - index - 1));
    } else {
        // For last element, just zero it out to avoid ambiguity
        memset(&buffer[index], 0, sizeof(heap_trace_record_t));
    }
    count--;
}

#include "heap_trace.inc"

#endif /*CONFIG_HEAP_TRACING_STANDALONE*/
