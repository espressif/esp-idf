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
#include "esp_memory_utils.h"
#include "sys/queue.h"


#define STACK_DEPTH CONFIG_HEAP_TRACING_STACK_DEPTH

#if CONFIG_HEAP_TRACING_STANDALONE

static portMUX_TYPE trace_mux = portMUX_INITIALIZER_UNLOCKED;
static bool tracing;
static heap_trace_mode_t mode;

/* Define struct: linked list of records */
TAILQ_HEAD(heap_trace_record_list_struct_t, heap_trace_record_t);
typedef struct heap_trace_record_list_struct_t heap_trace_record_list_t;

/* Linked List of Records */
typedef struct {

    /* Buffer used for records. */
    heap_trace_record_t *buffer;

    /* Linked list of recorded allocations */
    heap_trace_record_list_t list;

    /* Linked list of available record objects */
    heap_trace_record_list_t unused;

    /* capacity of 'buffer' */
    size_t capacity;

    /* Count of entries in 'list' */
    size_t count;

    /* During execution, we remember the maximum
       value of 'count'. This can help you
       choose the right size for your buffer capacity.*/
    size_t high_water_mark;

    /* Has the buffer overflowed and lost trace entries? */
    bool has_overflowed;
} records_t;

// Forward Defines
static void heap_trace_dump_base(bool internal_ram, bool psram);
static void record_deep_copy(heap_trace_record_t *rDest, const heap_trace_record_t* r_src);
static void list_setup(void);
static void list_remove(heap_trace_record_t* r_remove);
static bool list_add(const heap_trace_record_t* r_append);
static heap_trace_record_t* list_pop_unused(void);
static heap_trace_record_t* list_find_address_reverse(void* p);

/* The actual records. */
static records_t records;

/* Actual number of allocations logged */
static size_t total_allocations;

/* Actual number of frees logged */
static size_t total_frees;

/* Used to speed up heap_trace_get */
static heap_trace_record_t* r_get;
static size_t r_get_idx;

esp_err_t heap_trace_init_standalone(heap_trace_record_t *record_buffer, size_t num_records)
{
    if (tracing) {
        return ESP_ERR_INVALID_STATE;
    }

    if (record_buffer == NULL || num_records == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    records.buffer = record_buffer;
    records.capacity = num_records;

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

    // clear buffer
    memset(records.buffer, 0, sizeof(heap_trace_record_t) * records.capacity);

    records.count = 0;
    records.has_overflowed = false;
    list_setup();

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

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *r_out)
{
    if (r_out == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t result = ESP_OK;

    portENTER_CRITICAL(&trace_mux);

    if (index >= records.count) {

        result = ESP_ERR_INVALID_ARG; /* out of range for 'count' */

    } else {

        // Perf: speed up sequential access
        if (r_get && r_get_idx == index - 1) {

            r_get = TAILQ_NEXT(r_get, tailq);
            r_get_idx = index;

        } else {

            // Iterate through through the linked list

            r_get = TAILQ_FIRST(&records.list);

            for (int i = 0; i < index; i++) {

                if (r_get == NULL) {
                    break;
                }

                r_get = TAILQ_NEXT(r_get, tailq);
                r_get_idx = i + 1;
            }
        }

        // copy to destination
        if (r_get) {
            memcpy(r_out, r_get, sizeof(heap_trace_record_t));
        } else {
            // this should not happen since we already
            // checked that index < records.count,
            // but could be indicative of memory corruption
            result = ESP_ERR_INVALID_STATE;
            memset(r_out, 0, sizeof(heap_trace_record_t));
        }
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

void heap_trace_dump(void) {
    heap_trace_dump_caps(MALLOC_CAP_INTERNAL | MALLOC_CAP_SPIRAM);
}

void heap_trace_dump_caps(const uint32_t caps) {
    heap_trace_dump_base(caps & MALLOC_CAP_INTERNAL, caps & MALLOC_CAP_SPIRAM);
}

static void heap_trace_dump_base(bool internal_ram, bool psram)
{
    portENTER_CRITICAL(&trace_mux);

    size_t delta_size = 0;
    size_t delta_allocs = 0;
    size_t start_count = records.count;

    esp_rom_printf("====== Heap Trace: %u records (%u capacity) ======\n",
        records.count, records.capacity);

    // Iterate through through the linked list

    heap_trace_record_t *rCur = TAILQ_FIRST(&records.list);

    for (int i = 0; i < records.count; i++) {

        // check corruption
        if (rCur == NULL) {
            esp_rom_printf("\nError: heap trace linked list is corrupt. expected more records.\n");
            break;
        }

        bool should_print = rCur->address != NULL &&
            ((psram && internal_ram) ||
             (internal_ram && esp_ptr_internal(rCur->address)) ||
             (psram && esp_ptr_external_ram(rCur->address)));

        if (should_print) {

            const char* label = "";
            if (esp_ptr_internal(rCur->address)) {
                label = ", Internal";
            }
            if (esp_ptr_external_ram(rCur->address)) {
                label = ",    PSRAM";
            }

            esp_rom_printf("%6d bytes (@ %p%s) allocated CPU %d ccount 0x%08x caller ",
                   rCur->size, rCur->address, label, rCur->ccount & 1, rCur->ccount & ~3);

            for (int j = 0; j < STACK_DEPTH && rCur->alloced_by[j] != 0; j++) {
                esp_rom_printf("%p%s", rCur->alloced_by[j],
                       (j < STACK_DEPTH - 1) ? ":" : "");
            }

            if (mode != HEAP_TRACE_ALL || STACK_DEPTH == 0 || rCur->freed_by[0] == NULL) {
                delta_size += rCur->size;
                delta_allocs++;
                esp_rom_printf("\n");
            } else {
                esp_rom_printf("\nfreed by ");
                for (int j = 0; j < STACK_DEPTH; j++) {
                    esp_rom_printf("%p%s", rCur->freed_by[j],
                           (j < STACK_DEPTH - 1) ? ":" : "\n");
                }
            }
        }

        rCur = TAILQ_NEXT(rCur, tailq);
    }

    esp_rom_printf("====== Heap Trace Summary ======\n");

    if (mode == HEAP_TRACE_ALL) {
        esp_rom_printf("Mode: Heap Trace All\n");
        esp_rom_printf("%u bytes alive in trace (%u/%u allocations)\n",
               delta_size, delta_allocs, heap_trace_get_count());
    } else {
        esp_rom_printf("Mode: Heap Trace Leaks\n");
        esp_rom_printf("%u bytes 'leaked' in trace (%u allocations)\n", delta_size, delta_allocs);
    }

    esp_rom_printf("records: %u (%u capacity, %u high water mark)\n",
        records.count, records.capacity, records.high_water_mark);

    esp_rom_printf("total allocations: %u\n", total_allocations);
    esp_rom_printf("total frees: %u\n", total_frees);

    if (start_count != records.count) { // only a problem if trace isn't stopped before dumping
        esp_rom_printf("(NB: New entries were traced while dumping, so trace dump may have duplicate entries.)\n");
    }
    if (records.has_overflowed) {
        esp_rom_printf("(NB: Internal Buffer has overflowed, so trace data is incomplete.)\n");
    }
    esp_rom_printf("================================\n");

    portEXIT_CRITICAL(&trace_mux);
}

/* Add a new allocation to the heap trace records */
static IRAM_ATTR void record_allocation(const heap_trace_record_t *rAllocation)
{
    if (!tracing || rAllocation->address == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    if (tracing) {

        // If buffer is full, pop off the oldest
        // record to make more space
        if (records.count == records.capacity) {

            records.has_overflowed = true;

            heap_trace_record_t *rFirst = TAILQ_FIRST(&records.list);

            list_remove(rFirst);
        }

        // push onto end of list
        list_add(rAllocation);

        total_allocations++;
    }

    portEXIT_CRITICAL(&trace_mux);
}

/* record a free event in the heap trace log

   For HEAP_TRACE_ALL, this means filling in the freed_by pointer.
   For HEAP_TRACE_LEAKS, this means removing the record from the log.

   callers is an array of  STACK_DEPTH function pointer from the call stack
   leading to the call of record_free.
*/
static IRAM_ATTR void record_free(void *p, void **callers)
{
    if (!tracing || p == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    if (tracing) {

        total_frees++;

        // search backwards for the allocation record matching this free
        heap_trace_record_t* rFound = list_find_address_reverse(p);

        if (rFound) {
            if (mode == HEAP_TRACE_ALL) {

                // add 'freed_by' info to the record
                memcpy(rFound->freed_by, callers, sizeof(void *) * STACK_DEPTH);

            } else { // HEAP_TRACE_LEAKS

                // Leak trace mode, once an allocation is freed
                // we remove it from the list
                list_remove(rFound);
            }
        }
    }

    portEXIT_CRITICAL(&trace_mux);
}

// connect all records into a linked list of 'unused' records
static void list_setup(void)
{
    TAILQ_INIT(&records.list);
    TAILQ_INIT(&records.unused);

    for (int i = 0; i < records.capacity; i++) {

        heap_trace_record_t* rCur = &records.buffer[i];

        TAILQ_INSERT_TAIL(&records.unused, rCur, tailq);
    }
}

/* 1. removes record r_remove from records.list,
   2. places it into records.unused */
static IRAM_ATTR void list_remove(heap_trace_record_t* r_remove)
{
    assert(records.count > 0);

    // remove from records.list
    TAILQ_REMOVE(&records.list, r_remove, tailq);

    // set as unused
    r_remove->address = 0;
    r_remove->size = 0;

    // add to records.unused
    TAILQ_INSERT_HEAD(&records.unused, r_remove, tailq);

    // decrement
    records.count--;
}


// pop record from unused list
static IRAM_ATTR heap_trace_record_t* list_pop_unused(void)
{
    // no records left?
    if (records.count >= records.capacity) {
        return NULL;
    }

    // get from records.unused
    heap_trace_record_t* rUnused = TAILQ_FIRST(&records.unused);
    assert(rUnused->address == NULL);
    assert(rUnused->size == 0);

    // remove from records.unused
    TAILQ_REMOVE(&records.unused, rUnused, tailq);

    return rUnused;
}

// deep copy a record.
// Note: only copies the *allocation data*, not the next & prev ptrs
static IRAM_ATTR void record_deep_copy(heap_trace_record_t *rDest, const heap_trace_record_t *r_src)
{
    rDest->ccount  = r_src->ccount;
    rDest->address = r_src->address;
    rDest->size    = r_src->size;
    memcpy(rDest->freed_by,   r_src->freed_by,   sizeof(void *) * STACK_DEPTH);
    memcpy(rDest->alloced_by, r_src->alloced_by, sizeof(void *) * STACK_DEPTH);
}

// Append a record to records.list
// Note: This deep copies r_append
static IRAM_ATTR bool list_add(const heap_trace_record_t *r_append)
{
    if (records.count < records.capacity) {

        // get unused record
        heap_trace_record_t* rDest = list_pop_unused();

        // we checked that there is capacity, so this
        // should never be null.
        assert(rDest != NULL);

        // copy allocation data
        record_deep_copy(rDest, r_append);

        // append to records.list
        TAILQ_INSERT_TAIL(&records.list, rDest, tailq);

        // increment
        records.count++;

        // high water mark
        if (records.count > records.high_water_mark) {
            records.high_water_mark = records.count;
        }

        return true;

    } else {
        records.has_overflowed = true;
        return false;
    }
}

// search records.list backwards for the allocation record matching this address
static IRAM_ATTR heap_trace_record_t* list_find_address_reverse(void* p)
{
    if (records.count == 0) {
        return NULL;
    }

    heap_trace_record_t* rFound = NULL;

    // Perf: We search backwards because new allocations are appended
    // to the end of the list and most allocations are short lived.
    heap_trace_record_t *rCur = NULL;
    TAILQ_FOREACH_REVERSE(rCur, &records.list, heap_trace_record_list_struct_t, tailq) {
        if (rCur->address == p) {
            rFound = rCur;
            break;
        }
    }

    return rFound;
}

#include "heap_trace.inc"

#endif // CONFIG_HEAP_TRACING_STANDALONE
