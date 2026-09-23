/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Sv32 address and CSR layouts:
 *
 * Virtual address (32 bits):
 *   | VPN[1] | VPN[0] | page offset |
 *    31    22 21    12 11          0
 *
 * Physical address (34 bits):
 *   | PPN[1] | PPN[0] | page offset |
 *    33    22 21    12 11          0
 *
 * satp (RV32):
 *   | MODE | ASID | PPN |
 *    31     30  22 21  0
 *
 * Page-table entry (32 bits):
 *   | PPN[1] | PPN[0] | RSW | D | A | G | U | X | W | R | V |
 *    31    20 19    10  9 8   7   6   5   4   3   2   1   0
 */

/** Physical address encoded by an Sv32 page-table entry. */
typedef uint64_t riscv_sv32_paddr_t;

/** Raw Sv32 page-table entry. */
typedef uint32_t riscv_sv32_pte_t;

#define RISCV_SV32_PAGE_SHIFT            12U
#define RISCV_SV32_PAGE_SIZE             (1U << RISCV_SV32_PAGE_SHIFT)
#define RISCV_SV32_PTE_COUNT             1024U
#define RISCV_SV32_TABLE_SIZE            RISCV_SV32_PAGE_SIZE
#define RISCV_SV32_PADDR_BITS            34U
#define RISCV_SV32_PADDR_LIMIT           (UINT64_C(1) << RISCV_SV32_PADDR_BITS)
#define RISCV_SV32_ASID_BITS             9U
#define RISCV_SV32_ASID_SHIFT            22U
#define RISCV_SV32_ASID_MAX              ((1U << RISCV_SV32_ASID_BITS) - 1U)

/** Extract an Sv32 level-1 or level-0 virtual-page index. */
#define RISCV_SV32_VPN1(vaddr)           (((uint32_t)(vaddr) >> 22U) & (RISCV_SV32_PTE_COUNT - 1U))
#define RISCV_SV32_VPN0(vaddr)           (((uint32_t)(vaddr) >> RISCV_SV32_PAGE_SHIFT) & \
                                          (RISCV_SV32_PTE_COUNT - 1U))

/** Align a 32-bit virtual address down to an Sv32 page boundary. */
#define RISCV_SV32_PAGE_ALIGN_DOWN(vaddr) ((uint32_t)(vaddr) & ~(RISCV_SV32_PAGE_SIZE - 1U))

enum {
    RISCV_SV32_PTE_V = (1U << 0), /**< Valid: entry may be used by the walker */
    RISCV_SV32_PTE_R = (1U << 1), /**< Readable leaf */
    RISCV_SV32_PTE_W = (1U << 2), /**< Writable leaf (requires R) */
    RISCV_SV32_PTE_X = (1U << 3), /**< Executable leaf */
    RISCV_SV32_PTE_U = (1U << 4), /**< Accessible from U-mode */
    RISCV_SV32_PTE_G = (1U << 5), /**< Global: mapping exists in all ASIDs */
    RISCV_SV32_PTE_A = (1U << 6), /**< Accessed since A was last cleared */
    RISCV_SV32_PTE_D = (1U << 7), /**< Dirty: written since D was last cleared */
};
/* R, W and X all clear in a valid PTE marks a pointer to the next-level table. */

#define RISCV_SV32_PTE_PERMISSION_MASK   (RISCV_SV32_PTE_R | RISCV_SV32_PTE_W | \
                                          RISCV_SV32_PTE_X | RISCV_SV32_PTE_U | \
                                          RISCV_SV32_PTE_G)
#define RISCV_SV32_PTE_LEAF_MASK         (RISCV_SV32_PTE_R | RISCV_SV32_PTE_W | \
                                          RISCV_SV32_PTE_X)
#define RISCV_SV32_PTE_PPN_SHIFT         10U
#define RISCV_SV32_PTE_PPN_MASK          UINT32_C(0xfffffc00)

typedef struct __attribute__((aligned(RISCV_SV32_TABLE_SIZE))) {
    riscv_sv32_pte_t entries[RISCV_SV32_PTE_COUNT];
} riscv_sv32_table_t;

/*
 * This API deliberately exposes only architectural Sv32 mechanisms. Trusted
 * callers own the platform policy: table placement in walker-visible memory,
 * PMP/PMA permissions for page-table walks and mapped accesses, and exclusion
 * or shootdown when a root can be active on another core.
 */

ESP_STATIC_ASSERT(sizeof(riscv_sv32_table_t) == RISCV_SV32_TABLE_SIZE,
                  "Sv32 table must occupy one page");

static inline bool riscv_sv32_pte_is_valid(riscv_sv32_pte_t pte)
{
    return (pte & RISCV_SV32_PTE_V) != 0;
}

static inline bool riscv_sv32_pte_is_leaf(riscv_sv32_pte_t pte)
{
    return riscv_sv32_pte_is_valid(pte) && (pte & RISCV_SV32_PTE_LEAF_MASK) != 0;
}

static inline bool riscv_sv32_pte_is_table(riscv_sv32_pte_t pte)
{
    return riscv_sv32_pte_is_valid(pte) && (pte & RISCV_SV32_PTE_LEAF_MASK) == 0;
}

/** Return the page-aligned physical base address of the page or table a PTE points to. */
static inline riscv_sv32_paddr_t riscv_sv32_pte_get_paddr_base(riscv_sv32_pte_t pte)
{
    return ((riscv_sv32_paddr_t)((pte & RISCV_SV32_PTE_PPN_MASK) >> RISCV_SV32_PTE_PPN_SHIFT))
           << RISCV_SV32_PAGE_SHIFT;
}

static inline uint32_t riscv_sv32_pte_get_permissions(riscv_sv32_pte_t pte)
{
    return pte & RISCV_SV32_PTE_PERMISSION_MASK;
}

/** Construct a valid non-leaf PTE for an aligned, Sv32-representable table. */
static inline riscv_sv32_pte_t riscv_sv32_pte_make_table(riscv_sv32_paddr_t child_table_paddr)
{
    assert((child_table_paddr & (RISCV_SV32_PAGE_SIZE - 1U)) == 0);
    assert(child_table_paddr < RISCV_SV32_PADDR_LIMIT);
    return (riscv_sv32_pte_t)((child_table_paddr >> RISCV_SV32_PAGE_SHIFT) << RISCV_SV32_PTE_PPN_SHIFT)
           | RISCV_SV32_PTE_V;
}

/**
 * Construct a valid 4 KiB leaf PTE.
 *
 * Accessed is preset, as is Dirty for writable leaves. Write without read is
 * rejected because Sv32 reserves that encoding.
 */
static inline riscv_sv32_pte_t riscv_sv32_pte_make_leaf(riscv_sv32_paddr_t paddr,
                                                        uint32_t permissions)
{
    assert((paddr & (RISCV_SV32_PAGE_SIZE - 1U)) == 0);
    assert(paddr < RISCV_SV32_PADDR_LIMIT);
    assert((permissions & ~RISCV_SV32_PTE_PERMISSION_MASK) == 0);
    assert((permissions & RISCV_SV32_PTE_LEAF_MASK) != 0);
    assert((permissions & RISCV_SV32_PTE_W) == 0 || (permissions & RISCV_SV32_PTE_R) != 0);

    uint32_t preset = RISCV_SV32_PTE_V | RISCV_SV32_PTE_A;
    if ((permissions & RISCV_SV32_PTE_W) != 0) {
        preset |= RISCV_SV32_PTE_D;
    }
    return (riscv_sv32_pte_t)((paddr >> RISCV_SV32_PAGE_SHIFT) << RISCV_SV32_PTE_PPN_SHIFT)
           | permissions | preset;
}

static inline void riscv_sv32_pte_clear(riscv_sv32_pte_t *pte)
{
    assert(pte != NULL);
    *pte = 0;
}

/** Read the raw RV32 satp CSR. */
uint32_t riscv_sv32_read_satp(void);

/** Read the ASID encoded in the RV32 satp CSR. */
uint32_t riscv_sv32_read_asid(void);

/* Assembly callers can write a pre-encoded satp value with riscv/sv32_asm.h. */

/** Encode Sv32 mode, ASID zero, and an aligned root-table address. */
uint32_t riscv_sv32_make_satp(riscv_sv32_paddr_t root_paddr);

/**
 * Encode Sv32 mode, an architectural 9-bit ASID, and an aligned root address.
 *
 * A hart may implement fewer than nine ASID bits, including none. Platform
 * code must discover or document the implemented ASID width before assigning
 * nonzero ASIDs.
 */
uint32_t riscv_sv32_make_satp_with_asid(riscv_sv32_paddr_t root_paddr, uint32_t asid);

/**
 * Order prior page-table writes before subsequent memory accesses (FENCE RW, RW),
 * e.g. before a remote-hart shootdown protocol continues.
 *
 * This does not invalidate local translations; use one of the invalidate
 * helpers for that. It is not a cache writeback operation either: the trusted
 * caller must ensure that its chosen table memory is visible to the hardware
 * walker.
 */
void riscv_sv32_fence_pte_writes(void);

/** Select an Sv32 root and ASID, then invalidate all translations on this hart. */
void riscv_sv32_activate_with_asid(riscv_sv32_paddr_t root_paddr, uint32_t asid);

/** Select an Sv32, ASID-zero root and invalidate all translations on this hart. */
void riscv_sv32_activate(riscv_sv32_paddr_t root_paddr);

/** Select Bare translation and invalidate all local translations. */
void riscv_sv32_deactivate(void);

/** Invalidate all translations on this hart, including global entries. */
void riscv_sv32_invalidate_all(void);

/** Invalidate non-global translations for one ASID on this hart. */
void riscv_sv32_invalidate_asid(uint32_t asid);

/** Invalidate translations for one virtual address across all ASIDs on this hart. */
void riscv_sv32_invalidate_vaddr(uint32_t vaddr);

/** Invalidate a non-global translation for one virtual address and ASID on this hart. */
void riscv_sv32_invalidate_vaddr_asid(uint32_t vaddr, uint32_t asid);

#ifdef __cplusplus
}
#endif
