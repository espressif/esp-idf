/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#include "soc/soc_caps.h"
#include "soc/dport_access.h"
#include "soc/system_reg.h"
#include "esp_bit_defs.h"
#include "soc/assist_debug_reg.h"
#include "esp_attr.h"
#include "riscv/csr.h"
#include "riscv/semihosting.h"

/*performance counter*/
#define CSR_PCER_MACHINE    0x7e0
#define CSR_PCMR_MACHINE    0x7e1
#define CSR_PCCR_MACHINE    0x7e2

/*fast gpio*/
#define CSR_GPIO_OEN_USER   0x803
#define CSR_GPIO_IN_USER    0x804
#define CSR_GPIO_OUT_USER   0x805

#ifdef __cplusplus
extern "C" {
#endif

static inline int IRAM_ATTR cpu_ll_get_core_id(void)
{
#if SOC_CPU_CORES_NUM == 1
    return 0; // No need to check core ID on single core hardware
#else
    int cpuid;
    cpuid = RV_READ_CSR(mhartid);
    return cpuid;
#endif
}

static inline void cpu_ll_enable_cycle_count(void)
{
    RV_WRITE_CSR(CSR_PCER_MACHINE,1);
    RV_WRITE_CSR(CSR_PCMR_MACHINE,1);
    return;
}

static inline uint32_t IRAM_ATTR cpu_ll_get_cycle_count(void)
{
    uint32_t result;
    result = RV_READ_CSR(CSR_PCCR_MACHINE);
    return result;
}

static inline void IRAM_ATTR cpu_ll_set_cycle_count(uint32_t val)
{
    RV_WRITE_CSR(CSR_PCCR_MACHINE, val);
}

static inline void* cpu_ll_get_sp(void)
{
    void *sp;
    asm volatile ("mv %0, sp;" : "=r" (sp));
    return sp;
}

static inline void cpu_ll_init_hwloop(void)
{
    // Nothing needed here for ESP32-C3
}

FORCE_INLINE_ATTR bool cpu_ll_is_debugger_attached(void)
{
    return REG_GET_BIT(ASSIST_DEBUG_CORE_0_DEBUG_MODE_REG, ASSIST_DEBUG_CORE_0_DEBUG_MODULE_ACTIVE);
}

static inline void cpu_ll_set_breakpoint(int id, uint32_t pc)
{
    if (cpu_ll_is_debugger_attached()) {
        /* If we want to set breakpoint which when hit transfers control to debugger
         * we need to set `action` in `mcontrol` to 1 (Enter Debug Mode).
         * That `action` value is supported only when `dmode` of `tdata1` is set.
         * But `dmode` can be modified by debugger only (from Debug Mode).
         *
         * So when debugger is connected we use special syscall to ask it to set breakpoint for us.
         */
        long args[] = {true, id, (long)pc};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_BREAKPOINT_SET, args);
        if (ret == 0) {
            return;
        }
    }
    /* The code bellow sets breakpoint which will trigger `Breakpoint` exception
     * instead transfering control to debugger. */
    RV_WRITE_CSR(tselect,id);
    RV_SET_CSR(CSR_TCONTROL,TCONTROL_MTE);
    RV_SET_CSR(CSR_TDATA1, TDATA1_USER|TDATA1_MACHINE|TDATA1_EXECUTE);
    RV_WRITE_CSR(tdata2,pc);
    return;
}

static inline void cpu_ll_clear_breakpoint(int id)
{
    if (cpu_ll_is_debugger_attached()) {
        /* see description in cpu_ll_set_breakpoint()  */
        long args[] = {false, id};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_BREAKPOINT_SET, args);
        if (ret == 0){
            return;
        }
    }
    RV_WRITE_CSR(tselect,id);
    RV_CLEAR_CSR(CSR_TCONTROL,TCONTROL_MTE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_USER|TDATA1_MACHINE|TDATA1_EXECUTE);
    return;
}

static inline uint32_t cpu_ll_ptr_to_pc(const void* addr)
{
    return ((uint32_t) addr);
}

static inline void* cpu_ll_pc_to_ptr(uint32_t pc)
{
    return (void*) ((pc & 0x3fffffff) | 0x40000000);
}

static inline void cpu_ll_set_watchpoint(int id,
                                        const void* addr,
                                        size_t size,
                                        bool on_read,
                                        bool on_write)
{
    uint32_t addr_napot;

    if (cpu_ll_is_debugger_attached()) {
        /* see description in cpu_ll_set_breakpoint()  */
        long args[] = {true, id, (long)addr, (long)size,
            (long)((on_read ? ESP_SEMIHOSTING_WP_FLG_RD : 0) | (on_write ? ESP_SEMIHOSTING_WP_FLG_WR : 0))};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_WATCHPOINT_SET, args);
        if (ret == 0) {
            return;
        }
    }

    RV_WRITE_CSR(tselect,id);
    RV_SET_CSR(CSR_TCONTROL, TCONTROL_MPTE | TCONTROL_MTE);
    RV_SET_CSR(CSR_TDATA1, TDATA1_USER|TDATA1_MACHINE);
    RV_SET_CSR_FIELD(CSR_TDATA1, TDATA1_MATCH, 1);
    // add 0 in napot encoding
    addr_napot = ((uint32_t) addr) | ((size >> 1) - 1);
    if (on_read) {
        RV_SET_CSR(CSR_TDATA1, TDATA1_LOAD);
    }
    if (on_write) {
        RV_SET_CSR(CSR_TDATA1, TDATA1_STORE);
    }
    RV_WRITE_CSR(tdata2,addr_napot);
   return;
}

static inline void cpu_ll_clear_watchpoint(int id)
{
    if (cpu_ll_is_debugger_attached()) {
        /* see description in cpu_ll_set_breakpoint()  */
        long args[] = {false, id};
        int ret = semihosting_call_noerrno(ESP_SEMIHOSTING_SYS_WATCHPOINT_SET, args);
        if (ret == 0){
            return;
        }
    }
    RV_WRITE_CSR(tselect,id);
    RV_CLEAR_CSR(CSR_TCONTROL,TCONTROL_MTE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_USER|TDATA1_MACHINE);
    RV_CLEAR_CSR_FIELD(CSR_TDATA1,TDATA1_MATCH);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_MACHINE);
    RV_CLEAR_CSR(CSR_TDATA1, TDATA1_LOAD|TDATA1_STORE|TDATA1_EXECUTE);
    return;
}

static inline void cpu_ll_break(void)
{
    asm volatile("ebreak\n");
    return;
}

static inline void cpu_ll_set_vecbase(const void* vecbase)
{
    uintptr_t vecbase_int = (uintptr_t)vecbase;
    vecbase_int |= 1; // Set MODE field to treat MTVEC as a vector base address
    RV_WRITE_CSR(mtvec, vecbase_int);
}

static inline void cpu_ll_waiti(void)
{
    if (cpu_ll_is_debugger_attached() && DPORT_REG_GET_BIT(SYSTEM_CPU_PER_CONF_REG, SYSTEM_CPU_WAIT_MODE_FORCE_ON) == 0) {
        /* when SYSTEM_CPU_WAIT_MODE_FORCE_ON is disabled in WFI mode SBA access to memory does not work for debugger,
           so do not enter that mode when debugger is connected */
        return;
    }
    asm volatile ("wfi\n");
}

static inline void cpu_ll_enable_dedic_gpio_output(uint32_t mask)
{
    RV_WRITE_CSR(CSR_GPIO_OEN_USER, mask);
}

static inline void cpu_ll_write_dedic_gpio_all(uint32_t value)
{
    RV_WRITE_CSR(CSR_GPIO_OUT_USER, value);
}

static inline uint32_t cpu_ll_read_dedic_gpio_in(void)
{
    uint32_t value = RV_READ_CSR(CSR_GPIO_IN_USER);
    return value;
}

static inline uint32_t cpu_ll_read_dedic_gpio_out(void)
{
    uint32_t value = RV_READ_CSR(CSR_GPIO_OUT_USER);
    return value;
}

static inline void cpu_ll_write_dedic_gpio_mask(uint32_t mask, uint32_t value)
{
    RV_SET_CSR(CSR_GPIO_OUT_USER, mask & value);
    RV_CLEAR_CSR(CSR_GPIO_OUT_USER, mask & ~(value));
}

#ifdef __cplusplus
}
#endif
