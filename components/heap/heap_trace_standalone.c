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


#define STACK_DEPTH CONFIG_HEAP_TRACING_STACK_DEPTH

#if CONFIG_HEAP_TRACING_STANDALONE

static portMUX_TYPE trace_mux = portMUX_INITIALIZER_UNLOCKED;
static bool tracing;
static heap_trace_mode_t mode;

/* Linked List of Records */
typedef struct {

    /* Buffer used for records. */
    heap_trace_record_t *buffer;

    /* The first valid record in the Linked list. May be NULL. */
    heap_trace_record_t* first;

    /* The last valid record in the Linked list. May be NULL. */
    heap_trace_record_t* last;

    /* Records that are not yet storing any allocation data. May be NULL. */
    heap_trace_record_t* unused;

    /* capacity of the buffer */
    size_t capacity;

    /* Count of entries logged in the buffer. */
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
static void linked_list_setup(records_t* rs);
static void linked_list_remove(records_t* rs, heap_trace_record_t* rRemove);
static void linked_list_copy(heap_trace_record_t *rDest, const heap_trace_record_t* rSrc);
static bool linked_list_append_copy(records_t* rs, const heap_trace_record_t* rAppend);
static heap_trace_record_t* linked_list_pop_unused(const records_t* rs);
static heap_trace_record_t* linked_list_find_address_reverse(const records_t* rs, void* p);

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
    linked_list_setup(&records);

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

esp_err_t heap_trace_get(size_t index, heap_trace_record_t *rOut)
{
    if (record == NULL) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t result = ESP_OK;

    portENTER_CRITICAL(&trace_mux);

    if (index >= records.count) {
        
        result = ESP_ERR_INVALID_ARG; /* out of range for 'count' */

    } else {

        // Iterate through through the linked list

        heap_trace_record_t* rCur = records.first;

        for (int i = 0; i < index; i++) {
            if (rCur->next) {
                rCur = rCur->next;
            } else {
                // this should not happen, since we already
                // checked that false == (index >= records.count)
                result = ESP_ERR_INVALID_STATE;
                break;
            }
        }

        // copy to destination
        memcpy(rOut, rCur, sizeof(heap_trace_record_t));
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

    printf("====== Heap Trace: %u records (%u capacity) ======\n",
        records.count, records.capacity);

    heap_trace_record_t *rCur = records.first;

    for (int i = 0; i < records.count; i++) {

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

            printf("%6d bytes (@ %p%s) allocated CPU %d ccount 0x%08x caller ",
                   rCur->size, rCur->address, label, rCur->ccount & 1, rCur->ccount & ~3);

            for (int j = 0; j < STACK_DEPTH && rCur->alloced_by[j] != 0; j++) {
                printf("%p%s", rCur->alloced_by[j],
                       (j < STACK_DEPTH - 1) ? ":" : "");
            }

            if (mode != HEAP_TRACE_ALL || STACK_DEPTH == 0 || rCur->freed_by[0] == NULL) {
                delta_size += rCur->size;
                delta_allocs++;
                printf("\n");
            } else {
                printf("\nfreed by ");
                for (int j = 0; j < STACK_DEPTH; j++) {
                    printf("%p%s", rCur->freed_by[j],
                           (j < STACK_DEPTH - 1) ? ":" : "\n");
                }
            }
        }

        // next record
        if (rCur->next) {
            rCur = rCur->next;
        } else {
            printf("\nError: heap trace linked list is corrupt. expected more records.\n");
            break;
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

            linked_list_remove(&records, records.first);
        }

        // push onto end of list
        linked_list_append_copy(&records, rAllocation);

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

        // search backwards for the allocation record matching this free 
        heap_trace_record_t* rFound = linked_list_find_address_reverse(&records, p);

        if (rFound) {
            if (mode == HEAP_TRACE_ALL) {

                // add 'freed_by' info to the record
                memcpy(rFound->freed_by, callers, sizeof(void *) * STACK_DEPTH);

            } else { // HEAP_TRACE_LEAKS

                // Leak trace mode, once an allocation is freed
                // we remove it from the list
                linked_list_remove(&records, rFound);
            }
        }
    }

    portEXIT_CRITICAL(&trace_mux);
}

// connect all records into a linked list of 'unused' records
static void linked_list_setup(records_t* rs)
{
    rs->first = NULL;
    rs->last = NULL;
    rs->unused = &rs->buffer[0];

    for (int i = 0; i < rs->capacity; i++) {

        heap_trace_record_t* rCur = &rs->buffer[i];

        memset(rCur, 0, sizeof(heap_trace_record_t));

        if (i != rs->capacity - 1) {
            rCur->next = rCur + 1;
        }

        if (i != 0) {
            rCur->prev = rCur - 1;
        }
    }
}

/* remove a record from the first/last linked list,
   and place it into the 'unused' linked list */
static IRAM_ATTR void linked_list_remove(records_t* rs, heap_trace_record_t* rRemove)
{
    assert(rs->count > 0);
    assert(rs->last != NULL);
    assert(rs->first != NULL);

    // set as available
    rRemove->address = 0;
    rRemove->size = 0;

    heap_trace_record_t* rPrev = rRemove->prev;
    heap_trace_record_t* rNext = rRemove->next;

    if (rPrev) {
        assert(rPrev->next == rRemove);
    }

    if (rNext) {
        assert(rNext->prev == rRemove);
    }

    // update prev neighbor
    if (rPrev) {
        rPrev->next = rNext;
    }

    // update next neighbor
    if (rNext) {
        rNext->prev = rPrev;
    }

    // update first
    if (rRemove == rs->first) {
        rs->first = rNext;
    }

    // update last
    if (rRemove == rs->last) {
        rs->last = rPrev;
    }

    // move self to unused
    rRemove->next = rs->unused;
    rRemove->prev = NULL;
    rs->unused = rRemove;

    // decrement
    rs->count--;

    if (rs->count > 1) {
        assert(rs->last != NULL);
        assert(rs->first != NULL);
        assert(rs->last != rs->first);
    }

    if (rs->count == 1) {
        assert(rs->last != NULL);
        assert(rs->first != NULL);
        assert(rs->last == rs->first);
    }
    
    if (rs->count == 0) {
        assert(rs->last == NULL);
        assert(rs->first == NULL);
    }
}


// pop record from unused list
static IRAM_ATTR heap_trace_record_t* linked_list_pop_unused(const records_t* rs)
{
    if (rs->count >= rs->capacity) {
        return NULL;
    }

    // we checked that there is capacity, 
    // so there should be some unused records
    assert(rs->unused != NULL);

    heap_trace_record_t* pop = rs->unused;
    assert(pop->address == NULL);
    assert(pop->size == NULL);

    // update next unused record
    heap_trace_record_t* next = pop->next;
    if (next) {
        next->prev = NULL;
    }

    // update unused list
    rs->unused = next;

    // assert we popped the last unused record
    if (rs->count == rs->capacity - 1) {
        assert(next == NULL)
    }

    return pop;
}

// copy a record.
// Note: only copies the *allocation data*, not the next & prev ptrs
static IRAM_ATTR void linked_list_copy(heap_trace_record_t *rDest, const heap_trace_record_t *rSrc)
{
    rDest->ccount = rSrc->ccount;
    rDest->address = rSrc->address;
    rDest->size = rSrc->size;
    memcpy(rDest->freed_by, rSrc->freed_by, sizeof(void *) * STACK_DEPTH);
    memcpy(rDest->alloced_by, rSrc->alloced_by, sizeof(void *) * STACK_DEPTH);
}

// Append a record to the end of the linked list.
// This deep copies rAppend into the linked list. 
static IRAM_ATTR bool linked_list_append_copy(records_t* rs, const heap_trace_record_t *rAppend)
{
    if (rs->count < rs->capacity) {

        // get unused record
        heap_trace_record_t* rDest = linked_list_pop_unused(rs);

        // we checked that there is capacity, so this
        // should never be null.
        assert(rDest != NULL);
        
        // copy allocation data
        linked_list_copy(rDest, rAppend);

        // update linked list connectivity
        rDest->next = NULL;
        rDest->prev = rs->last;

        // update last
        rs->last = rDest;

        // increment
        rs->count++;

        // high water mark
        if (rs->count > rs->high_water_mark) {
            rs->high_water_mark = rs->count;
        }

        return true;

    } else {
        rs->has_overflowed = true;
        return false;
    }
}

// search backwards for the allocation record matching this address 
static IRAM_ATTR heap_trace_record_t* linked_list_find_address_reverse(const records_t* rs, void* p)
{
    if (rs->count == 0) {
        return NULL;
    }

    heap_trace_record_t* rFound = NULL;

    heap_trace_record_t* rCur = rs->last;
    for (size_t i = 0; i < rs->count; i++) {

        if (rCur->address == p) {
            rFound = rCur;
            break;
        }

        rCur = rCur->prev;
    }

    return rFound;
}

#include "heap_trace.inc"

#endif // CONFIG_HEAP_TRACING_STANDALONE