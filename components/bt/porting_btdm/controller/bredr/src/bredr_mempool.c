/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_bt.h"
#include "esp_log.h"
#include "btdm_osal.h"
#include "bredr_mempool.h"
#include "bredr_user_cfg.h"

#if UC_BT_CTRL_BR_EDR_IS_ENABLE

#define BREDR_LOG_TAG "BREDR_MEMPOOL"

orca_mempool_t tlsf_create_with_pool(void *mem, size_t pool_bytes, size_t max_bytes);
void *tlsf_malloc(orca_mempool_t tlsf, size_t size);
void tlsf_free(orca_mempool_t tlsf, void *ptr);
size_t tlsf_block_size(void *ptr);
size_t tlsf_alloc_overhead(void);

#define BREDR_MEMPOOL_PC_RECORDS 64

/* Pool buffer is owned by the controller. */
static void IRAM_ATTR bredr_mempool_destroy(orca_mempool_t mempool)
{
    (void)mempool;
}

#if BREDR_MEMPOOL_TRACE

typedef struct {
    size_t used_bytes;
    size_t peak_used_bytes;
    size_t alloc_count;
    size_t free_count;
    size_t fail_count;
    size_t overflow_count;
    size_t live_count;
    size_t bad_free_count;
    size_t pool_bytes;
} bredr_mempool_trace_summary_t;

#if BREDR_MEMPOOL_TRACE_PC && CONFIG_IDF_TARGET_ARCH_RISCV
#define BREDR_MEMPOOL_PC_CAPTURE 1
#else
#define BREDR_MEMPOOL_PC_CAPTURE 0
#endif

#if BREDR_MEMPOOL_PC_CAPTURE

typedef struct {
    void *ptr;
    size_t cost;
    void *pc;
} bredr_mempool_pc_rec_t;

/* 2x records, chaining. Alloc/free locate a slot without scanning the table. */
#define BREDR_MEMPOOL_PC_NIL       0xFFu
#define BREDR_MEMPOOL_PC_HASH_BITS 7u
#define BREDR_MEMPOOL_PC_HASH      (1u << BREDR_MEMPOOL_PC_HASH_BITS)
_Static_assert(BREDR_MEMPOOL_PC_RECORDS <= BREDR_MEMPOOL_PC_NIL, "pc index must fit in uint8_t");
_Static_assert((BREDR_MEMPOOL_PC_HASH & (BREDR_MEMPOOL_PC_HASH - 1u)) == 0u,
               "pc hash size must be a power of two");

#include "esp_memory_utils.h"

#define BREDR_MEMPOOL_PROLOGUE_SEARCH_BYTES 256U
/* addi and sw ra are not adjacent: the wrapper saves s0 and loads the ops first. */
#define BREDR_MEMPOOL_SP_SEARCH_BYTES       64U
#define BREDR_MEMPOOL_PROLOGUE_CACHE        16
#define BREDR_MEMPOOL_PROLOGUE_PENDING      BREDR_MEMPOOL_PROLOGUE_CACHE

#define BREDR_MEMPOOL_INLINE static inline __attribute__((always_inline))
#define BREDR_MEMPOOL_IRAM_INLINE static inline __attribute__((always_inline)) IRAM_ATTR
#define BREDR_MEMPOOL_IRAM_NOINLINE static __attribute__((noinline)) IRAM_ATTR

#if (SOC_IRAM_LOW == SOC_DRAM_LOW) && (SOC_IRAM_HIGH == SOC_DRAM_HIGH)
extern char _iram_text_start;
extern char _iram_text_end;
#endif

/* BAD_FRAME clears on the next complete capture. */
enum {
    BREDR_MEMPOOL_PC_OK = 0,
    BREDR_MEMPOOL_PC_BAD_FRAME,
};

typedef struct {
    uintptr_t ra;
    uint32_t frame_size;
    uint32_t ra_off;
    bool used;
    bool ok;
} bredr_mempool_prologue_ent_t;

#endif /* BREDR_MEMPOOL_PC_CAPTURE */

/*
 * Trace state. dump_summary is scratch copied out of the HW critical section
 * and is not re-entrant. pc and prologue exist only while PC capture is on.
 */
static struct {
    bredr_mempool_trace_summary_t summary;
    size_t alloc_overhead;
    bredr_mempool_trace_summary_t dump_summary;
#if BREDR_MEMPOOL_TRACE_PC
    bool pc_status_logged;
#endif
#if BREDR_MEMPOOL_PC_CAPTURE
    /* hash chains record indexes. cache is direct-mapped by return address. */
    struct {
        bredr_mempool_pc_rec_t rec[BREDR_MEMPOOL_PC_RECORDS];
        bredr_mempool_pc_rec_t dump[BREDR_MEMPOOL_PC_RECORDS];
        uint8_t hash[BREDR_MEMPOOL_PC_HASH];
        uint8_t next[BREDR_MEMPOOL_PC_RECORDS];
        uint8_t free_slot;
        uint8_t status;
        uintptr_t fail_ra;
    } pc;

    struct {
        bredr_mempool_prologue_ent_t cache[BREDR_MEMPOOL_PROLOGUE_CACHE];
        uintptr_t pending[BREDR_MEMPOOL_PROLOGUE_PENDING];
        uint8_t pending_n;
        /* Alloc path refused a flash insn fetch. Not a hard failure. */
        bool flash_skipped;
    } prologue;
#endif
} s_bredr_mempool;

#if BREDR_MEMPOOL_PC_CAPTURE

/*
 * Address helpers (IRAM-safe; do not call esp_ptr_executable from alloc path)
 */
BREDR_MEMPOOL_INLINE
bool bredr_mempool_pc_is_code(const void *pc)
{
    uintptr_t ip = (uintptr_t)pc;

    /* Compressed instructions are halfword-aligned. */
    if (pc == NULL || (ip & 0x1U) != 0U) {
        return false;
    }

#if (SOC_IRAM_LOW == SOC_DRAM_LOW) && (SOC_IRAM_HIGH == SOC_DRAM_HIGH)
    /* Shared IRAM/DRAM: only linked text is executable. */
    const bool in_iram_text = (ip >= (uintptr_t)&_iram_text_start) &&
                              (ip < (uintptr_t)&_iram_text_end);
#else
    const bool in_iram_text = (ip >= SOC_IRAM_LOW) && (ip < SOC_IRAM_HIGH);
#endif

    return (ip >= SOC_IROM_LOW && ip < SOC_IROM_HIGH)
        || (ip >= SOC_IROM_MASK_LOW && ip < SOC_IROM_MASK_HIGH)
        || in_iram_text
#if defined(SOC_CACHE_APP_LOW) && defined(CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
        || (ip >= SOC_CACHE_APP_LOW && ip < SOC_CACHE_APP_HIGH)
#endif
#if SOC_RTC_FAST_MEM_SUPPORTED && \
    !((SOC_RTC_IRAM_LOW == SOC_RTC_DRAM_LOW) && (SOC_RTC_IRAM_HIGH == SOC_RTC_DRAM_HIGH))
        || (ip >= SOC_RTC_IRAM_LOW && ip < SOC_RTC_IRAM_HIGH)
#endif
        ;
}

/*
 * One caller above the controller wrapper.
 * Requires CONFIG_IDF_TARGET_ARCH_RISCV, noinline bredr_mempool_malloc with RA at CFA-4,
 * and a prologue of addi / c.addi / c.addi16sp then sw / c.swsp ra within 64 bytes.
 */
BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_dram_u32(uintptr_t addr, uint32_t *out)
{
    if ((addr & 0x3U) != 0U ||
        !esp_ptr_in_dram((const void *)addr) ||
        !esp_ptr_in_dram((const void *)(addr + sizeof(uint32_t) - 1U))) {
        return false;
    }
    *out = *(const uint32_t *)addr;
    return true;
}

/* Flash text is not readable while the cache is off. */
extern bool spi_flash_cache_enabled(void);

BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_in_flash(uintptr_t ip)
{
    return (ip >= SOC_IROM_LOW && ip < SOC_IROM_HIGH)
        || (ip >= SOC_IROM_MASK_LOW && ip < SOC_IROM_MASK_HIGH)
#if defined(SOC_CACHE_APP_LOW) && defined(CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
        || (ip >= SOC_CACHE_APP_LOW && ip < SOC_CACHE_APP_HIGH)
#endif
        ;
}

/*
 * Prologue decode. 32-bit insns are only read when both parcels are executable
 * and the whole insn lies in [addr, limit).
 * allow_flash is false on the alloc path: flash text is queued, not read.
 */
BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_read_half(uintptr_t addr, uint16_t *out, bool allow_flash)
{
    if (!bredr_mempool_pc_is_code((const void *)addr)) {
        return false;
    }
    if (bredr_mempool_in_flash(addr)) {
        if (!allow_flash) {
            s_bredr_mempool.prologue.flash_skipped = true;
            return false;
        }
        if (!spi_flash_cache_enabled()) {
            return false;
        }
    }
    *out = *(const uint16_t *)addr;
    return true;
}

BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_read_insn32(uintptr_t addr, uintptr_t limit, uint32_t *insn, bool allow_flash)
{
    uint16_t lo;
    uint16_t hi;

    if (addr > limit || limit - addr < 4U) {
        return false;
    }
    if (!bredr_mempool_read_half(addr, &lo, allow_flash) || (lo & 0x3U) != 0x3U ||
        !bredr_mempool_read_half(addr + 2U, &hi, allow_flash)) {
        return false;
    }
    *insn = (uint32_t)lo | ((uint32_t)hi << 16);
    return true;
}

BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_frame_ok(uint32_t frame_size)
{
    /* RV ABI keeps SP 16-byte aligned, so a random addi is rejected. */
    return frame_size != 0U && (frame_size & 0xfU) == 0U;
}

BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_decode_sp_alloc(uintptr_t addr, uintptr_t limit, uint32_t *frame_size,
                                  bool allow_flash)
{
    uint16_t h;
    uint32_t size = 0;

    if (addr > limit || limit - addr < 2U || !bredr_mempool_read_half(addr, &h, allow_flash)) {
        return false;
    }

    /* c.addi16sp */
    if ((h & 0x3U) != 0x3U && (h & 0xef83U) == 0x6101U) {
        int32_t imm = (int32_t)((((h >> 12) & 0x1U) << 9) |
                                (((h >> 6) & 0x1U) << 4) |
                                (((h >> 5) & 0x1U) << 6) |
                                (((h >> 3) & 0x3U) << 7) |
                                (((h >> 2) & 0x1U) << 5));
        if ((imm & 0x200) != 0) {
            imm |= ~0x3ff;
        }
        if (imm < 0) {
            size = (uint32_t)-imm;
        }
    }

    /* c.addi sp, sp, imm */
    if (size == 0U && (h & 0x3U) != 0x3U && (h & 0xef83U) == 0x0101U) {
        int32_t imm = (int32_t)((((h >> 12) & 0x1U) << 5) | ((h >> 2) & 0x1fU));
        if ((imm & 0x20) != 0) {
            imm |= ~0x3f;
        }
        if (imm < 0) {
            size = (uint32_t)-imm;
        }
    }

    /* addi sp, sp, imm */
    if (size == 0U && (h & 0x3U) == 0x3U) {
        uint32_t insn;

        if (!bredr_mempool_read_insn32(addr, limit, &insn, allow_flash)) {
            return false;
        }
        if ((insn & 0x000fffffU) == 0x00010113U) {
            int32_t imm = (int32_t)insn >> 20;
            if (imm < 0) {
                size = (uint32_t)-imm;
            }
        }
    }

    if (!bredr_mempool_frame_ok(size)) {
        return false;
    }
    *frame_size = size;
    return true;
}

BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_decode_ra_save(uintptr_t addr, uintptr_t limit, uint32_t *ra_offset,
                                 bool allow_flash)
{
    uint16_t h;
    uint32_t off = 0;
    bool matched = false;

    if (addr > limit || limit - addr < 2U || !bredr_mempool_read_half(addr, &h, allow_flash)) {
        return false;
    }

    /* c.swsp ra, offset(sp). Low bits are 10, so this is not a 32-bit opcode. */
    if ((h & 0x3U) != 0x3U && (h & 0xe07fU) == 0xc006U) {
        off = (((uint32_t)h >> 9) & 0xfU) << 2 |
              (((uint32_t)h >> 7) & 0x3U) << 6;
        matched = true;
    }

    /* sw ra, offset(sp) */
    if (!matched && (h & 0x3U) == 0x3U) {
        uint32_t insn;

        if (!bredr_mempool_read_insn32(addr, limit, &insn, allow_flash)) {
            return false;
        }
        if ((insn & 0x01fff07fU) == 0x00112023U) {
            off = ((insn >> 7) & 0x1fU) | ((insn >> 25) << 5);
            matched = true;
        }
    }

    if (!matched || (off & 0x3U) != 0U) {
        return false;
    }
    *ra_offset = off;
    return true;
}

/* Nearest addi / c.addi / c.addi16sp before the RA save. A high parcel is skipped. */
BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_prev_sp_alloc(uintptr_t ra_save, uint32_t *frame_size, bool allow_flash)
{
    for (uint32_t distance = 2U; distance <= BREDR_MEMPOOL_SP_SEARCH_BYTES; distance += 2U) {
        uintptr_t addr;
        uint16_t h;
        uint16_t prev;

        if ((uintptr_t)distance > ra_save) {
            break;
        }
        addr = ra_save - distance;
        if (!bredr_mempool_read_half(addr, &h, allow_flash)) {
            return false;
        }
        if ((h & 0x3U) != 0x3U && addr >= 2U &&
            bredr_mempool_read_half(addr - 2U, &prev, allow_flash) && (prev & 0x3U) == 0x3U) {
            continue;
        }
        if (bredr_mempool_decode_sp_alloc(addr, ra_save, frame_size, allow_flash)) {
            return true;
        }
    }
    return false;
}

/* True if pc is the instruction after a call that links ra or t0. */
BREDR_MEMPOOL_IRAM_INLINE
bool bredr_mempool_pc_follows_call(uintptr_t pc, bool allow_flash)
{
    uint16_t h;
    uint32_t insn;
    uint32_t rd;

    if (pc < 2U || !bredr_mempool_read_half(pc - 2U, &h, allow_flash)) {
        return false;
    }

    /* c.jal / c.jalr occupy [pc-2, pc). */
    if ((h & 0x3U) != 0x3U) {
        if ((h & 0xe003U) == 0x2001U) {
            return true; /* c.jal */
        }
        if ((h & 0xf07fU) == 0x9002U && ((h >> 7) & 0x1fU) != 0U) {
            return true; /* c.jalr, rs1 != 0 */
        }
    }

    /*
     * jal / jalr opcode is at pc-4. The half at pc-2 is the high half and
     * its low bits are not an opcode, so a compressed check cannot see it.
     */
    if (pc < 4U || !bredr_mempool_read_insn32(pc - 4U, pc, &insn, allow_flash)) {
        return false;
    }
    rd = (insn >> 7) & 0x1fU;
    return ((insn & 0x7fU) == 0x6fU || (insn & 0x707fU) == 0x67U) &&
           (rd == 1U || rd == 5U);
}

BREDR_MEMPOOL_IRAM_NOINLINE
bool bredr_mempool_measure_prologue(uintptr_t pc, uint32_t *frame_size, uint32_t *ra_offset,
                                    bool allow_flash)
{
    for (uint32_t distance = 2U; distance <= BREDR_MEMPOOL_PROLOGUE_SEARCH_BYTES; distance += 2U) {
        uintptr_t ra_save;
        uint32_t off = 0;
        uint32_t size = 0;

        if ((uintptr_t)distance > pc) {
            break;
        }
        ra_save = pc - distance;
        if (!bredr_mempool_pc_is_code((const void *)ra_save)) {
            break;
        }
        if (!bredr_mempool_decode_ra_save(ra_save, pc, &off, allow_flash) ||
            !bredr_mempool_prev_sp_alloc(ra_save, &size, allow_flash) ||
            off + sizeof(uint32_t) > size) {
            continue;
        }

        *frame_size = size;
        *ra_offset = off;
        return true;
    }
    return false;
}

BREDR_MEMPOOL_IRAM_INLINE
size_t bredr_mempool_prologue_slot(uintptr_t ra)
{
    return ((size_t)(ra >> 2)) & (BREDR_MEMPOOL_PROLOGUE_CACHE - 1U);
}

/* Caller holds the controller HW critical section. */
static void IRAM_ATTR bredr_mempool_pending_add(uintptr_t ra)
{
    if (ra == 0U) {
        return;
    }
    for (uint8_t i = 0; i < s_bredr_mempool.prologue.pending_n; i++) {
        if (s_bredr_mempool.prologue.pending[i] == ra) {
            return;
        }
    }
    if (s_bredr_mempool.prologue.pending_n >= BREDR_MEMPOOL_PROLOGUE_PENDING) {
        return;
    }
    s_bredr_mempool.prologue.pending[s_bredr_mempool.prologue.pending_n++] = ra;
}

/* Publish under the controller HW critical section. */
static void IRAM_ATTR bredr_mempool_prologue_store(uintptr_t pc, bool ok, uint32_t frame_size, uint32_t ra_off)
{
    bredr_mempool_prologue_ent_t *ent = &s_bredr_mempool.prologue.cache[bredr_mempool_prologue_slot(pc)];

    ent->ra = pc;
    ent->ok = ok;
    ent->frame_size = ok ? frame_size : 0;
    ent->ra_off = ok ? ra_off : 0;
    ent->used = true;
}

/*
 * Flash text is scanned here, outside the controller HW critical section.
 * A scan that loses the cache is queued again and not stored.
 */
static void bredr_mempool_prologue_resolve(const uintptr_t *ras, uint8_t n)
{
    for (uint8_t i = 0; i < n; i++) {
        uintptr_t ra = ras[i];
        uint32_t frame_size = 0;
        uint32_t ra_off = 0;
        bool ok;
        uint32_t crit;

        if (bredr_mempool_in_flash(ra) && !spi_flash_cache_enabled()) {
            crit = btdm_osal_hw_enter_critical();
            bredr_mempool_pending_add(ra);
            btdm_osal_hw_exit_critical(crit);
            continue;
        }

        ok = bredr_mempool_pc_follows_call(ra, true) &&
             bredr_mempool_measure_prologue(ra, &frame_size, &ra_off, true);
        if (bredr_mempool_in_flash(ra) && !spi_flash_cache_enabled()) {
            crit = btdm_osal_hw_enter_critical();
            bredr_mempool_pending_add(ra);
            btdm_osal_hw_exit_critical(crit);
            continue;
        }

        crit = btdm_osal_hw_enter_critical();
        bredr_mempool_prologue_store(ra, ok, frame_size, ra_off);
        btdm_osal_hw_exit_critical(crit);
    }
}

/*
 * Cached result is reused. Flash text is not read here: the return address is
 * queued and decoded from dump. A scan that skipped flash is not stored.
 */
BREDR_MEMPOOL_IRAM_NOINLINE
bool bredr_mempool_prologue_for(uintptr_t pc, uint32_t *frame_size, uint32_t *ra_offset)
{
    bredr_mempool_prologue_ent_t *ent = &s_bredr_mempool.prologue.cache[bredr_mempool_prologue_slot(pc)];
    bool ok = false;

    if (ent->used && ent->ra == pc) {
        *frame_size = ent->frame_size;
        *ra_offset = ent->ra_off;
        return ent->ok;
    }
    if (bredr_mempool_in_flash(pc)) {
        bredr_mempool_pending_add(pc);
        s_bredr_mempool.prologue.flash_skipped = true;
        return false;
    }

    ok = bredr_mempool_measure_prologue(pc, frame_size, ra_offset, false);
    if (s_bredr_mempool.prologue.flash_skipped) {
        return false;
    }
    ent->used = true;
    ent->ra = pc;
    ent->ok = ok;
    ent->frame_size = ok ? *frame_size : 0;
    ent->ra_off = ok ? *ra_offset : 0;
    return ok;
}

BREDR_MEMPOOL_IRAM_INLINE
void bredr_mempool_note_pc_fail(uintptr_t ra)
{
    /* A deferred flash scan is retried. Keep the first real failure in this episode. */
    if (s_bredr_mempool.prologue.flash_skipped || s_bredr_mempool.pc.status != BREDR_MEMPOOL_PC_OK) {
        return;
    }
    s_bredr_mempool.pc.fail_ra = ra;
    s_bredr_mempool.pc.status = BREDR_MEMPOOL_PC_BAD_FRAME;
}

BREDR_MEMPOOL_IRAM_INLINE
void bredr_mempool_note_pc_ok(void)
{
    s_bredr_mempool.pc.status = BREDR_MEMPOOL_PC_OK;
    s_bredr_mempool.pc.fail_ra = 0;
    s_bredr_mempool.pc_status_logged = false;
}

/*
 * Inlined into bredr_mempool_malloc so CFA-4 is that function's saved RA.
 * The direct caller is the controller wrapper and is not returned.
 */
BREDR_MEMPOOL_INLINE
void *bredr_mempool_capture_caller(void)
{
    uintptr_t cfa;
    uintptr_t ra;
    uint32_t frame_size = 0;
    uint32_t ra_off = 0;
    uint32_t saved_ra = 0;
    uint32_t caller = 0;

    s_bredr_mempool.prologue.flash_skipped = false;
    cfa = (uintptr_t)__builtin_frame_address(0);
    ra = (uintptr_t)__builtin_return_address(0);

    if (cfa < sizeof(uint32_t) ||
        !bredr_mempool_dram_u32(cfa - sizeof(uint32_t), &saved_ra) ||
        saved_ra != (uint32_t)ra) {
        bredr_mempool_note_pc_fail(ra);
        return NULL;
    }

    /* Flash insns are not read under the HW critical section. */
    if (!bredr_mempool_in_flash(ra) && !bredr_mempool_pc_follows_call(ra, false)) {
        bredr_mempool_note_pc_fail(ra);
        return NULL;
    }

    if (!bredr_mempool_prologue_for(ra, &frame_size, &ra_off)) {
        if (!s_bredr_mempool.prologue.flash_skipped) {
            bredr_mempool_note_pc_fail(ra);
        }
        return NULL;
    }

    if ((cfa + frame_size) <= cfa ||
        (cfa + ra_off) < cfa ||
        ra_off + sizeof(uint32_t) > frame_size ||
        !bredr_mempool_dram_u32(cfa + ra_off, &caller)) {
        bredr_mempool_note_pc_fail(ra);
        return NULL;
    }

    if (!bredr_mempool_in_flash((uintptr_t)caller) &&
        !bredr_mempool_pc_follows_call((uintptr_t)caller, false)) {
        bredr_mempool_note_pc_fail(ra);
        return NULL;
    }

    bredr_mempool_note_pc_ok();
    return (void *)(uintptr_t)caller;
}

BREDR_MEMPOOL_IRAM_INLINE
size_t bredr_mempool_pc_hash_slot(const void *ptr)
{
    uintptr_t x = (uintptr_t)ptr >> 2;

    x *= (uintptr_t)0x9E3779B1u;
    return (size_t)(x >> ((sizeof(uintptr_t) * 8u) - BREDR_MEMPOOL_PC_HASH_BITS));
}

static void bredr_mempool_pc_index_reset(void)
{
    memset(s_bredr_mempool.pc.rec, 0, sizeof(s_bredr_mempool.pc.rec));
    for (size_t i = 0; i < BREDR_MEMPOOL_PC_HASH; i++) {
        s_bredr_mempool.pc.hash[i] = BREDR_MEMPOOL_PC_NIL;
    }
    for (uint8_t i = 0; i < (uint8_t)(BREDR_MEMPOOL_PC_RECORDS - 1u); i++) {
        s_bredr_mempool.pc.next[i] = (uint8_t)(i + 1u);
    }
    s_bredr_mempool.pc.next[BREDR_MEMPOOL_PC_RECORDS - 1u] = BREDR_MEMPOOL_PC_NIL;
    s_bredr_mempool.pc.free_slot = 0;
}

static IRAM_ATTR void bredr_mempool_pc_store(void *ptr, size_t cost, void *pc)
{
    uint8_t slot;
    size_t h;
    bredr_mempool_pc_rec_t *rec;

    slot = s_bredr_mempool.pc.free_slot;
    if (slot == BREDR_MEMPOOL_PC_NIL) {
        s_bredr_mempool.summary.overflow_count++;
        return;
    }

    s_bredr_mempool.pc.free_slot = s_bredr_mempool.pc.next[slot];
    rec = &s_bredr_mempool.pc.rec[slot];
    rec->ptr = ptr;
    rec->cost = cost;
    rec->pc = pc;

    h = bredr_mempool_pc_hash_slot(ptr);
    s_bredr_mempool.pc.next[slot] = s_bredr_mempool.pc.hash[h];
    s_bredr_mempool.pc.hash[h] = slot;
}

static IRAM_ATTR void bredr_mempool_pc_clear(void *ptr)
{
    size_t h;
    uint8_t slot;
    uint8_t prev;
    bredr_mempool_pc_rec_t *rec;

    h = bredr_mempool_pc_hash_slot(ptr);
    prev = BREDR_MEMPOOL_PC_NIL;
    slot = s_bredr_mempool.pc.hash[h];
    while (slot != BREDR_MEMPOOL_PC_NIL) {
        uint8_t next = s_bredr_mempool.pc.next[slot];

        rec = &s_bredr_mempool.pc.rec[slot];
        if (rec->ptr != ptr) {
            prev = slot;
            slot = next;
            continue;
        }

        if (prev == BREDR_MEMPOOL_PC_NIL) {
            s_bredr_mempool.pc.hash[h] = next;
        } else {
            s_bredr_mempool.pc.next[prev] = next;
        }
        rec->ptr = NULL;
        rec->cost = 0;
        rec->pc = NULL;
        s_bredr_mempool.pc.next[slot] = s_bredr_mempool.pc.free_slot;
        s_bredr_mempool.pc.free_slot = slot;
        return;
    }
}

#endif /* BREDR_MEMPOOL_PC_CAPTURE */

/*
 * Accounting. used_bytes follows tlsf_block_size plus the alloc overhead.
 * PC slots are optional and do not affect the byte counters.
 */
static IRAM_ATTR size_t bredr_mempool_cost(void *ptr)
{
    return tlsf_block_size(ptr) + s_bredr_mempool.alloc_overhead;
}

/*
 * Mempool ops (run under controller HW critical section)
 */
static orca_mempool_t bredr_mempool_create_with_pool(void *mem, size_t pool_bytes, size_t max_bytes)
{
    orca_mempool_t tlsf = tlsf_create_with_pool(mem, pool_bytes, max_bytes);

    if (tlsf == NULL) {
        return NULL;
    }

    memset(&s_bredr_mempool.summary, 0, sizeof(s_bredr_mempool.summary));
    s_bredr_mempool.summary.pool_bytes = pool_bytes;
    s_bredr_mempool.alloc_overhead = tlsf_alloc_overhead();
#if BREDR_MEMPOOL_PC_CAPTURE
    bredr_mempool_pc_index_reset();
    s_bredr_mempool.pc_status_logged = false;
    s_bredr_mempool.pc.status = BREDR_MEMPOOL_PC_OK;
    s_bredr_mempool.pc.fail_ra = 0;
    s_bredr_mempool.prologue.flash_skipped = false;
    s_bredr_mempool.prologue.pending_n = 0;
    memset(s_bredr_mempool.prologue.cache, 0, sizeof(s_bredr_mempool.prologue.cache));
    memset(s_bredr_mempool.prologue.pending, 0, sizeof(s_bredr_mempool.prologue.pending));
#elif BREDR_MEMPOOL_TRACE_PC
    s_bredr_mempool.pc_status_logged = false;
#endif
    return tlsf;
}

static __attribute__((noinline)) void *IRAM_ATTR bredr_mempool_malloc(orca_mempool_t tlsf, size_t size)
{
    void *ptr;
    size_t cost;

    ptr = tlsf_malloc(tlsf, size);
    if (ptr == NULL) {
        s_bredr_mempool.summary.fail_count++;
        return NULL;
    }

    cost = bredr_mempool_cost(ptr);
    s_bredr_mempool.summary.used_bytes += cost;
    if (s_bredr_mempool.summary.used_bytes > s_bredr_mempool.summary.peak_used_bytes) {
        s_bredr_mempool.summary.peak_used_bytes = s_bredr_mempool.summary.used_bytes;
    }
#if BREDR_MEMPOOL_PC_CAPTURE
    bredr_mempool_pc_store(ptr, cost, bredr_mempool_capture_caller());
#endif
    s_bredr_mempool.summary.alloc_count++;
    s_bredr_mempool.summary.live_count++;
    return ptr;
}

static void IRAM_ATTR bredr_mempool_free(orca_mempool_t tlsf, void *ptr)
{
    size_t cost;

    if (ptr == NULL) {
        return;
    }

    cost = bredr_mempool_cost(ptr);
    if (s_bredr_mempool.summary.used_bytes < cost || s_bredr_mempool.summary.live_count == 0) {
        /* Keep the previous totals so one bad free does not wipe them. */
        s_bredr_mempool.summary.bad_free_count++;
    } else {
        s_bredr_mempool.summary.used_bytes -= cost;
        s_bredr_mempool.summary.live_count--;
    }
#if BREDR_MEMPOOL_PC_CAPTURE
    bredr_mempool_pc_clear(ptr);
#endif
    s_bredr_mempool.summary.free_count++;
    tlsf_free(tlsf, ptr);
}

void esp_bredr_mempool_dump_stats(void)
{
    uint32_t crit = btdm_osal_hw_enter_critical();
    const bredr_mempool_trace_summary_t *sum;
#if BREDR_MEMPOOL_PC_CAPTURE
    uint8_t pc_status;
    uintptr_t pc_fail_ra;
    uintptr_t pending_ra[BREDR_MEMPOOL_PROLOGUE_PENDING];
    uint8_t pending_n;
#endif

    s_bredr_mempool.dump_summary = s_bredr_mempool.summary;
#if BREDR_MEMPOOL_PC_CAPTURE
    pc_status = s_bredr_mempool.pc.status;
    pc_fail_ra = s_bredr_mempool.pc.fail_ra;
    pending_n = s_bredr_mempool.prologue.pending_n;
    if (pending_n > BREDR_MEMPOOL_PROLOGUE_PENDING) {
        pending_n = BREDR_MEMPOOL_PROLOGUE_PENDING;
    }
    for (uint8_t i = 0; i < pending_n; i++) {
        pending_ra[i] = s_bredr_mempool.prologue.pending[i];
    }
    s_bredr_mempool.prologue.pending_n = 0;
#endif
    btdm_osal_hw_exit_critical(crit);

#if BREDR_MEMPOOL_PC_CAPTURE
    /* Outside critical: memcpy of PC records is slow for BT timing. */
    memcpy(s_bredr_mempool.pc.dump, s_bredr_mempool.pc.rec,
           sizeof(s_bredr_mempool.pc.dump));
    /* Flash prologue scans stay outside the controller HW critical section. */
    bredr_mempool_prologue_resolve(pending_ra, pending_n);
#endif

    sum = &s_bredr_mempool.dump_summary;

#if BREDR_MEMPOOL_PC_CAPTURE
    if (pc_status != BREDR_MEMPOOL_PC_OK && !s_bredr_mempool.pc_status_logged) {
        ESP_LOGE(BREDR_LOG_TAG,
                 "pc capture failed: return address %p is not at CFA-4 "
                 "or the caller prologue did not match",
                 (void *)pc_fail_ra);
        s_bredr_mempool.pc_status_logged = true;
    }
#elif BREDR_MEMPOOL_TRACE_PC
    if (!s_bredr_mempool.pc_status_logged) {
        ESP_LOGE(BREDR_LOG_TAG,
                 "pc capture off: RISC-V prologue unwind only");
        s_bredr_mempool.pc_status_logged = true;
    }
#endif

    ESP_LOGI(BREDR_LOG_TAG,
             "mempool: pool=%u used=%u peak=%u alloc=%u free=%u fail=%u overflow=%u live=%u bad_free=%u",
             (unsigned)sum->pool_bytes,
             (unsigned)sum->used_bytes,
             (unsigned)sum->peak_used_bytes,
             (unsigned)sum->alloc_count,
             (unsigned)sum->free_count,
             (unsigned)sum->fail_count,
             (unsigned)sum->overflow_count,
             (unsigned)sum->live_count,
             (unsigned)sum->bad_free_count);
    if (sum->bad_free_count != 0) {
        ESP_LOGW(BREDR_LOG_TAG,
                 "bad free: count=%u used=%u live=%u",
                 (unsigned)sum->bad_free_count,
                 (unsigned)sum->used_bytes,
                 (unsigned)sum->live_count);
    }

#if BREDR_MEMPOOL_PC_CAPTURE
    if (sum->overflow_count != 0) {
        ESP_LOGW(BREDR_LOG_TAG,
                 "pc list is partial: overflow=%u live=%u",
                 (unsigned)sum->overflow_count, (unsigned)sum->live_count);
    }
    for (size_t i = 0; i < BREDR_MEMPOOL_PC_RECORDS; i++) {
        const bredr_mempool_pc_rec_t *rec = &s_bredr_mempool.pc.dump[i];

        if (rec->ptr == NULL) {
            continue;
        }
        ESP_LOGI(BREDR_LOG_TAG, "  [%u] ptr=%p cost=%u pc=%p",
                 (unsigned)i, rec->ptr, (unsigned)rec->cost, rec->pc);
    }
#endif
}

static struct orca_mempool_ops s_orca_mempool_ops = {
    .create_with_pool = bredr_mempool_create_with_pool,
    .destroy = bredr_mempool_destroy,
    .malloc = bredr_mempool_malloc,
    .free = bredr_mempool_free,
};

#else /* !BREDR_MEMPOOL_TRACE */

void esp_bredr_mempool_dump_stats(void)
{
}

static struct orca_mempool_ops s_orca_mempool_ops = {
    .create_with_pool = tlsf_create_with_pool,
    .destroy = bredr_mempool_destroy,
    .malloc = tlsf_malloc,
    .free = tlsf_free,
};

#endif /* BREDR_MEMPOOL_TRACE */

struct orca_mempool_ops *bredr_mempool_ops_get(void)
{
    return &s_orca_mempool_ops;
}

#endif /* UC_BT_CTRL_BR_EDR_IS_ENABLE */
