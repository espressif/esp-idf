/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdint.h>
#include "riscv/encoding.h"
#include "riscv/sv32.h"

uint32_t riscv_sv32_read_satp(void)
{
    uint32_t value;
    __asm__ volatile ("csrr %0, satp" : "=r" (value));
    return value;
}

uint32_t riscv_sv32_read_asid(void)
{
    return (riscv_sv32_read_satp() >> RISCV_SV32_ASID_SHIFT) & RISCV_SV32_ASID_MAX;
}

uint32_t riscv_sv32_make_satp(riscv_sv32_paddr_t root_paddr)
{
    return riscv_sv32_make_satp_with_asid(root_paddr, 0);
}

uint32_t riscv_sv32_make_satp_with_asid(riscv_sv32_paddr_t root_paddr, uint32_t asid)
{
    assert((root_paddr & (RISCV_SV32_PAGE_SIZE - 1U)) == 0);
    assert(root_paddr < RISCV_SV32_PADDR_LIMIT);
    assert(asid <= RISCV_SV32_ASID_MAX);
    return SATP32_MODE | (asid << RISCV_SV32_ASID_SHIFT) |
           (uint32_t)(root_paddr >> RISCV_SV32_PAGE_SHIFT);
}

void riscv_sv32_fence_pte_writes(void)
{
    __asm__ volatile ("fence rw, rw" ::: "memory");
}

void riscv_sv32_activate_with_asid(riscv_sv32_paddr_t root_paddr, uint32_t asid)
{
    const uint32_t satp = riscv_sv32_make_satp_with_asid(root_paddr, asid);
    /*
     * The data fence orders earlier page-table writes; the SFENCE.VMA after the
     * satp write then orders them against this hart's implicit walker accesses
     * and drops any stale translations.
     */
    riscv_sv32_fence_pte_writes();
    __asm__ volatile ("csrw satp, %0" :: "rK" (satp) : "memory");
    __asm__ volatile ("sfence.vma x0, x0" ::: "memory");
}

void riscv_sv32_activate(riscv_sv32_paddr_t root_paddr)
{
    riscv_sv32_activate_with_asid(root_paddr, 0);
}

void riscv_sv32_deactivate(void)
{
    __asm__ volatile ("csrw satp, zero" ::: "memory");
    __asm__ volatile ("sfence.vma x0, x0" ::: "memory");
}

void riscv_sv32_invalidate_all(void)
{
    __asm__ volatile ("sfence.vma x0, x0" ::: "memory");
}

void riscv_sv32_invalidate_asid(uint32_t asid)
{
    assert(asid <= RISCV_SV32_ASID_MAX);
    /* rs2 is a register even when its value (and the requested ASID) is zero. */
    __asm__ volatile ("sfence.vma x0, %0" :: "r" (asid) : "memory");
}

void riscv_sv32_invalidate_vaddr(uint32_t vaddr)
{
    /* rs1 is a register even when the requested virtual address is zero. */
    __asm__ volatile ("sfence.vma %0, x0" :: "r" (vaddr) : "memory");
}

void riscv_sv32_invalidate_vaddr_asid(uint32_t vaddr, uint32_t asid)
{
    assert(asid <= RISCV_SV32_ASID_MAX);
    __asm__ volatile ("sfence.vma %0, %1" :: "r" (vaddr), "r" (asid) : "memory");
}
