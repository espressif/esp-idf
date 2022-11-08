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

/* Linked List of Records */
struct records_t {

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

    /* During execution, we remember the maximuim 
       value of count. This can help you choose
       the right size for your buffer capacity.*/
    size_t high_water_mark;

    /* Has the buffer overflowed and lost trace entries? */
    bool has_overflowed;
};

typedef struct records_t records_t;

// Forward Defines
static void drain_internal_cache();
static void heap_trace_dump_base(bool internalRam, bool spiRam);
static void linked_list_setup(records_t* r);
static void linked_list_move(heap_trace_record_t* toInsert, heap_trace_record_t* after);
static void linked_list_remove(records_t* r, heap_trace_record_t* rec);
static void linked_list_copy(heap_trace_record_t *dest, const heap_trace_record_t* src);
static bool linked_list_append_copy(records_t* r, const heap_trace_record_t* toAppend);
static heap_trace_record_t* linked_list_next_available(const records_t* r);
static heap_trace_record_t* linked_list_find_address_reverse(const records_t* r, void* p);

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

    spi_records.buffer = spi_record_buffer;
    spi_records.capacity = spi_num_records;

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
    linked_list_setup(&internal_records);

    spi_records.count = 0;
    spi_records.has_overflowed = false;
    linked_list_setup(&spi_records);

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

    // In order to keep the linked list valid,
    // we must stop tracing
    bool was_tracing = false;
    if (tracing) {
        printf("Heap Tracing: temporarily disabled\n");
        was_tracing = true;
        tracing = false;
    }

    portENTER_CRITICAL(&trace_mux);

    records_t* r = spi_records.capacity ? 
        &spi_records : &internal_records;

    size_t delta_size = 0;
    size_t delta_allocs = 0;
    size_t start_count = r->count;

    printf("====== Heap Trace: %u records (%u capacity) ======\n",
        r->count, r->capacity);

    for (int i = 0; i < r->count; i++) {

        heap_trace_record_t *rec = r->first;

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

        if (rec->next) {
            rec = rec->next;
        } else {
            printf("\nError: heap trace dump linked list is corrupt\n");
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

    portEXIT_CRITICAL(&trace_mux);

    if (was_tracing) {
        printf("Heap Tracing: re-enabled\n");;
        tracing = true;
    }
}

// Move from internal_records to spi_records
static void drain_internal_cache()
{
    if(spi_records.capacity == 0 || spi_records.buffer == NULL) {
        return;
    }

    portENTER_CRITICAL(&trace_mux);

    heap_trace_record_t* rec = internal_records.first;

    // process in chronological order
    for (int i = 0; i < internal_records.count; i++) {

        if (rec->alloced_by[0]) {

            // move the internal record into the SPI cache
            linked_list_append_copy(&spi_records, rec);
    
        } else if (rec->freed_by[0]) {

            // search backwards for the allocation record matching this free 
            heap_trace_record_t* found = linked_list_find_address_reverse(
                &spi_records, rec->address);

            if (found) {

                if (mode == HEAP_TRACE_ALL) {

                    // copy 'freed_by' info into found
                    memcpy(found->freed_by, rec->freed_by, sizeof(void *) * STACK_DEPTH);

                } else { // HEAP_TRACE_LEAKS

                    // Leak trace mode, once an allocation is 
                    // freed we remove it from the list
                    linked_list_remove(&spi_records, found);
                }
            }
        }

        // go to next record
        rec = rec->next;
    }

    internal_records.count = 0;
    internal_records.last = internal_records.first;

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

        // If buffer is full, pop off the oldest record 
        // to make more space
        if (r->count == r->capacity) {
            r->has_overflowed = true;
            linked_list_remove(r, r->first);
        }

        // push onto end of list
        linked_list_append_copy(r, record);

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

        // search backwards for the allocation record matching this free 
        heap_trace_record_t* found = linked_list_find_address_reverse(r,p);

        if (found) {
            if (mode == HEAP_TRACE_ALL) {

                // just copy the freed_by info into found
                memcpy(found->freed_by, callers, sizeof(void *) * STACK_DEPTH);

            } else { // HEAP_TRACE_LEAKS

                // Leak trace mode, once an allocation is freed we remove it from the list
                linked_list_remove(r, found);
            }

        } else if (spi_records.capacity > 0) {

            // Temporily store the 'freed_by' info in the internal cache.
            // We'll search the SPI records later.
            heap_trace_record_t freed = {.address = p};
            memcpy(freed.freed_by, callers, sizeof(void *) * STACK_DEPTH);
            linked_list_append_copy(r, &freed);
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