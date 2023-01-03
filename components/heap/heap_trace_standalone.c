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

    /* Buffer used for records. Linked list.*/
    heap_trace_record_t *buffer;

    /* The first record in the Linked list.*/
    heap_trace_record_t* first;

    /* The last valid record in the Linked list, 
       i.e. a record that has actual data when count > 0*/
    heap_trace_record_t* last;

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
static void heap_trace_dump_base(bool internal_ram, bool psram);
static void linked_list_setup(records_t* r);
static void linked_list_move(heap_trace_record_t* toInsert, heap_trace_record_t* after);
static void linked_list_remove(records_t* r, heap_trace_record_t* rec);
static void linked_list_copy(heap_trace_record_t *dest, const heap_trace_record_t* src);
static bool linked_list_append_copy(records_t* r, const heap_trace_record_t* toAppend);
static heap_trace_record_t* linked_list_next_available(const records_t* r);
static heap_trace_record_t* linked_list_find_address_reverse(const records_t* r, void* p);

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

        // Iterate through through the linked list

        heap_trace_record_t* rec = r->first;

        for (int i = 0; i < index; i++) {
            if (rec->next) {
                rec = rec->next;
            } else {
                // this should not happen, since we already
                // checked that index < r->count 
                result = ESP_ERR_INVALID_STATE;
                break;
            }
        }

        // copy to destination
        memcpy(record, rec, sizeof(heap_trace_record_t));
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
    // In order to keep the linked list valid,
    // we must stop tracing
    bool was_tracing = false;
    if (tracing) {
        printf("Heap Tracing: temporarily disabled\n");
        was_tracing = true;
        tracing = false;
    }

    portENTER_CRITICAL(&trace_mux);

    size_t delta_size = 0;
    size_t delta_allocs = 0;
    size_t start_count = records.count;

    printf("====== Heap Trace: %u records (%u capacity) ======\n",
        records.count, records.capacity);

    for (int i = 0; i < records.count; i++) {

        heap_trace_record_t *rec = r->first;

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

        // next record
        if (rec->next) {
            rec = rec->next;
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

    if (was_tracing) {
        printf("Heap Tracing: re-enabled\n");
        tracing = true;
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

        // If buffer is full, pop off the oldest 
        // record to make more space
        if (records.count == records.capacity) {

            records.has_overflowed = true;

            linked_list_remove(r, r->first);
        }

        // push onto end of list
        linked_list_append_copy(r, record);

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
        heap_trace_record_t* found = linked_list_find_address_reverse(r,p);

        if (found) {
            if (mode == HEAP_TRACE_ALL) {

                // add 'freed_by' info to the record
                memcpy(found->freed_by, callers, sizeof(void *) * STACK_DEPTH);

            } else { // HEAP_TRACE_LEAKS

                // Leak trace mode, once an allocation is freed
                // we remove it from the list
                linked_list_remove(r, found);
            }
        }
    }

    portEXIT_CRITICAL(&trace_mux);
}

// connect all records into a linked list
static void linked_list_setup(records_t* r)
{
    r->first = &r->buffer[0];
    r->last = &r->buffer[0];

    for (int i = 0; i < r->capacity; i++) {

        heap_trace_record_t* rec = &r->buffer[i];

        memset(rec, 0, sizeof(heap_trace_record_t));

        if (i != r->capacity - 1) {
            rec->next = rec + 1;
        }

        if (i != 0) {
            rec->prev = rec - 1;
        }
    }
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

/* remove the record from the linked list */
static IRAM_ATTR void linked_list_remove(records_t* r, heap_trace_record_t* rec)
{
    // set as available
    rec->address = 0;
    rec->size = 0;

    if (r->count > 1) {

        heap_trace_record_t* prev = rec->prev;
        heap_trace_record_t* next = rec->next;

        if (prev){
            assert(prev->next == rec);
        }

        if (next){
            assert(next->prev == rec);
        }

        // update prev neighbor
        if (prev) {
            prev->next = next;
        }

        // update next neighbor
        if (next) {
            next->prev = prev;
        }

        // update first
        if (rec == r->first) {
            r->first = next;
        }

        // update last
        if (rec == r->last) {
            r->last = prev;
        }

        // move after last
        linked_list_move(rec, r->last);

        r->count--;

        if (r->count == 1){
            assert(r->last == r->first);
        }

    } else if (r->count == 1) {
        r->count--;
    }
}

// insert a record after the given record
static IRAM_ATTR void linked_list_move(heap_trace_record_t* toInsert, heap_trace_record_t* after)
{
    after->next->prev = toInsert;
    toInsert->prev = after;
    toInsert->next = after->next;
    after->next = toInsert;
}

static IRAM_ATTR heap_trace_record_t* linked_list_next_available(const records_t* r)
{
    if (r->count == r->capacity){
        return NULL;
    }
    if (r->count == 0){
        assert(r->last == r->first);
        return r->last;
    } else {
        return r->last->next;
    }
}

// only copies the *allocation data*, not the next & prev ptrs
static IRAM_ATTR void linked_list_copy(heap_trace_record_t *dest, const heap_trace_record_t *src)
{
    dest->ccount = src->ccount;
    dest->address = src->address;
    dest->size = src->size;
    memcpy(dest->freed_by, src->freed_by, sizeof(void *) * STACK_DEPTH);
    memcpy(dest->alloced_by, src->alloced_by, sizeof(void *) * STACK_DEPTH);
}

// Append a record to the end of the linked list.
// This implicitly creates a copy.
static IRAM_ATTR bool linked_list_append_copy(records_t* r, const heap_trace_record_t *toAppend)
{
    if (r->count < r->capacity) {

        // copy record into the next available slot
        heap_trace_record_t* nextA = linked_list_next_available(r);
        linked_list_copy(nextA, toAppend);

        r->last = nextA;
        r->count++;

        // high water mark
        if (r->count > r->high_water_mark) {
            r->high_water_mark = r->count;
        }

        return true;

    } else {
        r->has_overflowed = true;
        return false;
    }
}

// search backwards for the allocation record matching this address 
static IRAM_ATTR heap_trace_record_t* linked_list_find_address_reverse(const records_t* r, void* p)
{
    if (r->count == 0) {
        return NULL;
    }

    heap_trace_record_t* found = NULL;

    heap_trace_record_t* cur = r->last;
    for (size_t i = 0; i < r->count; i++) {

        if (cur->address == p) {
            found = cur;
            break;
        }

        cur = cur->prev;
    }

    return found;
}

#include "heap_trace.inc"

#endif // CONFIG_HEAP_TRACING_STANDALONE
