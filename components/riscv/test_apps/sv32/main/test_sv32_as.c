/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Page tables of riscv/sv32_as.h, read back with riscv_sv32_translate_checked(). Nothing here is
 * translated by the MMU; test_sv32.c checks that the hardware accepts the tables.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "esp_heap_caps.h"
#include "riscv/encoding.h"
#include "riscv/sv32.h"
#include "riscv/sv32_as.h"
#include "unity.h"

#define PAGE                RISCV_SV32_PAGE_SIZE
#define TABLE_SPAN          (UINT32_C(1) << 22)   /* Virtual addresses of one second-level table */
#define RW                  (RISCV_SV32_PTE_R | RISCV_SV32_PTE_W)

/* Tables allocated by a test, as a caller of the API does; s_tables[0] is the root */
static riscv_sv32_table_t *s_tables[8];
static size_t s_used;

static riscv_sv32_table_t *table_alloc(void)
{
    TEST_ASSERT_LESS_THAN(sizeof(s_tables) / sizeof(s_tables[0]), s_used);
    riscv_sv32_table_t *table = heap_caps_aligned_calloc(RISCV_SV32_TABLE_SIZE, 1, sizeof(riscv_sv32_table_t),
                                                         MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(table);
    s_tables[s_used++] = table;
    return table;
}

/* Free the tables of the test, once the address space is no longer used */
static void tables_free(void)
{
    while (s_used > 0) {
        heap_caps_free(s_tables[--s_used]);
    }
}

static riscv_sv32_as_t *new_as(void)
{
    riscv_sv32_as_t *as = riscv_sv32_as_init(table_alloc());
    TEST_ASSERT_NOT_NULL(as);
    return as;
}

/* Map with the tables the range needs, as a caller does */
static esp_err_t map(riscv_sv32_as_t *as, uint32_t vaddr, uint64_t paddr, size_t size, uint32_t permissions)
{
    riscv_sv32_table_t *tables[4];
    const size_t count = riscv_sv32_as_count_tables_needed(as, vaddr, size);
    TEST_ASSERT_LESS_OR_EQUAL(sizeof(tables) / sizeof(tables[0]), count);
    for (size_t i = 0; i < count; i++) {
        tables[i] = table_alloc();
    }
    return riscv_sv32_as_map(as, vaddr, paddr, size, permissions, tables, count);
}

static bool paddr_any(uint32_t paddr)
{
    (void)paddr;
    return true;
}

static uint32_t s_bad_paddr;

static bool paddr_not_bad(uint32_t paddr)
{
    return paddr / PAGE != s_bad_paddr / PAGE;
}

/* Translate through the address space as U-mode with the given permissions */
static bool translate(const riscv_sv32_as_t *as, uint32_t vaddr, uint32_t permissions, uint32_t *paddr)
{
    return riscv_sv32_translate_checked(riscv_sv32_as_satp(as), vaddr, permissions | RISCV_SV32_PTE_U,
                                        paddr_any, paddr);
}

static void assert_mapped(const riscv_sv32_as_t *as, uint32_t vaddr, uint32_t size, uint32_t paddr,
                          uint32_t permissions)
{
    for (uint32_t off = 0; off < size; off += PAGE) {
        uint32_t pa = 0;
        TEST_ASSERT_TRUE(translate(as, vaddr + off + 0x123, permissions, &pa));
        TEST_ASSERT_EQUAL_HEX32(paddr + off + 0x123, pa);
        /* Every permission the page lacks is refused */
        const uint32_t missing = RISCV_SV32_PTE_LEAF_MASK & ~permissions;
        for (uint32_t bit = RISCV_SV32_PTE_R; bit <= RISCV_SV32_PTE_X; bit <<= 1) {
            if (missing & bit) {
                TEST_ASSERT_FALSE(translate(as, vaddr + off, bit, &pa));
            }
        }
    }
}

static void assert_unmapped(const riscv_sv32_as_t *as, uint32_t vaddr, uint32_t size)
{
    for (uint32_t off = 0; off < size; off += PAGE) {
        uint32_t pa;
        TEST_ASSERT_FALSE(translate(as, vaddr + off, 0, &pa));
    }
}

TEST_CASE("RISC-V Sv32 address space maps a page", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_EQUAL(1, riscv_sv32_as_count_tables_needed(as, 0x60001000, PAGE));
    /* Mapping needs the table of the window */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x60001000, 0x40800000, PAGE, RW, NULL, 0));
    assert_unmapped(as, 0x60001000, PAGE);

    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0x60001000, 0x40800000, PAGE, RW));
    TEST_ASSERT_EQUAL(0, riscv_sv32_as_count_tables_needed(as, 0x603ff000, PAGE));
    assert_mapped(as, 0x60001000, PAGE, 0x40800000, RW);
    assert_unmapped(as, 0x60000000, PAGE);
    assert_unmapped(as, 0x60002000, PAGE);

    /* Leaves are user pages with A preset, and D preset when writable */
    const riscv_sv32_pte_t leaf = s_tables[1]->entries[RISCV_SV32_VPN0(0x60001000)];
    TEST_ASSERT_EQUAL_HEX32(RW | RISCV_SV32_PTE_U, riscv_sv32_pte_get_permissions(leaf));
    TEST_ASSERT_BITS_HIGH(RISCV_SV32_PTE_A | RISCV_SV32_PTE_D, leaf);
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space maps ranges across tables", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();

    /* Identity mapping across a 4 MiB boundary */
    const uint32_t ident = 0x40400000 - 2 * PAGE;
    TEST_ASSERT_EQUAL(ESP_OK, map(as, ident, ident, 4 * PAGE, RISCV_SV32_PTE_R | RISCV_SV32_PTE_X));
    assert_mapped(as, ident, 4 * PAGE, ident, RISCV_SV32_PTE_R | RISCV_SV32_PTE_X);
    TEST_ASSERT_EQUAL(3, s_used);

    /* A range spanning three tables at another address */
    const uint32_t va = 0x10000000 - PAGE;
    const uint32_t size = TABLE_SPAN + 2 * PAGE;
    TEST_ASSERT_EQUAL(ESP_OK, map(as, va, 0x48000000, size, RISCV_SV32_PTE_R));
    assert_mapped(as, va, size, 0x48000000, RISCV_SV32_PTE_R);
    TEST_ASSERT_EQUAL(6, s_used);
    assert_unmapped(as, va - PAGE, PAGE);
    assert_unmapped(as, va + size, PAGE);

    /* Top and bottom of the address space */
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0xfffff000, 0x3fffff000ULL, PAGE, RISCV_SV32_PTE_X));
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0, 0x1000, PAGE, RW));
    assert_mapped(as, 0, PAGE, 0x1000, RW);
    /* The physical address of 0xfffff000 is above 4 GiB, which the checked walk does not return */
    uint32_t pa;
    TEST_ASSERT_FALSE(translate(as, 0xfffff000, RISCV_SV32_PTE_X, &pa));
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space rejects invalid arguments", "[riscv][sv32]")
{
    TEST_ASSERT_NULL(riscv_sv32_as_init(NULL));
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_NULL(riscv_sv32_as_init((riscv_sv32_table_t *)((uint8_t *)s_tables[0] + 4)));
    /* A NULL, unaligned or repeated table, the root, no tables, or a count that is not the number
     * the range needs */
    riscv_sv32_table_t *const table[] = { table_alloc() };
    riscv_sv32_table_t *const null_table[] = { NULL };
    riscv_sv32_table_t *const unaligned[] = { (riscv_sv32_table_t *)((uint8_t *)table[0] + 4) };
    riscv_sv32_table_t *const root[] = { s_tables[0] };
    riscv_sv32_table_t *const repeated[] = { table[0], table[0] };
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, null_table, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, unaligned, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, root, 1));
    TEST_ASSERT_EQUAL(2, riscv_sv32_as_count_tables_needed(as, 0x3ff000, 2 * PAGE));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, 2 * PAGE, RW, repeated, 2));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, NULL, 1));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, table, 0));
    TEST_ASSERT_EQUAL(1, riscv_sv32_as_count_tables_needed(as, 0x3ff000, PAGE));
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_map(as, 0x3ff000, 0x2000, PAGE, RW, table, 1));
    /* A window has one table */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x1000, 0x2000, PAGE, RW, table, 1));

    /* Unaligned, empty or wrapping ranges; no rounding to pages */
    const struct {
        uint32_t vaddr;
        size_t size;
    } bad_ranges[] = {
        { 0x1004, PAGE },
        { 0x1000, PAGE + 4 },
        { 0x1000, 0 },
        { 0xfffff000, 2 * PAGE },
    };
    for (size_t i = 0; i < sizeof(bad_ranges) / sizeof(bad_ranges[0]); i++) {
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, bad_ranges[i].vaddr, 0x2000, bad_ranges[i].size, RW, NULL, 0));
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_unmap(as, bad_ranges[i].vaddr, bad_ranges[i].size));
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG,
                          riscv_sv32_as_protect(as, bad_ranges[i].vaddr, bad_ranges[i].size, RW));
    }
    /* No permission, W without R, and U, which the API sets itself */
    const uint32_t bad_permissions[] = { 0, RISCV_SV32_PTE_W, RISCV_SV32_PTE_R | RISCV_SV32_PTE_U };
    for (size_t i = 0; i < sizeof(bad_permissions) / sizeof(bad_permissions[0]); i++) {
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x1000, 0x2000, PAGE, bad_permissions[i], NULL, 0));
        TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_protect(as, 0x1000, PAGE, bad_permissions[i]));
    }
    /* Unaligned physical address, and physical range beyond 34 bits */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x1000, 0x2004, PAGE, RW, NULL, 0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x1000, 0x3fffff000ULL, 2 * PAGE, RW, NULL, 0));
    assert_unmapped(as, 0, 4 * PAGE);

    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(NULL, 0x1000, 0x2000, PAGE, RW, NULL, 0));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_unmap(NULL, 0x1000, PAGE));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_protect(NULL, 0x1000, PAGE, RW));
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space changes nothing when a map fails", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0x00402000, 0x40800000, PAGE, RW));

    /* The overlap is in the last page, after a window that needs a table */
    riscv_sv32_table_t *const tables[] = { table_alloc() };
    memset(tables[0], 0xa5, sizeof(*tables[0]));
    TEST_ASSERT_EQUAL(1, riscv_sv32_as_count_tables_needed(as, 0x003fe000, 5 * PAGE));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, riscv_sv32_as_map(as, 0x003fe000, 0x40900000, 5 * PAGE, RW, tables, 1));
    /* The table was neither added nor written */
    TEST_ASSERT_EQUAL(1, riscv_sv32_as_count_tables_needed(as, 0x003fe000, 5 * PAGE));
    TEST_ASSERT_EACH_EQUAL_HEX8(0xa5, tables[0], sizeof(*tables[0]));
    assert_unmapped(as, 0x003fe000, 4 * PAGE);
    assert_mapped(as, 0x00402000, PAGE, 0x40800000, RW);

    /* The last window of the range has no table */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, 0x007ff000, 0x40900000, 2 * PAGE, RW, NULL, 0));
    assert_unmapped(as, 0x007ff000, PAGE);
    /* The table that the failed calls did not take maps the range */
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_map(as, 0x007ff000, 0x40900000, 2 * PAGE, RW, tables, 1));
    assert_mapped(as, 0x007ff000, 2 * PAGE, 0x40900000, RW);
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space unmaps ranges and keeps the tables", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    const uint32_t va = TABLE_SPAN - 2 * PAGE;
    TEST_ASSERT_EQUAL(ESP_OK, map(as, va, 0x40800000, 4 * PAGE, RW));

    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, va, PAGE));
    assert_unmapped(as, va, PAGE);
    assert_mapped(as, va + PAGE, 3 * PAGE, 0x40801000, RW);

    /* Holes are skipped, and emptied tables stay */
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, va, 2 * PAGE));
    TEST_ASSERT_EQUAL(0, riscv_sv32_as_count_tables_needed(as, va, PAGE));
    assert_mapped(as, TABLE_SPAN, 2 * PAGE, 0x40802000, RW);

    /* A large range over windows without tables */
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, 0, 0x80000000));
    assert_unmapped(as, va, 4 * PAGE);

    /* The pages can be mapped again, in the same tables */
    TEST_ASSERT_EQUAL(ESP_OK, map(as, va, 0x40900000, 4 * PAGE, RISCV_SV32_PTE_R));
    assert_mapped(as, va, 4 * PAGE, 0x40900000, RISCV_SV32_PTE_R);
    TEST_ASSERT_EQUAL(3, s_used);
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, 0xfffff000, PAGE));
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, 0, 0xfffff000));
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space changes the permissions of a range only", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0x10000, 0x40800000, 4 * PAGE, RW));

    /* A loader writes code, then makes it executable and read-only */
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_protect(as, 0x11000, 2 * PAGE, RISCV_SV32_PTE_R | RISCV_SV32_PTE_X));
    assert_mapped(as, 0x10000, PAGE, 0x40800000, RW);
    assert_mapped(as, 0x11000, 2 * PAGE, 0x40801000, RISCV_SV32_PTE_R | RISCV_SV32_PTE_X);
    assert_mapped(as, 0x13000, PAGE, 0x40803000, RW);

    /* Read-only leaves lose D; writable leaves get it back */
    const riscv_sv32_table_t *l0 = s_tables[1];
    TEST_ASSERT_BITS_LOW(RISCV_SV32_PTE_D, l0->entries[RISCV_SV32_VPN0(0x11000)]);
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_protect(as, 0x11000, PAGE, RW));
    TEST_ASSERT_BITS_HIGH(RISCV_SV32_PTE_D | RISCV_SV32_PTE_U, l0->entries[RISCV_SV32_VPN0(0x11000)]);

    /* A range with an unmapped page is refused as a whole */
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, riscv_sv32_as_protect(as, 0x12000, 3 * PAGE, RISCV_SV32_PTE_R));
    assert_mapped(as, 0x12000, PAGE, 0x40802000, RISCV_SV32_PTE_R | RISCV_SV32_PTE_X);
    assert_mapped(as, 0x13000, PAGE, 0x40803000, RW);
    TEST_ASSERT_EQUAL(ESP_ERR_NOT_FOUND, riscv_sv32_as_protect(as, 0x800000, PAGE, RISCV_SV32_PTE_R));
    tables_free();
}

TEST_CASE("RISC-V Sv32 checked walk checks each address it reads", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0x5000, 0x40805000, PAGE, RW));
    const uint32_t satp = riscv_sv32_as_satp(as);
    riscv_sv32_table_t *l0 = s_tables[1];
    uint32_t pa;

    s_bad_paddr = 0;
    TEST_ASSERT_TRUE(riscv_sv32_translate_checked(satp, 0x5010, RISCV_SV32_PTE_U, paddr_not_bad, &pa));
    TEST_ASSERT_EQUAL_HEX32(0x40805010, pa);
    /* The root table, the second-level table and the page are each checked */
    const uint32_t bad[] = { (uint32_t)(uintptr_t)s_tables[0], (uint32_t)(uintptr_t)l0, 0x40805000 };
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
        s_bad_paddr = bad[i];
        TEST_ASSERT_FALSE(riscv_sv32_translate_checked(satp, 0x5010, RISCV_SV32_PTE_U, paddr_not_bad, &pa));
    }
    /* Bare mode is not translated */
    TEST_ASSERT_FALSE(riscv_sv32_translate_checked(satp & ~SATP32_MODE, 0x5010, 0, paddr_any, &pa));
    /* The reserved encoding W without R is treated as unmapped */
    l0->entries[6] = riscv_sv32_pte_make_leaf(0x40806000, RW | RISCV_SV32_PTE_U) & ~RISCV_SV32_PTE_R;
    TEST_ASSERT_FALSE(riscv_sv32_translate_checked(satp, 0x6000, RISCV_SV32_PTE_U | RISCV_SV32_PTE_W, paddr_any, &pa));
    riscv_sv32_pte_clear(&l0->entries[6]);
    /* A leaf at level 1 (a 4 MiB page) is treated as unmapped, and map() refuses to cover it */
    s_tables[0]->entries[1] = riscv_sv32_pte_make_leaf(0x40c00000, RISCV_SV32_PTE_R | RISCV_SV32_PTE_U);
    TEST_ASSERT_FALSE(riscv_sv32_translate_checked(satp, TABLE_SPAN, 0, paddr_any, &pa));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, riscv_sv32_as_map(as, TABLE_SPAN + PAGE, 0x40900000, PAGE, RW, NULL, 0));
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space takes the missing tables of a range", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    /* A range over four windows, of which the second one already has a table */
    const uint32_t va = 2 * TABLE_SPAN - PAGE;
    const size_t size = 2 * TABLE_SPAN + 2 * PAGE;
    TEST_ASSERT_EQUAL(4, riscv_sv32_as_count_tables_needed(as, va, size));
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 2 * TABLE_SPAN + 5 * PAGE, 0x40800000, PAGE, RW));
    TEST_ASSERT_EQUAL(3, riscv_sv32_as_count_tables_needed(as, va, size));
    TEST_ASSERT_EQUAL(1, riscv_sv32_as_count_tables_needed(as, va, PAGE));
    TEST_ASSERT_EQUAL(0, riscv_sv32_as_count_tables_needed(as, 2 * TABLE_SPAN, PAGE));

    riscv_sv32_table_t *const tables[3] = { table_alloc(), table_alloc(), table_alloc() };
    /* Too few or too many tables */
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, va, 0x48000000, size, RISCV_SV32_PTE_R, tables, 2));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_ARG, riscv_sv32_as_map(as, va, 0x48000000, size, RISCV_SV32_PTE_R, tables, 4));
    TEST_ASSERT_EQUAL(3, riscv_sv32_as_count_tables_needed(as, va, size));

    /* The tables go to the windows without one, in order of address, and are zeroed */
    memset(tables[0], 0xa5, sizeof(*tables[0]));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, riscv_sv32_as_map(as, va, 0x48000000, size, RISCV_SV32_PTE_R, tables, 3));
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_unmap(as, 2 * TABLE_SPAN + 5 * PAGE, PAGE));
    TEST_ASSERT_EQUAL(ESP_OK, riscv_sv32_as_map(as, va, 0x48000000, size, RISCV_SV32_PTE_R, tables, 3));
    TEST_ASSERT_EQUAL(0, riscv_sv32_as_count_tables_needed(as, va, size));
    const uint32_t windows[3] = { 1, 3, 4 };
    for (size_t i = 0; i < 3; i++) {
        const riscv_sv32_pte_t l1 = s_tables[0]->entries[windows[i]];
        TEST_ASSERT_TRUE(riscv_sv32_pte_is_table(l1));
        TEST_ASSERT_EQUAL_HEX32((uintptr_t)tables[i], (uint32_t)riscv_sv32_pte_get_paddr_base(l1));
    }
    assert_mapped(as, va, size, 0x48000000, RISCV_SV32_PTE_R);
    /* The rest of the first window, whose table was filled before the call */
    assert_unmapped(as, TABLE_SPAN, 8 * PAGE);
    tables_free();
}

TEST_CASE("RISC-V Sv32 address space gives its tables back", "[riscv][sv32]")
{
    riscv_sv32_as_t *as = new_as();
    TEST_ASSERT_NULL(riscv_sv32_as_detach_table(as));
    TEST_ASSERT_EQUAL(ESP_OK, map(as, TABLE_SPAN - PAGE, 0x40800000, 2 * PAGE, RW));
    TEST_ASSERT_EQUAL(ESP_OK, map(as, 0xfffff000, 0x40900000, PAGE, RW));
    TEST_ASSERT_EQUAL(4, s_used);

    /* Each table is returned once */
    bool detached[4] = { false };
    for (size_t n = 0; n < 3; n++) {
        riscv_sv32_table_t *table = riscv_sv32_as_detach_table(as);
        TEST_ASSERT_NOT_NULL(table);
        size_t i = 1;
        while (i < s_used && s_tables[i] != table) {
            i++;
        }
        TEST_ASSERT_LESS_THAN(s_used, i);
        TEST_ASSERT_FALSE(detached[i]);
        detached[i] = true;
    }
    TEST_ASSERT_NULL(riscv_sv32_as_detach_table(as));

    /* The address space is empty, and can be used again */
    assert_unmapped(as, TABLE_SPAN - PAGE, 2 * PAGE);
    assert_unmapped(as, 0xfffff000, PAGE);
    TEST_ASSERT_EQUAL(RISCV_SV32_PTE_COUNT, riscv_sv32_as_count_tables_needed(as, 0, 0xfffff000));
    TEST_ASSERT_EQUAL(2, riscv_sv32_as_count_tables_needed(as, TABLE_SPAN - PAGE, 2 * PAGE));
    TEST_ASSERT_EQUAL(ESP_OK, map(as, TABLE_SPAN - PAGE, 0x40800000, 2 * PAGE, RW));
    assert_mapped(as, TABLE_SPAN - PAGE, 2 * PAGE, 0x40800000, RW);
    tables_free();
}
