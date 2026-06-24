/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * KASAN heap hooks for ESP-IDF.
 *
 * Provides strong definitions of the weak heap hooks so the KASAN shadow stays
 * in sync with every heap_caps_malloc / heap_caps_free.
 *
 * When CONFIG_KASAN_HEAP_REDZONE_SIZE > 0, each allocation gets a poisoned
 * guard band on both sides (left and right redzones) for overflow/underflow
 * detection.
 *
 * When CONFIG_KASAN_QUARANTINE_SIZE > 0, freed blocks are held in a FIFO
 * before the real free, keeping their shadow poisoned to catch use-after-free.
 *
 * Compiled with -fno-sanitize=kernel-address to avoid recursive instrumentation.
 */

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_rom_sys.h"
/*
 * Avoid including esp_heap_caps.h: it declares the hook symbols as weak, and
 * GCC propagates that attribute to definitions in the same TU.  Forward-declare
 * only what we need.
 */
void heap_caps_free(void *ptr);
size_t heap_caps_get_allocated_size(void *ptr);

void kasan_heap_alloc_impl(void *ptr, size_t size, uint32_t caps);
void kasan_heap_free_impl(void *ptr);
bool kasan_heap_should_defer_free(void);
void kasan_heap_clear_deferred_free(void);

#include "esp_kasan.h"
#include "esp_private/startup_internal.h"
#include "heap_private.h"
#include "heap_kasan_layout.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#if CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS

/*
 * Initialise the KASAN shadow region before the heap allocator (priority 100).
 * Runs at priority 99 so the shadow offset is in place before the first heap
 * caps registration, and before the alloc/free hooks below start running.
 */
ESP_SYSTEM_INIT_FN(init_kasan_shadow, CORE, BIT(0), 98)
{
    kasan_init_shadow();
    return ESP_OK;
}

/* ---- Left-redzone header ------------------------------------------------ */

#define KASAN_LRZ_MAGIC  0xA5B6C7D8UL

typedef struct {
    uint32_t magic;
    size_t   user_size;
} kasan_lrz_hdr_t;

#if HEAP_KASAN_RZ_ENABLED
_Static_assert((CONFIG_KASAN_HEAP_REDZONE_SIZE % 4) == 0,
               "CONFIG_KASAN_HEAP_REDZONE_SIZE must be a multiple of 4");
_Static_assert(sizeof(kasan_lrz_hdr_t) <= KASAN_RZ,
               "CONFIG_KASAN_HEAP_REDZONE_SIZE is too small to hold the KASAN header");
#endif

/* ---- Quarantine ring-buffer --------------------------------------------- */

static inline unsigned kasan_q_core_id(void)
{
    int core_id = xPortGetCoreID();
    assert(core_id >= 0 && core_id < portNUM_PROCESSORS);
    return (unsigned)core_id;
}

#if CONFIG_KASAN_QUARANTINE_SIZE > 0

/*
 * Per-core "deferred-free ticket counter".  Each call into
 * kasan_heap_free_impl() enqueues one block in the quarantine and bumps the
 * counter; heap_caps_free() pops one ticket through kasan_heap_should_defer_free
 * + kasan_heap_clear_deferred_free.  A counter (rather than a bool) is required
 * because frees can nest: e.g. heap_caps_free(A) starts on core 0, an ISR fires
 * mid-way and runs heap_caps_free(B) on the same core, then heap_caps_free(A)
 * resumes.  With a bool the ISR's "clear" would mask the outer free's defer
 * request and the outer pointer would be released both via multi_heap_free()
 * *and* later via the quarantine eviction (double free).
 *
 * Access is from one core at a time but can be from an ISR on that core, so
 * an atomic fetch-add is sufficient; we don't need a cross-core barrier here.
 */
static DRAM_ATTR atomic_uint_fast32_t s_q_defer_free[portNUM_PROCESSORS];

#define KASAN_Q_ENTRIES  64U

typedef struct {
    void  *ptr;
    size_t size;
} kasan_q_entry_t;

static kasan_q_entry_t s_q[KASAN_Q_ENTRIES];
static unsigned        s_q_head;
static unsigned        s_q_tail;
static size_t          s_q_bytes;
static portMUX_TYPE    s_q_mux = portMUX_INITIALIZER_UNLOCKED;

static void kasan_q_release_one(void *ptr)
{
    void *raw_ptr = KASAN_USER_TO_RAW(ptr);
    void *block_owner_ptr = MULTI_HEAP_REMOVE_BLOCK_OWNER_OFFSET(raw_ptr);
    heap_t *heap = find_containing_heap(block_owner_ptr);
    assert(heap != NULL && "quarantine free target pointer is outside heap areas");
    multi_heap_free(heap->heap, block_owner_ptr);
}

static void kasan_q_add(void *ptr, size_t size)
{
    /*
     * Collect every block that needs to be evicted into a small on-stack
     * array.  We update head/tail/bytes atomically inside the critical
     * section and only call multi_heap_free() after we have exited it.  This
     * avoids the previous "drop the lock, do work, re-acquire" pattern where
     * a concurrent kasan_q_add() on the other core could mutate s_q_head /
     * s_q_tail / s_q_bytes during the lock-release window and we would
     * resume with stale state.
     */
    void *evict[KASAN_Q_ENTRIES];
    unsigned n_evict = 0;

    portENTER_CRITICAL(&s_q_mux);

    /* If the ring is full, evict the oldest entry to make room. */
    unsigned next = (s_q_head + 1U) % KASAN_Q_ENTRIES;
    if (next == s_q_tail) {
        evict[n_evict++] = s_q[s_q_tail].ptr;
        s_q_bytes -= s_q[s_q_tail].size;
        s_q_tail = (s_q_tail + 1U) % KASAN_Q_ENTRIES;
    }

    /* Insert the new entry at head. */
    s_q[s_q_head].ptr  = ptr;
    s_q[s_q_head].size = size;
    s_q_head = (s_q_head + 1U) % KASAN_Q_ENTRIES;
    s_q_bytes += size;

    /* Trim back to the configured byte budget. */
    while (s_q_bytes > (size_t)CONFIG_KASAN_QUARANTINE_SIZE
           && s_q_tail != s_q_head
           && n_evict < KASAN_Q_ENTRIES) {
        evict[n_evict++] = s_q[s_q_tail].ptr;
        s_q_bytes -= s_q[s_q_tail].size;
        s_q_tail = (s_q_tail + 1U) % KASAN_Q_ENTRIES;
    }

    portEXIT_CRITICAL(&s_q_mux);

    for (unsigned i = 0; i < n_evict; i++) {
        kasan_q_release_one(evict[i]);
    }
}

#endif /* CONFIG_KASAN_QUARANTINE_SIZE > 0 */

bool kasan_heap_should_defer_free(void)
{
#if CONFIG_KASAN_QUARANTINE_SIZE > 0
    return atomic_load_explicit(&s_q_defer_free[kasan_q_core_id()],
                                memory_order_acquire) > 0U;
#else
    return false;
#endif
}

void kasan_heap_clear_deferred_free(void)
{
#if CONFIG_KASAN_QUARANTINE_SIZE > 0
    /*
     * Consume one ticket.  Wrapping below zero is treated as a programming
     * error (call to clear() without matching enqueue from kasan_q_add).
     */
    uint_fast32_t prev = atomic_fetch_sub_explicit(&s_q_defer_free[kasan_q_core_id()],
                                                   1U, memory_order_release);
    assert(prev > 0U && "kasan_heap_clear_deferred_free: counter underflow");
    (void)prev;
#endif
}

/* ---- Heap hooks --------------------------------------------------------- */

void kasan_heap_alloc_impl(void *ptr, size_t size, uint32_t caps)
{
    (void)caps;
    if (ptr == NULL || size == 0) {
        return;
    }

    /*
     * Mark the full granule containing the tail of the user allocation as
     * unconditionally valid (instead of "first N bytes valid").  Many libc
     * memcpy / strlen / strcpy implementations on RISC-V perform word-at-a-
     * time loads and may legitimately touch the bytes between the requested
     * end and the next 4-byte boundary; treating those as a partial poison
     * would flag a false positive.  The actual right redzone still starts at
     * the next granule boundary, so genuine overflows past 4 bytes are still
     * detected.
     */
    const size_t granule_aligned_size = (size + 3U) & ~(size_t)3U;
    kasan_unpoison_region(ptr, granule_aligned_size);

#if CONFIG_KASAN_HEAP_REDZONE_SIZE > 0
    uint8_t *lrz = (uint8_t *)ptr - KASAN_RZ;
    kasan_lrz_hdr_t hdr = { .magic = KASAN_LRZ_MAGIC, .user_size = size };
    memcpy(lrz, &hdr, sizeof(hdr));
    kasan_poison_region(lrz, KASAN_RZ, KASAN_POISON_HEAP_LRZ);

    /* Start RRZ at the next granule boundary, not at user_ptr + size. */
    uint8_t *rrz = (uint8_t *)ptr + granule_aligned_size;
    kasan_poison_region(rrz, KASAN_RZ, KASAN_POISON_HEAP_RRZ);
#endif
}

void kasan_heap_free_impl(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

#if CONFIG_KASAN_HEAP_REDZONE_SIZE > 0
    kasan_lrz_hdr_t hdr;
    uint8_t *lrz = (uint8_t *)ptr - KASAN_RZ;
    memcpy(&hdr, lrz, sizeof(hdr));
    if (hdr.magic == KASAN_LRZ_MAGIC) {
        size_t total = KASAN_RZ + hdr.user_size + KASAN_RZ;
        kasan_poison_region(lrz, total, KASAN_POISON_HEAP_FREE);
    } else {
        size_t poison_size = heap_caps_get_allocated_size(ptr);
        if (poison_size == 0) {
            poison_size = 8;
        }
        kasan_poison_region(ptr, poison_size, KASAN_POISON_HEAP_FREE);
    }
#else
    size_t poison_size = heap_caps_get_allocated_size(ptr);
    if (poison_size == 0) {
        poison_size = 8;
    }
    kasan_poison_region(ptr, poison_size, KASAN_POISON_HEAP_FREE);
#endif

#if CONFIG_KASAN_QUARANTINE_SIZE > 0
    size_t q_size = 8;
#if CONFIG_KASAN_HEAP_REDZONE_SIZE > 0
    {
        kasan_lrz_hdr_t qhdr;
        memcpy(&qhdr, (uint8_t *)ptr - KASAN_RZ, sizeof(qhdr));
        if (qhdr.magic == KASAN_LRZ_MAGIC) {
            q_size = qhdr.user_size;
        }
    }
#endif
    kasan_q_add(ptr, q_size);
    /*
     * Bump the per-core counter *after* the block is safely in the
     * quarantine.  This keeps in-progress heap_caps_free() invocations on
     * the same core (including ISR-driven nested ones) in 1:1 lock-step with
     * kasan_heap_clear_deferred_free() consumes, so neither the outer call
     * nor the ISR-injected call can hand its pointer back to multi_heap_free
     * after the block is already queued for deferred release.
     */
    atomic_fetch_add_explicit(&s_q_defer_free[kasan_q_core_id()], 1U,
                              memory_order_release);
#endif
}

#endif /* CONFIG_COMPILER_KASAN && CONFIG_HEAP_USE_HOOKS */
