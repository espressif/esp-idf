/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
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

typedef struct {

    /* Buffer used for records, starting at offset 0 */
    heap_trace_record_t *buffer;

    /* capacity of the buffer */
    size_t capacity;

    /* Count of entries logged in the buffer.*/
    size_t count;

    /* During execution, we remember the maximum
       value of 'count'. This can help you
       choose the right size for your buffer capacity.*/
    size_t high_water_mark;

    /* Has the buffer overflowed and lost trace entries? */
    bool has_overflowed;
} records_t;


// Forward Defines
static void remove_record(records_t *r, int index);
static void heap_trace_dump_base(bool internal_ram, bool psram);

/* The actual records. */
static records_t records;

/* Actual number of allocations logged */
static size_t total_allocations;

/* Actual number of frees logged */
static size_t total_frees;

esp_err_t heap_trace_init_standalone(heap_trace_record_t *record_buffer, size_t num_records)
{
    if (tracing) {
        return ESP_ERR_INVALID_STATE;
    }

    records.buffer = record_buffer;
    records.capacity = num_records;
    memset(records.buffer, 0, num_records * sizeof(heap_trace_record_t));

    return ESP_OK;
}

esp_err_t heap_trace_start(heap_trace_mode_t mode_param)
{
    if (records.buffer == NULL || records.capacity == 0) {
        return ESP_ERR_INVALID_STATE;
    }


    portENTER_CRITICAL(&trace_mux);

    tracing = false;
    mode = mode_param;

    records.count = 0;
    records.has_overflowed = false;

    total_allocations = 0;
    total_frees = 0;
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
    return records.count;
}

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *record)
{
    if (record == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t result = ESP_OK;

    portENTER_CRITICAL(&trace_mux);

    if (index >= records.count) {
        result = ESP_ERR_INVALID_ARG; /* out of range for 'count' */
    } else {
        memcpy(record, &records.buffer[index], sizeof(heap_trace_record_t));
    }

    portEXIT_CRITICAL(&trace_mux);
    return result;
}

esp_err_t heap_trace_summary(heap_trace_summary_t *summary)
{
    if (summary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&trace_mux);
    summary->mode = mode;
    summary->total_allocations = total_allocations;
    summary->total_frees = total_frees;
    summary->count = records.count;
    summary->capacity = records.capacity;
    summary->high_water_mark = records.high_water_mark;
    summary->has_overflowed = records.has_overflowed;
    portEXIT_CRITICAL(&trace_mux);

    return ESP_OK;
}

void heap_trace_dump_internal_ram(void) {
    heap_trace_dump_base(true, false);
}

void heap_trace_dump_psram(void) {
    heap_trace_dump_base(false, true);
}

void heap_trace_dump(void) {
    heap_trace_dump_base(true, true);
}

static void heap_trace_dump_base(bool internal_ram, bool psram)
{
    size_t delta_size = 0;
    size_t delta_allocs = 0;
    size_t start_count = records.count;

    printf("====== Heap Trace: %u records (%u capacity) ======\n",
        records.count, records.capacity);

    for (int i = 0; i < records.count; i++) {

        heap_trace_record_t *rec = &records.buffer[i];

        bool should_print = rec->address != NULL &&
            ((psram && internal_ram) ||
             (internal_ram && esp_ptr_internal(rec->address)) ||
             (psram && esp_ptr_external_ram(rec->address)));

        if (should_print) {

            const char* label = "";
            if (esp_ptr_internal(rec->address)) {
                label = ", Internal";
            }
            if (esp_ptr_external_ram(rec->address)) {
                label = ",    PSRAM";
            }

            printf("%6d bytes (@ %p%s) allocated CPU %d ccount 0x%08x caller ",
                   rec->size, rec->address, label, rec->ccount & 1, rec->ccount & ~3);

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

    printf("====== Heap Trace Summary ======\n");

    if (mode == HEAP_TRACE_ALL) {
        printf("Mode: Heap Trace All\n");
        printf("%u bytes alive in trace (%u/%u allocations)\n",
               delta_size, delta_allocs, heap_trace_get_count());
    } else {
        printf("Mode: Heap Trace Leaks\n");
        printf("%u bytes 'leaked' in trace (%u allocations)\n", delta_size, delta_allocs);
    }

    printf("records: %u (%u capacity, %u high water mark)\n",
        records.count, records.capacity, records.high_water_mark);

    printf("total allocations: %u\n", total_allocations);
    printf("total frees: %u\n", total_frees);

    if (start_count != records.count) { // only a problem if trace isn't stopped before dumping
        printf("(NB: New entries were traced while dumping, so trace dump may have duplicate entries.)\n");
    }
    if (records.has_overflowed) {
        printf("(NB: Internal Buffer has overflowed, so trace data is incomplete.)\n");
    }
    printf("================================\n");
}

/* Add a new allocation to the heap trace records */
static IRAM_ATTR void record_allocation(const heap_trace_record_t *record)
{
    if (!tracing || record->address == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    if (tracing) {

        if (records.count == records.capacity) {

            records.has_overflowed = true;

            /* Move the whole buffer back one slot.

            This is a bit slow, compared to treating this buffer as a
            ringbuffer and rotating a head pointer.

            However, ringbuffer code gets tricky when we remove elements
            in mid-buffer (for leak trace mode) while trying to keep
            track of an item count that may overflow.
            */
            memmove(&records.buffer[0], &records.buffer[1],
                sizeof(heap_trace_record_t) * (records.capacity -1));

            records.count--;
        }

        // Copy new record into place
        memcpy(&records.buffer[records.count], record, sizeof(heap_trace_record_t));

        records.count++;

        // high water mark
        if (records.count > records.high_water_mark) {
            records.high_water_mark = records.count;
        }

        total_allocations++;
    }

    portEXIT_CRITICAL(&trace_mux);
}

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

    if (tracing) {

        total_frees++;

        /* search backwards for the allocation record matching this free */
        int i = -1;
        if (records.count > 0) {
            for (i = records.count - 1; i >= 0; i--) {
                if (records.buffer[i].address == p) {
                    break;
                }
            }
        }

        if (i >= 0) {
            if (mode == HEAP_TRACE_ALL) {

                // add 'freed_by' info to the record
                memcpy(records.buffer[i].freed_by, callers, sizeof(void *) * STACK_DEPTH);

            } else { // HEAP_TRACE_LEAKS

                // Leak trace mode, once an allocation is freed
                // we remove it from the list
                remove_record(&records, i);
            }
        }
    }

    portEXIT_CRITICAL(&trace_mux);
}

/* remove the entry at 'index' from the ringbuffer of saved records */
static IRAM_ATTR void remove_record(records_t *r, int index)
{
    if (index < r->count - 1) {
        // Remove the buffer entry from the list
        memmove(&r->buffer[index], &r->buffer[index+1],
                sizeof(heap_trace_record_t) * (r->capacity - index - 1));
    } else {
        // For last element, just zero it out to avoid ambiguity
        memset(&r->buffer[index], 0, sizeof(heap_trace_record_t));
    }
    r->count--;
}

#include "heap_trace.inc"

#endif // CONFIG_HEAP_TRACING_STANDALONE
