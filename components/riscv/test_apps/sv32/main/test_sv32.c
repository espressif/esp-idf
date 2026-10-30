/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "riscv/sv32.h"
#include "riscv/encoding.h"
#include "soc/soc.h"
#include "unity.h"

#define TEST_VADDR                 UINT32_C(0x60001000)
#define TEST_WORD_OFFSET           UINT32_C(0x40)
#define TEST_MSTATUS_MPP_S         (PRV_S << 11)

static riscv_sv32_table_t s_root;
static riscv_sv32_table_t s_level;
static uint8_t s_backing_page[RISCV_SV32_PAGE_SIZE] __attribute__((aligned(RISCV_SV32_PAGE_SIZE)));
static portMUX_TYPE s_access_lock = portMUX_INITIALIZER_UNLOCKED;

static riscv_sv32_paddr_t object_paddr(const void *object)
{
    return (riscv_sv32_paddr_t)(uintptr_t)object;
}

static uint32_t *backing_word(void)
{
    return (uint32_t *)&s_backing_page[TEST_WORD_OFFSET];
}

static void assert_supported_page(const void *page)
{
    const riscv_sv32_paddr_t paddr = object_paddr(page);
    TEST_ASSERT_EQUAL_UINT32(0, paddr & (RISCV_SV32_PAGE_SIZE - 1));
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64(SOC_DRAM_LOW, paddr);
    TEST_ASSERT_LESS_OR_EQUAL_UINT64(SOC_DRAM_HIGH, paddr + RISCV_SV32_PAGE_SIZE);
}

static void reset_fixture(void)
{
    memset(&s_root, 0, sizeof(s_root));
    memset(&s_level, 0, sizeof(s_level));
    memset(s_backing_page, 0, sizeof(s_backing_page));

    assert_supported_page(&s_root);
    assert_supported_page(&s_level);
    assert_supported_page(s_backing_page);
    s_root.entries[RISCV_SV32_VPN1(TEST_VADDR)] =
        riscv_sv32_pte_make_table(object_paddr(&s_level));
}

static void map_backing_page(uint32_t permissions)
{
    s_level.entries[RISCV_SV32_VPN0(TEST_VADDR)] =
        riscv_sv32_pte_make_leaf(object_paddr(s_backing_page), permissions);
}

static uint32_t read_mstatus_for_test(void)
{
    uint32_t value;
    __asm__ volatile ("csrr %0, mstatus" : "=r" (value));
    return value;
}

/*
 * Perform one S-mode data access through the Sv32 root while staying in M-mode.
 *
 * With mstatus.MPRV set only loads and stores are translated, so the CSR write,
 * the access and the restore must be a single asm statement with no compiler
 * generated memory accesses in between. Interrupts stay masked for the window.
 * An unexpected page fault goes to the regular panic handler and fails the test.
 */
static void translated_access(bool store, uint32_t *value)
{
    const uint32_t vaddr = TEST_VADDR + TEST_WORD_OFFSET;

    portENTER_CRITICAL(&s_access_lock);
    const uint32_t saved = read_mstatus_for_test();
    const uint32_t access = (saved & ~(MSTATUS_MIE | MSTATUS_MPP | MSTATUS_SUM | MSTATUS_MXR)) |
                            MSTATUS_MPRV | TEST_MSTATUS_MPP_S;
    riscv_sv32_activate(object_paddr(&s_root));
    if (store) {
        __asm__ volatile ("csrw mstatus, %[access]\n"
                          "sw   %[value], 0(%[vaddr])\n"
                          "csrw mstatus, %[saved]\n"
                          :: [access] "r" (access), [saved] "r" (saved),
                          [vaddr] "r" (vaddr), [value] "r" (*value)
                          : "memory");
    } else {
        __asm__ volatile ("csrw mstatus, %[access]\n"
                          "lw   %[value], 0(%[vaddr])\n"
                          "csrw mstatus, %[saved]\n"
                          : [value] "=&r" (*value)
                          : [access] "r" (access), [saved] "r" (saved), [vaddr] "r" (vaddr)
                          : "memory");
    }
    riscv_sv32_deactivate();
    const uint32_t restored = read_mstatus_for_test();
    portEXIT_CRITICAL(&s_access_lock);

    TEST_ASSERT_BITS_LOW(MSTATUS_MPRV, saved);
    TEST_ASSERT_EQUAL_HEX32(saved, restored);
    TEST_ASSERT_EQUAL_HEX32(0, riscv_sv32_read_satp());
}

TEST_CASE("RISC-V Sv32 address helpers extract VPNs and align pages", "[riscv][sv32]")
{
    TEST_ASSERT_EQUAL_HEX32(TEST_VADDR, RISCV_SV32_PAGE_ALIGN_DOWN(TEST_VADDR + 0xabc));
    TEST_ASSERT_EQUAL_UINT32(0x180, RISCV_SV32_VPN1(TEST_VADDR));
    TEST_ASSERT_EQUAL_UINT32(1, RISCV_SV32_VPN0(TEST_VADDR));
}

TEST_CASE("RISC-V Sv32 entry helpers encode and decode entries", "[riscv][sv32]")
{
    riscv_sv32_pte_t entry = riscv_sv32_pte_make_table(UINT64_C(0x12345000));
    TEST_ASSERT_EQUAL_HEX32(UINT32_C(0x048d1401), entry);
    TEST_ASSERT_TRUE(riscv_sv32_pte_is_valid(entry));
    TEST_ASSERT_TRUE(riscv_sv32_pte_is_table(entry));
    TEST_ASSERT_FALSE(riscv_sv32_pte_is_leaf(entry));
    TEST_ASSERT_EQUAL_UINT64(UINT64_C(0x12345000), riscv_sv32_pte_get_paddr_base(entry));

    entry = riscv_sv32_pte_make_leaf(UINT64_C(0x3fffff000),
                                    RISCV_SV32_PTE_R | RISCV_SV32_PTE_W | RISCV_SV32_PTE_U);
    TEST_ASSERT_EQUAL_HEX32(UINT32_C(0xfffffcd7), entry);
    TEST_ASSERT_TRUE(riscv_sv32_pte_is_leaf(entry));
    TEST_ASSERT_EQUAL_UINT64(UINT64_C(0x3fffff000), riscv_sv32_pte_get_paddr_base(entry));
    TEST_ASSERT_EQUAL_HEX32(RISCV_SV32_PTE_R | RISCV_SV32_PTE_W | RISCV_SV32_PTE_U,
                            riscv_sv32_pte_get_permissions(entry));
    TEST_ASSERT_BITS_HIGH(RISCV_SV32_PTE_A | RISCV_SV32_PTE_D, entry);

    entry = riscv_sv32_pte_make_leaf(UINT64_C(0x2000), RISCV_SV32_PTE_X | RISCV_SV32_PTE_G);
    TEST_ASSERT_BITS_HIGH(RISCV_SV32_PTE_A, entry);
    TEST_ASSERT_BITS_LOW(RISCV_SV32_PTE_D, entry);

    riscv_sv32_pte_clear(&entry);
    TEST_ASSERT_EQUAL_HEX32(0, entry);
    TEST_ASSERT_FALSE(riscv_sv32_pte_is_valid(entry));
}

TEST_CASE("RISC-V Sv32 satp helpers encode ASIDs", "[riscv][sv32]")
{
    const riscv_sv32_paddr_t root_paddr = UINT64_C(0x12345000);
    const uint32_t asid = UINT32_C(0x155);
    const uint32_t satp = riscv_sv32_make_satp_with_asid(root_paddr, asid);

    TEST_ASSERT_EQUAL_UINT32(asid, (satp >> RISCV_SV32_ASID_SHIFT) & RISCV_SV32_ASID_MAX);
    TEST_ASSERT_EQUAL_HEX32(riscv_sv32_make_satp(root_paddr),
                            riscv_sv32_make_satp_with_asid(root_paddr, 0));
}

TEST_CASE("RISC-V Sv32 local invalidation forms execute", "[riscv][sv32][hardware]")
{
    const uint32_t satp_before = riscv_sv32_read_satp();

    riscv_sv32_invalidate_all();
    riscv_sv32_invalidate_asid(0);
    riscv_sv32_invalidate_vaddr(TEST_VADDR);
    riscv_sv32_invalidate_vaddr_asid(TEST_VADDR, 0);

    TEST_ASSERT_EQUAL_HEX32(satp_before, riscv_sv32_read_satp());
}

TEST_CASE("RISC-V Sv32 selects an ASID and restores Bare", "[riscv][sv32][hardware]")
{
    const riscv_sv32_paddr_t root_paddr = object_paddr(&s_root);
    const uint32_t asid = UINT32_C(0x155);
    TEST_ASSERT_GREATER_OR_EQUAL_UINT64(SOC_DRAM_LOW, root_paddr);
    TEST_ASSERT_LESS_OR_EQUAL_UINT64(SOC_DRAM_HIGH, root_paddr + sizeof(s_root));
    TEST_ASSERT_EQUAL_UINT32(0, root_paddr & (RISCV_SV32_TABLE_SIZE - 1));

    riscv_sv32_fence_pte_writes();
    riscv_sv32_activate_with_asid(root_paddr, asid);
    const uint32_t active_satp = riscv_sv32_read_satp();
    TEST_ASSERT_EQUAL_UINT32((active_satp >> RISCV_SV32_ASID_SHIFT) & RISCV_SV32_ASID_MAX,
                             riscv_sv32_read_asid());
    riscv_sv32_deactivate();
    riscv_sv32_invalidate_all();

    TEST_ASSERT_EQUAL_HEX32(riscv_sv32_make_satp(root_paddr), active_satp & ~SATP32_ASID);
    TEST_ASSERT_EQUAL_HEX32(0, riscv_sv32_read_satp());
    TEST_ASSERT_EQUAL_UINT32(0, riscv_sv32_read_asid());
}

TEST_CASE("RISC-V Sv32 translates a load and store", "[riscv][sv32][hardware]")
{
    reset_fixture();
    map_backing_page(RISCV_SV32_PTE_R | RISCV_SV32_PTE_W);
    riscv_sv32_fence_pte_writes();

    volatile uint32_t *word = backing_word();
    *word = UINT32_C(0x13579bdf);

    uint32_t value = 0;
    translated_access(false, &value);
    TEST_ASSERT_EQUAL_HEX32(UINT32_C(0x13579bdf), value);

    value = UINT32_C(0xa55a3cc3);
    translated_access(true, &value);
    TEST_ASSERT_EQUAL_HEX32(UINT32_C(0xa55a3cc3), *word);
}
