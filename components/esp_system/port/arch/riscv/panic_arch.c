/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "spi_flash_mmap.h"

#include "soc/extmem_reg.h"
#include "esp_private/panic_internal.h"
#include "esp_private/panic_reason.h"
#include "riscv/rvruntime-frames.h"
#include "esp_private/cache_err_int.h"
#include "soc/timer_periph.h"

#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#include "esp_private/esp_memprot_internal.h"
#include "esp_memprot.h"
#endif

#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
#include "esp_private/eh_frame_parser.h"
#include "esp_private/cache_utils.h"
#endif


#define DIM(array) (sizeof(array)/sizeof(*array))

/**
 * Structure used to define a flag/bit to test in case of cache error.
 * The message describes the cause of the error when the bit is set in
 * a given status register.
 */
typedef struct {
    const uint32_t bit;
    const char *msg;
} register_bit_t;

/**
 * Function to check each bits defined in the array reg_bits in the given
 * status register. The first bit from the array to be set in the status
 * register will have its associated message printed. This function returns
 * true. If not bit was set in the register, it returns false.
 * The order of the bits in the array is important as only the first bit to
 * be set in the register will have its associated message printed.
 */
static inline bool test_and_print_register_bits(const uint32_t status,
        const register_bit_t *reg_bits,
        const uint32_t size)
{
    /* Browse the flag/bit array and test each one with the given status
     * register. */
    for (int i = 0; i < size; i++) {
        const uint32_t bit = reg_bits[i].bit;
        if ((status & bit) == bit) {
            /* Reason of the panic found, print the reason. */
            panic_print_str(reg_bits[i].msg);
            panic_print_str("\r\n");

            return true;
        }
    }

    /* Panic cause not found, no message was printed. */
    return false;
}

/**
 * Function called when a cache error occurs. It prints details such as the
 * explanation of why the panic occured.
 */
static inline void print_cache_err_details(const void *frame)
{
#if !CONFIG_IDF_TARGET_ESP32C6 && !CONFIG_IDF_TARGET_ESP32H2 // ESP32C6-TODO, ESP32H2-TODO: IDF-5657
    /* Define the array that contains the status (bits) to test on the register
     * EXTMEM_CORE0_ACS_CACHE_INT_ST_REG. each bit is accompanied by a small
     * message.
     * The messages have been pulled from the header file where the status bit
     * are defined. */
    const register_bit_t core0_acs_bits[] = {
        {
            .bit = EXTMEM_CORE0_DBUS_WR_ICACHE_ST,
            .msg = "dbus tried to write cache"
        },
        {
            .bit = EXTMEM_CORE0_DBUS_REJECT_ST,
            .msg = "dbus authentication failed"
        },
        {
            .bit = EXTMEM_CORE0_DBUS_ACS_MSK_ICACHE_ST,
            .msg = "access to cache while dbus or cache is disabled"
        },
        {
            .bit = EXTMEM_CORE0_IBUS_REJECT_ST,
            .msg = "ibus authentication failed"
        },
        {
            .bit = EXTMEM_CORE0_IBUS_WR_ICACHE_ST,
            .msg = "ibus tried to write cache"
        },
        {
            .bit = EXTMEM_CORE0_IBUS_ACS_MSK_ICACHE_ST,
            .msg = "access to cache while ibus or cache is disabled"
        },
    };

    /* Same goes for the register EXTMEM_CACHE_ILG_INT_ST_REG and its bits. */
    const register_bit_t cache_ilg_bits[] = {
        {
            .bit = EXTMEM_MMU_ENTRY_FAULT_ST,
            .msg = "MMU entry fault"
        },
        {
            .bit = EXTMEM_ICACHE_PRELOAD_OP_FAULT_ST,
            .msg = "preload configurations fault"
        },
        {
            .bit = EXTMEM_ICACHE_SYNC_OP_FAULT_ST,
            .msg = "sync configurations fault"
        },
    };

    /* Read the status register EXTMEM_CORE0_ACS_CACHE_INT_ST_REG. This status
     * register is not equal to 0 when a cache access error occured. */
    const uint32_t core0_status = REG_READ(EXTMEM_CORE0_ACS_CACHE_INT_ST_REG);

    /* If the panic is due to a cache access error, one of the bit of the
     * register is set. Thus, this function will return true. */
    bool handled = test_and_print_register_bits(core0_status, core0_acs_bits, DIM(core0_acs_bits));

    /* If the panic was due to a cache illegal error, the previous call returned false and this
     * EXTMEM_CACHE_ILG_INT_ST_REG register should not me equal to 0.
     * Check each bit of it and print the message associated if found. */
    if (!handled) {
        const uint32_t cache_ilg_status = REG_READ(EXTMEM_CACHE_ILG_INT_ST_REG);
        handled = test_and_print_register_bits(cache_ilg_status, cache_ilg_bits, DIM(cache_ilg_bits));

        /* If the error was not found, print the both registers value */
        if (!handled) {
            panic_print_str("EXTMEM_CORE0_ACS_CACHE_INT_ST_REG = 0x");
            panic_print_hex(core0_status);
            panic_print_str("\r\nEXTMEM_CACHE_ILG_INT_ST_REG = 0x");
            panic_print_hex(cache_ilg_status);
            panic_print_str("\r\n");
        }
    }
#endif
}


/**
 * Function called when a memory protection error occurs (PMS). It prints details such as the
 * explanation of why the panic occured.
 */
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE

static esp_memp_intr_source_t s_memp_intr = {MEMPROT_TYPE_INVALID, -1};

#define PRINT_MEMPROT_ERROR(err) \
        do { \
            panic_print_str("N/A (error "); \
            panic_print_str(esp_err_to_name(err)); \
            panic_print_str(")"); \
        } while(0)

static inline void print_memprot_err_details(const void *frame __attribute__((unused)))
{
    if (s_memp_intr.mem_type == MEMPROT_TYPE_INVALID && s_memp_intr.core == -1) {
        panic_print_str("  - no details available -\r\n");
        return;
    }

    //common memprot fault info
    panic_print_str("  memory type: ");
    panic_print_str(esp_mprot_mem_type_to_str(s_memp_intr.mem_type));

    panic_print_str("\r\n  faulting address: ");
    void *faulting_addr;
    esp_err_t res = esp_mprot_get_violate_addr(s_memp_intr.mem_type, &faulting_addr, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str("0x");
        panic_print_hex((int)faulting_addr);
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str( "\r\n  world: ");
    esp_mprot_pms_world_t world;
    res = esp_mprot_get_violate_world(s_memp_intr.mem_type, &world, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_pms_world_to_str(world));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    panic_print_str( "\r\n  operation type: ");
    uint32_t operation;
    res = esp_mprot_get_violate_operation(s_memp_intr.mem_type, &operation, s_memp_intr.core);
    if (res == ESP_OK) {
        panic_print_str(esp_mprot_oper_type_to_str(operation));
    } else {
        PRINT_MEMPROT_ERROR(res);
    }

    if (esp_mprot_has_byte_enables(s_memp_intr.mem_type)) {
        panic_print_str("\r\n  byte-enables: " );
        uint32_t byte_enables;
        res = esp_mprot_get_violate_byte_enables(s_memp_intr.mem_type, &byte_enables, s_memp_intr.core);
        if (res == ESP_OK) {
            panic_print_hex(byte_enables);
        } else {
            PRINT_MEMPROT_ERROR(res);
        }
    }

    panic_print_str("\r\n");
}
#endif

void panic_print_registers(const void *f, int core)
{
    uint32_t *regs = (uint32_t *)f;

    // only print ABI name
    const char *desc[] = {
        "MEPC    ", "RA      ", "SP      ", "GP      ", "TP      ", "T0      ", "T1      ", "T2      ",
        "S0/FP   ", "S1      ", "A0      ", "A1      ", "A2      ", "A3      ", "A4      ", "A5      ",
        "A6      ", "A7      ", "S2      ", "S3      ", "S4      ", "S5      ", "S6      ", "S7      ",
        "S8      ", "S9      ", "S10     ", "S11     ", "T3      ", "T4      ", "T5      ", "T6      ",
        "MSTATUS ", "MTVEC   ", "MCAUSE  ", "MTVAL   ", "MHARTID "
    };

    panic_print_str("Core ");
    panic_print_dec(((RvExcFrame *)f)->mhartid);
    panic_print_str(" register dump:");

    for (int x = 0; x < sizeof(desc) / sizeof(desc[0]); x += 4) {
        panic_print_str("\r\n");
        for (int y = 0; y < 4 && x + y < sizeof(desc) / sizeof(desc[0]); y++) {
            if (desc[x + y][0] != 0) {
                panic_print_str(desc[x + y]);
                panic_print_str(": 0x");
                panic_print_hex(regs[x + y]);
                panic_print_str("  ");
            }
        }
    }
}

/**
 * This function will be called when a SoC-level panic occurs.
 * SoC-level panics include cache errors and watchdog interrupts.
 */
void panic_soc_fill_info(void *f, panic_info_t *info)
{
    RvExcFrame *frame = (RvExcFrame *) f;

    /* Please keep in sync with PANIC_RSN_* defines */
    static const char *pseudo_reason[PANIC_RSN_COUNT] = {
        "Unknown reason",
        "Interrupt wdt timeout on CPU0",
#if SOC_CPU_NUM > 1
        "Interrupt wdt timeout on CPU1",
#endif
        "Cache error",
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
        "Memory protection fault",
#endif
    };

    info->reason = pseudo_reason[0];
    info->addr = (void *) frame->mepc;

    /* The mcause has been set by the CPU when the panic occured.
     * All SoC-level panic will call this function, thus, this register
     * lets us know which error was triggered. */
    if (frame->mcause == ETS_CACHEERR_INUM) {
        /* Panic due to a cache error, multiple cache error are possible,
         * assign function print_cache_err_details to our structure's
         * details field. As its name states, it will give more details
         * about why the error happened. */

        info->core = esp_cache_err_get_cpuid();
        info->reason = pseudo_reason[PANIC_RSN_CACHEERR];
        info->details = print_cache_err_details;

    } else if (frame->mcause == ETS_INT_WDT_INUM) {
        /* Watchdog interrupt occured, get the core on which it happened
         * and update the reason/message accordingly. */

        const int core = esp_cache_err_get_cpuid();
        info->core = core;
        info->exception = PANIC_EXCEPTION_IWDT;

#if SOC_CPU_NUM > 1
        _Static_assert(PANIC_RSN_INTWDT_CPU0 + 1 == PANIC_RSN_INTWDT_CPU1,
                       "PANIC_RSN_INTWDT_CPU1 must be equal to PANIC_RSN_INTWDT_CPU0 + 1");
#endif
        info->reason = pseudo_reason[PANIC_RSN_INTWDT_CPU0 + core];
    }
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
    else if (frame->mcause == ETS_MEMPROT_ERR_INUM) {
        info->reason = pseudo_reason[PANIC_RSN_MEMPROT];
        info->details = print_memprot_err_details;
        info->core = esp_mprot_get_active_intr(&s_memp_intr) == ESP_OK ? s_memp_intr.core : -1;
    }
#endif
}

void panic_arch_fill_info(void *frame, panic_info_t *info)
{
    RvExcFrame *regs = (RvExcFrame *) frame;
    info->core = 0;
    info->exception = PANIC_EXCEPTION_FAULT;

    //Please keep in sync with PANIC_RSN_* defines
    static const char *reason[] = {
        "Instruction address misaligned",
        "Instruction access fault",
        "Illegal instruction",
        "Breakpoint",
        "Load address misaligned",
        "Load access fault",
        "Store address misaligned",
        "Store access fault",
        "Environment call from U-mode",
        "Environment call from S-mode",
        NULL,
        "Environment call from M-mode",
        "Instruction page fault",
        "Load page fault",
        NULL,
        "Store page fault",
    };

    if (regs->mcause < (sizeof(reason) / sizeof(reason[0]))) {
        if (reason[regs->mcause] != NULL) {
            info->reason = (reason[regs->mcause]);
        }
    }

    info->description = "Exception was unhandled.";

    info->addr = (void *) regs->mepc;
}

static void panic_print_basic_backtrace(const void *frame, int core)
{
    // Basic backtrace
    panic_print_str("\r\nStack memory:\r\n");
    uint32_t sp = (uint32_t)((RvExcFrame *)frame)->sp;
    const int per_line = 8;
    for (int x = 0; x < 1024; x += per_line * sizeof(uint32_t)) {
        uint32_t *spp = (uint32_t *)(sp + x);
        panic_print_hex(sp + x);
        panic_print_str(": ");
        for (int y = 0; y < per_line; y++) {
            panic_print_str("0x");
            panic_print_hex(spp[y]);
            panic_print_str(y == per_line - 1 ? "\r\n" : " ");
        }
    }
}

void panic_print_backtrace(const void *frame, int core)
{
#if CONFIG_ESP_SYSTEM_USE_EH_FRAME
    if (!spi_flash_cache_enabled()) {
        panic_print_str("\r\nWarning: SPI Flash cache is disabled, cannot process eh_frame parsing. "
                        "Falling back to basic backtrace.\r\n");
        panic_print_basic_backtrace(frame, core);
    } else {
        esp_eh_frame_print_backtrace(frame);
    }
#else
    panic_print_basic_backtrace(frame, core);
#endif
}

uint32_t panic_get_address(const void *f)
{
    return ((RvExcFrame *)f)->mepc;
}

uint32_t panic_get_cause(const void *f)
{
    return ((RvExcFrame *)f)->mcause;
}

void panic_set_address(void *f, uint32_t addr)
{
    ((RvExcFrame *)f)->mepc = addr;
}
