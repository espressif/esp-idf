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

struct records_t {

    /* Buffer used for records, starting at offset 0 */
    heap_trace_record_t *buffer;

    /* capacity of the buffer */
    size_t capacity;

    /* Count of entries logged in the buffer.*/
    size_t count;

    /* During execution, we remember the minimum 
       value of (capacity - count). This can help you
       choose the right size for your buffer capacity.*/
    size_t high_water_mark;

    /* Has the buffer overflowed and lost trace entries? */
    bool has_overflowed;
};

typedef struct records_t records_t;

// Forward Defines
static void drain_internal_cache();
static void remove_record(records_t* r, int index);
static void heap_trace_dump_base(bool internalRam, bool spiRam);

/* The actual records. Only used as a 
   temporary cache if SPI RAM is used */
static records_t internal_records;

/* Long term storage of records */
static records_t spi_records;

/* Actual number of allocations logged */
static size_t total_allocations;

/* Actual number of frees logged */
static size_t total_frees;

esp_err_t heap_trace_init_standalone(heap_trace_record_t* record_buffer, size_t num_records)
{
    if (tracing) {
        return ESP_ERR_INVALID_STATE;
    }

    internal_records.buffer = record_buffer;
    internal_records.capacity = num_records;
    memset(internal_records.buffer, 0, num_records * sizeof(heap_trace_record_t));

    spi_records.buffer = NULL;
    spi_records.capacity = 0;

    return ESP_OK;
}


esp_err_t heap_trace_init_standalone_with_spi_ram(
    heap_trace_record_t *internal_record_buffer, size_t internal_num_records,
    heap_trace_record_t *spi_record_buffer, size_t spi_num_records)
{
    if (tracing) {
        return ESP_ERR_INVALID_STATE;
    }

    internal_records.buffer = internal_record_buffer;
    internal_records.capacity = internal_num_records;
    memset(internal_records.buffer, 0, internal_num_records * sizeof(heap_trace_record_t));

    spi_records.buffer = spi_record_buffer;
    spi_records.capacity = spi_num_records;
    memset(spi_records.buffer, 0, spi_num_records * sizeof(heap_trace_record_t));

    return ESP_OK;
}

esp_err_t heap_trace_start(heap_trace_mode_t mode_param)
{
    if (internal_records.buffer == NULL || internal_records.capacity == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    if (spi_records.buffer && spi_records.capacity == 0) {
        return ESP_ERR_INVALID_STATE;
    }

    portENTER_CRITICAL(&trace_mux);

    tracing = false;
    mode = mode_param;

    internal_records.count = 0;
    internal_records.has_overflowed = false;

    spi_records.count = 0;
    spi_records.has_overflowed = false;

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
    if(spi_records.capacity) {
        drain_internal_cache();
    }
    return set_tracing(false);
}

esp_err_t heap_trace_resume(void)
{
    if(spi_records.capacity) {
        drain_internal_cache();
    }
    return set_tracing(true);
}

size_t heap_trace_get_count(void)
{
    if(spi_records.capacity) {
        drain_internal_cache();
    }

    records_t* r = spi_records.capacity ? 
        &spi_records : &internal_records;

    return r->count;
}

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *record)
{
    if (record == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    if(spi_records.capacity) {
        drain_internal_cache();
    }

    esp_err_t result = ESP_OK;

    portENTER_CRITICAL(&trace_mux);

    records_t* r = spi_records.capacity ? 
        &spi_records : &internal_records;

    if (index >= r->count) {
        result = ESP_ERR_INVALID_ARG; /* out of range for 'count' */
    } else {
        memcpy(record, &r->buffer[index], sizeof(heap_trace_record_t));
    }

    portEXIT_CRITICAL(&trace_mux);
    return result;
}

esp_err_t heap_trace_summary(heap_trace_summary_t* summary)
{
    if (summary == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&trace_mux);
    summary->mode = mode;
    summary->total_allocations = total_allocations;
    summary->total_frees = total_frees;
    summary->internal_count = internal_records.count;
    summary->internal_capacity = internal_records.capacity;
    summary->internal_high_water_mark = internal_records.high_water_mark;
    summary->internal_overflowed = internal_records.has_overflowed;
    summary->spi_count = spi_records.count;
    summary->spi_capacity = spi_records.capacity;
    summary->spi_high_water_mark = spi_records.high_water_mark;
    summary->spi_overflowed = spi_records.has_overflowed;
    portEXIT_CRITICAL(&trace_mux);

    return ESP_OK;
}

void heap_trace_dump_internal_ram_only(void) {
    heap_trace_dump_base(true, false);
}

void heap_trace_dump_spi_ram_only(void) {
    heap_trace_dump_base(false, true);
}

void heap_trace_dump(void) {
    heap_trace_dump_base(true, true);
}

static void heap_trace_dump_base(bool internalRam, bool spiRam)
{
    if(spi_records.capacity) {
        drain_internal_cache();
    }

    records_t* r = spi_records.capacity ? 
        &spi_records : &internal_records;

    size_t delta_size = 0;
    size_t delta_allocs = 0;
    size_t start_count = r->count;

    printf("====== Heap Trace: %u records (%u capacity) ======\n",
        r->count, r->capacity);

    for (int i = 0; i < r->count; i++) {

        heap_trace_record_t *rec = &r->buffer[i];

        bool shouldPrint = rec->address != NULL &&
            ((spiRam && internalRam) ||
             (internalRam && esp_ptr_internal(rec->address)) ||
             (spiRam && esp_ptr_external_ram(rec->address)));

        if (shouldPrint) {

            const char* label = "";
            if (esp_ptr_internal(rec->address)) {
                label = ", Internal";
            }
            if (esp_ptr_external_ram(rec->address)) {
                label = ",   SPIRAM";
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

    printf("records: %u (%u capacity, %u high water mark)\n", r->count, r->capacity, r->high_water_mark);

    if (spi_records.capacity) {
        printf("internal records: (%u capacity, %u high water mark)\n", 
            internal_records.capacity, internal_records.high_water_mark);
    }

    printf("total allocations: %u\n", total_allocations);
    printf("total frees: %u\n", total_frees);

    if (start_count != r->count) { // only a problem if trace isn't stopped before dumping
        printf("(NB: New entries were traced while dumping, so trace dump may have duplicate entries.)\n");
    }
    if (internal_records.has_overflowed) {
        printf("(NB: Internal Buffer has overflowed, so trace data is incomplete.)\n");
    }
    if (spi_records.has_overflowed) {
        printf("(NB: SPI Buffer has overflowed, so trace data is incomplete.)\n");
    }
    printf("================================\n");
}

// Move from internal_records to spi_records
static void drain_internal_cache()
{
    if(spi_records.capacity == 0 || spi_records.buffer == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    // process in chronological order
    for (int i = 0; i < internal_records.count; i++) {

        heap_trace_record_t* rec = &internal_records.buffer[i];

        if (rec->alloced_by[0]) {

            if (spi_records.count < spi_records.capacity) {

                // move the record into the SPI cache
                heap_trace_record_t* spiRec = &spi_records.buffer[spi_records.count];
                memcpy(spiRec, rec, sizeof(heap_trace_record_t));

                spi_records.count++;

                // high water mark
                if (spi_records.count > spi_records.high_water_mark) {
                    spi_records.high_water_mark = spi_records.count;
                }

            } else {
                spi_records.has_overflowed = true;
            }
    
        } else if (rec->freed_by[0]) {

            // search backwards for the allocation record matching this free 
            int i = -1;
            if (spi_records.count > 0) {
                for (i = spi_records.count - 1; i >= 0; i--) {
                    if (spi_records.buffer[i].address == rec->address) {
                        break;
                    }
                }
            }

            if (i >= 0) {

                if (mode == HEAP_TRACE_ALL) {

                    // copy 'freed_by' info
                    memcpy(spi_records.buffer[i].freed_by, rec->freed_by, sizeof(void *) * STACK_DEPTH);

                } else { // HEAP_TRACE_LEAKS

                    // Leak trace mode, once an allocation is freed we remove it from the list
                    remove_record(&spi_records, i);
                }
            }
        }
    }

    internal_records.count = 0;

    portEXIT_CRITICAL(&trace_mux);
}

/* Add a new allocation to the heap trace records */
static IRAM_ATTR void record_allocation(const heap_trace_record_t *record)
{
    if (!tracing || record->address == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    if (tracing) {

        // record into internal buffer
        records_t* r = &internal_records;

        if (r->count == r->capacity) {
            r->has_overflowed = true;
            /* Move the whole buffer back one slot.

               This is a bit slow, compared to treating this buffer as a ringbuffer and rotating a head pointer.

               However, ringbuffer code gets tricky when we remove elements in mid-buffer (for leak trace mode) while
               trying to keep track of an item count that may overflow.
            */
            memmove(&r->buffer[0], &r->buffer[1], sizeof(heap_trace_record_t) * (r->capacity -1));
            r->count--;
        }

        // Copy new record into place
        memcpy(&r->buffer[r->count], record, sizeof(heap_trace_record_t));

        r->count++;

        // high water mark
        if (r->count > r->high_water_mark) {
            r->high_water_mark = r->count;
        }

        total_allocations++;
    }

    portEXIT_CRITICAL(&trace_mux);

    if (spi_records.capacity) {

        void* func = record->alloced_by[0];

        // Is the calling function in IRAM?
        bool iram = 
            esp_ptr_in_iram(func) ||
            esp_ptr_in_dram(func) || 
            esp_ptr_in_diram_dram(func) || 
            esp_ptr_in_diram_iram(func) || 
            esp_ptr_in_rtc_iram_fast(func) || 
            esp_ptr_in_rtc_dram_fast(func) || 
            esp_ptr_in_rtc_slow(func) || 
            esp_ptr_internal(func) || 
            esp_ptr_in_drom(func); 

        if (iram == false) {
            drain_internal_cache();
        }
    }
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

    // record into internal buffer
    records_t* r = &internal_records;

    if (tracing) {

        total_frees++;

        /* search backwards for the allocation record matching this free */
        int i = -1;
        if (r->count > 0) {
            for (i = r->count - 1; i >= 0; i--) {
                if (r->buffer[i].address == p) {
                    break;
                }
            }
        }

        if (i >= 0) {

            if (mode == HEAP_TRACE_ALL) {
                memcpy(r->buffer[i].freed_by, callers, sizeof(void *) * STACK_DEPTH);
            } else { // HEAP_TRACE_LEAKS
                // Leak trace mode, once an allocation is freed we remove it from the list
                remove_record(r, i);
            }

        } else if (spi_records.capacity > 0) {
            if (r->count < r->capacity) {
                // temporarily keep the 'free' in the internal cache.
                // We'll search through the SPI records later.
                heap_trace_record_t* rec = &r->buffer[r->count];
                memset(rec, 0, sizeof(heap_trace_record_t));
                memcpy(rec->freed_by, callers, sizeof(void *) * STACK_DEPTH);
                rec->address = p;
                r->count++;
            } else {
                r->has_overflowed = true;
            }
        }
    }

    portEXIT_CRITICAL(&trace_mux);

    if (spi_records.capacity) {

        void* func = callers[0];

        // Is the calling function in IRAM?
        bool iram = 
            esp_ptr_in_iram(func) ||
            esp_ptr_in_dram(func) || 
            esp_ptr_in_diram_dram(func) || 
            esp_ptr_in_diram_iram(func) || 
            esp_ptr_in_rtc_iram_fast(func) || 
            esp_ptr_in_rtc_dram_fast(func) || 
            esp_ptr_in_rtc_slow(func) || 
            esp_ptr_internal(func) || 
            esp_ptr_in_drom(func); 

        if (iram == false) {
            drain_internal_cache();
        }
    }
}

/* remove the entry at 'index' from the ringbuffer of saved records */
static IRAM_ATTR void remove_record(records_t* r, int index)
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