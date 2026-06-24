/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Kernel Address Sanitizer (KASAN) runtime for ESP-IDF.
 *
 * GCC -fsanitize=kernel-address instruments loads/stores with calls to
 * __asan_load<N>_noabort / __asan_store<N>_noabort.  These stubs check a
 * shadow memory region and report a violation if poisoned memory is accessed.
 *
 * Shadow layout (nibble-based):
 *   One shadow byte covers 8 bytes of real memory via two 4-bit nibbles.
 *   Low nibble (bits 0-3) → real bytes 0-3; high nibble (bits 4-7) → 4-7.
 *   Shadow address = shadow_offset + (real_addr >> 3)
 *   Nibble select  = (real_addr >> 2) & 1
 *
 * Nibble values:
 *   0x0       all 4 bytes valid
 *   0x1-0x3   first N bytes valid (partial granule)
 *   0xC       uninitialised / never allocated
 *   0xD       heap right redzone
 *   0xE       heap left redzone
 *   0xF       freed heap block
 *
 * The 4-byte granule matches TLSF's native alignment, so ROM TLSF can be used.
 *
 * This file MUST be compiled with -fno-sanitize=kernel-address.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <string.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_rom_sys.h"
#include "esp_system.h"
#include "soc/soc.h"

#if CONFIG_COMPILER_KASAN

#define KASAN_SHADOW_MAP_BASE  ((uintptr_t)SOC_DRAM_LOW)
#define KASAN_SHADOW_SIZE      ((size_t)((((uintptr_t)SOC_DRAM_HIGH - (uintptr_t)SOC_DRAM_LOW) + 7U) >> 3))

/*
 * Shadow array — DRAM_ATTR so loads/stores remain valid when the SPI flash
 * cache is disabled (IRAM KASAN stubs still run during flash operations).
 */
DRAM_ATTR uint8_t __attribute__((aligned(4)))
kasan_shadow_mem[KASAN_SHADOW_SIZE];

/*
 * Runtime shadow offset: &kasan_shadow_mem[0] - (MAP_BASE >> 3).
 * DRAM_ATTR so it is accessible with cache disabled.
 */
DRAM_ATTR uintptr_t kasan_shadow_offset;

/* Nibble poison tags */
#define KASAN_NIBBLE_VALID      0x0
#define KASAN_NIBBLE_UNINIT     0xC
#define KASAN_NIBBLE_HEAP_RRZ   0xD
#define KASAN_NIBBLE_HEAP_LRZ   0xE
#define KASAN_NIBBLE_HEAP_FREE  0xF

/*
 * Suppression counter for KASAN checks.
 *
 * Incremented (and the corresponding decrement on exit) when:
 *   - a report is in flight: the report path itself executes instrumented code,
 *     which would otherwise recurse;
 *   - kasan_disable_checks() is called explicitly: the panic handler disables
 *     checks for the remainder of crash handling, since backtrace and stack
 *     dumps legitimately read guard pages and poisoned redzones that would
 *     otherwise trigger spurious reports.
 *
 * Atomic so it is safe across cores and ISRs.  DRAM-resident so it remains
 * accessible with cache disabled.
 */
static DRAM_ATTR atomic_uint_fast32_t s_kasan_suppress_depth;

#if CONFIG_KASAN_NO_HALT
static DRAM_ATTR atomic_uint_fast32_t s_kasan_error_count;
#endif

static inline bool kasan_checks_are_disabled(void)
{
    return atomic_load_explicit(&s_kasan_suppress_depth, memory_order_relaxed) != 0;
}

static inline void kasan_report_enter(void)
{
    atomic_fetch_add_explicit(&s_kasan_suppress_depth, 1, memory_order_relaxed);
}

static inline void kasan_report_exit(void)
{
    atomic_fetch_sub_explicit(&s_kasan_suppress_depth, 1, memory_order_relaxed);
}

#if CONFIG_KASAN_NO_HALT
static inline uint32_t kasan_error_count_get(void)
{
    return (uint32_t)atomic_load_explicit(&s_kasan_error_count, memory_order_relaxed);
}

static inline void kasan_error_count_reset(void)
{
    atomic_store_explicit(&s_kasan_error_count, 0, memory_order_relaxed);
}

static inline void kasan_error_count_inc(void)
{
    atomic_fetch_add_explicit(&s_kasan_error_count, 1, memory_order_relaxed);
}
#endif

/* ---- Shadow accessors --------------------------------------------------- */

/*
 * These helpers are always inlined into their callers, so they carry no
 * placement attribute of their own: when inlined into a flash-resident API
 * (e.g. kasan_poison_region) they stay in flash, and when inlined into the
 * IRAM hot path (kasan_is_valid_access / the __asan_* stubs) they run from
 * IRAM with the rest of that function.
 */
static inline __attribute__((always_inline)) uint8_t *kasan_mem_to_shadow(uintptr_t addr)
{
    return (uint8_t *)(kasan_shadow_offset + (addr >> 3));
}

static inline __attribute__((always_inline)) int kasan_is_high_nibble(uintptr_t addr)
{
    return (addr >> 2) & 1;
}

static inline __attribute__((always_inline)) uint8_t kasan_get_nibble(uintptr_t addr)
{
    uint8_t *shadow = kasan_mem_to_shadow(addr);
    if (kasan_is_high_nibble(addr)) {
        return (*shadow >> 4) & 0xF;
    } else {
        return *shadow & 0xF;
    }
}

static inline __attribute__((always_inline)) void kasan_set_nibble(uintptr_t addr, uint8_t val)
{
    uint8_t *shadow = kasan_mem_to_shadow(addr);
    if (kasan_is_high_nibble(addr)) {
        *shadow = (*shadow & 0x0F) | ((val & 0xF) << 4);
    } else {
        *shadow = (*shadow & 0xF0) | (val & 0xF);
    }
}

static inline __attribute__((always_inline)) bool kasan_addr_in_shadow_range(uintptr_t addr)
{
    uintptr_t map_base = KASAN_SHADOW_MAP_BASE;
    uintptr_t map_end  = map_base + ((uintptr_t)KASAN_SHADOW_SIZE << 3);
    return (addr >= map_base && addr < map_end);
}

/* ---- Poison / unpoison -------------------------------------------------- */

void kasan_poison_region(const void *addr, size_t size, uint8_t tag)
{
    if (!kasan_shadow_offset || size == 0) {
        return;
    }
    uintptr_t start = (uintptr_t)addr;
    uintptr_t end   = start + size;

    uintptr_t map_base = KASAN_SHADOW_MAP_BASE;
    uintptr_t map_end  = map_base + ((uintptr_t)KASAN_SHADOW_SIZE << 3);
    if (end <= map_base || start >= map_end) {
        return;
    }
    if (start < map_base) {
        start = map_base;
    }
    if (end > map_end) {
        end = map_end;
    }

    uintptr_t aligned_start = (start + 3) & ~3UL;
    uintptr_t aligned_end = end & ~3UL;

    if (start < aligned_start && start < end) {
        kasan_set_nibble(start & ~3UL, tag);
    }

    for (uintptr_t a = aligned_start; a < aligned_end; a += 4) {
        if ((a & 4) == 0 && (a + 4) < aligned_end) {
            uint8_t *shadow = kasan_mem_to_shadow(a);
            *shadow = (tag << 4) | tag;
            a += 4;
        } else {
            kasan_set_nibble(a, tag);
        }
    }

    if (aligned_end < end) {
        kasan_set_nibble(aligned_end, tag);
    }
}

void kasan_unpoison_region(const void *addr, size_t size)
{
    if (!kasan_shadow_offset || size == 0) {
        return;
    }
    uintptr_t start = (uintptr_t)addr;
    uintptr_t end   = start + size;

    uintptr_t map_base = KASAN_SHADOW_MAP_BASE;
    uintptr_t map_end  = map_base + ((uintptr_t)KASAN_SHADOW_SIZE << 3);
    if (end <= map_base || start >= map_end) {
        return;
    }
    if (start < map_base) {
        start = map_base;
    }
    if (end > map_end) {
        end = map_end;
    }

    uintptr_t granule_start = start & ~3UL;
    uintptr_t granule_end = (end + 3) & ~3UL;

    for (uintptr_t a = granule_start; a < granule_end; a += 4) {
        if (a + 4 > end && end > a) {
            uint8_t partial = (uint8_t)(end - a);
            if (partial > 0 && partial < 4) {
                kasan_set_nibble(a, partial);
            } else {
                kasan_set_nibble(a, KASAN_NIBBLE_VALID);
            }
        } else {
            if ((a & 4) == 0 && (a + 4) < granule_end) {
                uint8_t *shadow = kasan_mem_to_shadow(a);
                *shadow = 0x00;
                a += 4;
            } else {
                kasan_set_nibble(a, KASAN_NIBBLE_VALID);
            }
        }
    }
}

/* ---- Shadow initialisation ---------------------------------------------- */

void kasan_disable_checks(void)
{
    atomic_fetch_add_explicit(&s_kasan_suppress_depth, 1, memory_order_acquire);
}

void kasan_enable_checks(void)
{
    atomic_fetch_sub_explicit(&s_kasan_suppress_depth, 1, memory_order_release);
}

void kasan_init_shadow(void)
{
    /*
     * The shadow array lives in .data (DRAM_ATTR), not .bss, so it is loaded
     * from the image rather than implicitly zeroed at startup.  Zero it here
     * explicitly so every nibble starts as 0 (= valid).  The alloc hook then
     * marks redzones and the free hook poisons freed blocks; no bulk poisoning
     * is done here so boot-path code sees clean shadow and no false positives.
     */
    memset(kasan_shadow_mem, 0, KASAN_SHADOW_SIZE);

    kasan_shadow_offset = (uintptr_t)kasan_shadow_mem
                          - (KASAN_SHADOW_MAP_BASE >> 3);

    esp_rom_printf("KASAN: kernel-address sanitizer initialized (shadow %u bytes, map base 0x%08" PRIxPTR ")\n",
                   (unsigned)KASAN_SHADOW_SIZE, (uintptr_t)KASAN_SHADOW_MAP_BASE);
}

/* ---- Error counter (CONFIG_KASAN_NO_HALT) ------------------------------- */

#if CONFIG_KASAN_NO_HALT
uint32_t kasan_get_error_count(void)
{
    return kasan_error_count_get();
}

void kasan_reset_error_count(void)
{
    kasan_error_count_reset();
}
#endif

/* ---- Access validation -------------------------------------------------- */

static inline __attribute__((always_inline)) bool kasan_is_valid_access(uintptr_t addr, size_t size)
{
    /* Address outside monitored DRAM window, not mapped to shadow region, assume valid to avoid false positives */
    if (!kasan_addr_in_shadow_range(addr) || !kasan_addr_in_shadow_range(addr + size - 1)) {
        return true;
    }

    /*
     * Fast path: both nibbles in the shadow byte are valid.
     *
     * Each shadow byte covers 8 real bytes (two 4-byte granules), so we can
     * only short-circuit when the *entire* access falls inside the shadow
     * byte(s) we have actually examined.  Larger or mis-aligned accesses fall
     * through to the slow path below, which walks every granule.
     */
    uintptr_t offset_in_byte = addr & 7U;
    uint8_t shadow_byte = *kasan_mem_to_shadow(addr);
    if (shadow_byte == 0x00) {
        if ((offset_in_byte + size) <= 8U) {
            return true;
        }
        if ((offset_in_byte + size) <= 16U) {
            uint8_t next_shadow = *kasan_mem_to_shadow(addr + 8);
            if (next_shadow == 0x00) {
                return true;
            }
        }
    }

    /* Slow path: check each granule. */
    uintptr_t end_addr = addr + size;
    for (uintptr_t a = addr; a < end_addr;) {
        uint8_t nibble = kasan_get_nibble(a);

        if (nibble == KASAN_NIBBLE_VALID) {
            a = (a & ~3UL) + 4;
            continue;
        }

        if (nibble >= 1 && nibble <= 3) {
            uintptr_t granule_base = a & ~3UL;
            uintptr_t offset_in_granule = a - granule_base;
            uintptr_t access_end_in_granule = end_addr - granule_base;
            if (access_end_in_granule > 4) {
                access_end_in_granule = 4;
            }
            if (offset_in_granule >= nibble || access_end_in_granule > nibble) {
                return false;
            }
            a = granule_base + 4;
            continue;
        }

        return false;
    }

    return true;
}

/* ---- Error reporting ---------------------------------------------------- */

/*
 * Bug-type and access-type strings live in DRAM so that the IRAM error
 * reporting path stays safe when the SPI flash cache is disabled (ISR
 * context, spi_flash operations, early boot).  Plain string literals would
 * land in .rodata (flash) and dereferencing them with cache off would mask
 * the real KASAN violation with a cache-error panic.
 */
static DRAM_ATTR const char kasan_str_use_after_free[]    = "use-after-free";
static DRAM_ATTR const char kasan_str_underflow_lrz[]     = "heap-buffer-underflow (left redzone)";
static DRAM_ATTR const char kasan_str_overflow_rrz[]      = "heap-buffer-overflow (right redzone)";
static DRAM_ATTR const char kasan_str_uninitialised[]     = "uninitialised memory";
static DRAM_ATTR const char kasan_str_overflow_partial[]  = "heap-buffer-overflow (partial granule)";
static DRAM_ATTR const char kasan_str_unknown_poison[]    = "unknown poison";
static DRAM_ATTR const char kasan_str_write[]             = "WRITE";
static DRAM_ATTR const char kasan_str_read[]              = "READ";
#if !CONFIG_KASAN_NO_HALT
static DRAM_ATTR const char kasan_str_abort_msg[]         = "KASAN: invalid memory access";
#endif

static DRAM_ATTR const char kasan_fmt_error[] = "KASAN error: %s of size %u at 0x%08x\n";
static DRAM_ATTR const char kasan_fmt_bug[]   = "  Bug type: %s (shadow nibble=0x%x)\n";

static IRAM_ATTR const char *kasan_nibble_to_string(uint8_t nibble)
{
    switch (nibble) {
    case KASAN_NIBBLE_HEAP_FREE: return kasan_str_use_after_free;
    case KASAN_NIBBLE_HEAP_LRZ:  return kasan_str_underflow_lrz;
    case KASAN_NIBBLE_HEAP_RRZ:  return kasan_str_overflow_rrz;
    case KASAN_NIBBLE_UNINIT:    return kasan_str_uninitialised;
    default:
        if (nibble >= 1 && nibble <= 3) {
            return kasan_str_overflow_partial;
        }
        return kasan_str_unknown_poison;
    }
}

static IRAM_ATTR void kasan_report(uintptr_t addr, size_t size, bool is_write)
{
    kasan_report_enter();

    if (esp_cpu_dbgr_is_attached()) {
        esp_cpu_dbgr_break();
    }

    const char *access_type = is_write ? kasan_str_write : kasan_str_read;
    uint8_t nibble = kasan_get_nibble(addr);
    const char *bug_type = kasan_nibble_to_string(nibble);

    esp_rom_printf(kasan_fmt_error, access_type, (unsigned)size, (unsigned)addr);
    esp_rom_printf(kasan_fmt_bug, bug_type, nibble);

#if CONFIG_KASAN_NO_HALT
    kasan_error_count_inc();
    kasan_report_exit();
#else
    /*
     * Avoid flash-resident helpers (strlcat, libc string operations) here:
     * kasan_report runs from IRAM and may execute with the SPI flash cache
     * disabled.  esp_rom_printf above has already emitted the detailed
     * diagnostic via ROM; pass a short DRAM-resident message to the abort
     * path so the panic itself does not touch flash.
     */
    esp_system_abort(kasan_str_abort_msg);
#endif
}

/* ---- Check dispatcher --------------------------------------------------- */

static IRAM_ATTR void kasan_check(uintptr_t addr, size_t size, bool is_write)
{
    if (__builtin_expect(kasan_checks_are_disabled() || !kasan_shadow_offset, 0)) {
        return;
    }
    if (!kasan_is_valid_access(addr, size)) {
        kasan_report(addr, size, is_write);
    }
}

/* ---- GCC-emitted KASAN stubs -------------------------------------------- */

/*
 * __attribute__((used)) prevents --gc-sections from removing stubs that GCC's
 * instrumentation pass calls but that the linker cannot see at analysis time.
 */
__attribute__((used)) void IRAM_ATTR __asan_load1_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 1, false);
}

__attribute__((used)) void IRAM_ATTR __asan_load2_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 2, false);
}

__attribute__((used)) void IRAM_ATTR __asan_load4_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 4, false);
}

__attribute__((used)) void IRAM_ATTR __asan_load8_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 8, false);
}

__attribute__((used)) void IRAM_ATTR __asan_load16_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 16, false);
}

__attribute__((used)) void IRAM_ATTR __asan_loadN_noabort(void *addr, size_t size)
{
    kasan_check((uintptr_t)addr, size, false);
}

__attribute__((used)) void IRAM_ATTR __asan_store1_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 1, true);
}

__attribute__((used)) void IRAM_ATTR __asan_store2_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 2, true);
}

__attribute__((used)) void IRAM_ATTR __asan_store4_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 4, true);
}

__attribute__((used)) void IRAM_ATTR __asan_store8_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 8, true);
}

__attribute__((used)) void IRAM_ATTR __asan_store16_noabort(void *addr)
{
    kasan_check((uintptr_t)addr, 16, true);
}

__attribute__((used)) void IRAM_ATTR __asan_storeN_noabort(void *addr, size_t size)
{
    kasan_check((uintptr_t)addr, size, true);
}

/* Called before noreturn functions; nothing to do on bare-metal. */
void IRAM_ATTR __asan_handle_no_return(void)
{
}

#endif /* CONFIG_COMPILER_KASAN */
