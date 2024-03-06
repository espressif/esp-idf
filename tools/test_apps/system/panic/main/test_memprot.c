/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/soc.h"

#include "test_memprot.h"
#include "sdkconfig.h"

#define RND_VAL     (0xA5A5A5A5)
#define SPIN_ITER   (16)

extern int _iram_start;
extern int _iram_text_start;
extern int _iram_text_end;

/* NOTE: Naming conventions for RTC_FAST_MEM are
 * different for ESP32-C3 and other RISC-V targets
 */
#if CONFIG_SOC_RTC_FAST_MEM_SUPPORTED
#if CONFIG_IDF_TARGET_ARCH_RISCV
extern int _rtc_fast_start;
#else
extern int _rtc_text_start;
#endif
extern int _rtc_text_end;
extern int _rtc_force_fast_start;
#endif

#if CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED
extern int _rtc_force_slow_start;
extern int _rtc_data_start;
#endif

/* ---------------------------------------------------- DCACHE Violation Checks ---------------------------------------------------- */

#if SOC_DCACHE_SUPPORTED
/* DCACHE: Internal cache memory accessed via DBUS */
static uint32_t* get_test_dcache_addr(void)
{
    uint32_t *dcache_addr = NULL;
#if !CONFIG_ESP32S2_DATA_CACHE_0KB
    dcache_addr = (uint32_t *)MAP_IRAM_TO_DRAM((uint32_t)&_iram_start - 0x04);
#endif
    return dcache_addr;
}

void test_dcache_read_violation(void)
{
    uint32_t *test_addr = get_test_dcache_addr();
    printf("DCACHE: Read operation | Address: %p\n", test_addr);
    printf("Value : 0x%" PRIx32 "\n", *test_addr);
}

void test_dcache_write_violation(void)
{
    uint32_t *test_addr = get_test_dcache_addr();
    printf("DCACHE: Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}
#endif

/* ---------------------------------------------------- IRAM Violation Checks ---------------------------------------------------- */

/* IRAM: I/DCACHE boundary region */
void test_iram_reg1_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_iram_start) - 0x04);
    printf("IRAM: Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

/* IRAM: Interrupt vector table region */
void test_iram_reg2_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_iram_text_start) - 0x04);
    printf("IRAM: Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

/* IRAM: Text (and data) region */
void test_iram_reg3_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_iram_text_end) - 0x04);
    printf("IRAM: Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

/* IRAM: Through the data bus */
void test_iram_reg4_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)MAP_IRAM_TO_DRAM((uint32_t)&_iram_text_end - 0x04);
    printf("IRAM: Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

/* ---------------------------------------------------- DRAM Violation Checks ---------------------------------------------------- */

static void foo_d(void)
{
    for(int i = 0; i < SPIN_ITER; i++)
        __asm__ __volatile__("NOP");
}

static DRAM_ATTR uint8_t s_dram_buf[1024];

/* DRAM: Data region (DRAM_ATTR tagged) */
void test_dram_reg1_execute_violation(void)
{
    memcpy(&s_dram_buf, &foo_d, sizeof(s_dram_buf));
    void (*func_ptr)(void);
    func_ptr = (void(*)(void))&s_dram_buf;
    printf("DRAM: Execute operation | Address: %p\n", &s_dram_buf);
    func_ptr();
}

/* DRAM: Heap region */
void test_dram_reg2_execute_violation(void)
{
    uint8_t *instr = calloc(1024, sizeof(uint8_t));
    assert(instr != NULL);

    printf("DRAM: Execute operation | Address: %p\n", instr);

    memcpy(instr, &foo_d, 1024);
    void (*func_ptr)(void);
    func_ptr = (void(*)(void))instr;
    func_ptr();
}

/* ---------------------------------------------------- RTC Violation Checks ---------------------------------------------------- */

#if CONFIG_SOC_RTC_FAST_MEM_SUPPORTED
static RTC_FAST_ATTR uint32_t var_f = RND_VAL;

static RTC_IRAM_ATTR void foo_f(void)
{
    for(int i = 0; i < SPIN_ITER; i++)
        __asm__ __volatile__("NOP");
}

/* RTC_FAST_MEM: .text section start */
void test_rtc_fast_reg1_execute_violation(void)
{
#if CONFIG_IDF_TARGET_ARCH_RISCV
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_fast_start);
#else
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_text_start);
#endif
    printf("RTC_MEM (Fast): Execute operation | Address: %p\n",  test_addr);
    test_addr();
}

/* RTC_FAST_MEM: .text section boundary */
void test_rtc_fast_reg2_execute_violation(void)
{
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_text_end - 0x04);
    printf("RTC_MEM (Fast): Execute operation | Address: %p\n", test_addr);
    test_addr();
}

/* RTC_FAST_MEM: .data section */
void test_rtc_fast_reg3_execute_violation(void)
{
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_force_fast_start + 0x04);
    printf("RTC_MEM (Fast): Execute operation | Address: %p\n", test_addr);
    test_addr();
}
#endif

#if CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED
static RTC_SLOW_ATTR uint32_t var_s = RND_VAL;

static RTC_SLOW_ATTR void foo_s(void)
{
    for(int i = 0; i < SPIN_ITER; i++)
        __asm__ __volatile__("NOP");
}

/* RTC_SLOW_MEM: Data tagged with RTC_SLOW_ATTR */
void test_rtc_slow_reg1_execute_violation(void)
{
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_force_slow_start);
    printf("RTC_MEM (Slow): Execute operation | Address: %p\n", test_addr);
    test_addr();
}

/* RTC_SLOW_MEM: Region start */
void test_rtc_slow_reg2_execute_violation(void)
{
    void (*test_addr)(void) = (void(*)(void))((uint32_t)&_rtc_data_start);
    printf("RTC_MEM (Slow): Execute operation | Address: %p\n", test_addr);
    test_addr();
}
#endif

static void __attribute__((constructor)) test_print_rtc_var_func(void)
{
#if CONFIG_SOC_RTC_FAST_MEM_SUPPORTED
    printf("foo_f: %p | var_f: %p\n", &foo_f, &var_f);
#endif
#if CONFIG_SOC_RTC_SLOW_MEM_SUPPORTED
    printf("foo_s: %p | var_s: %p\n", &foo_s, &var_s);
#endif
}


/* ---------------------------------------------------- I/D Cache (Flash) Violation Checks ---------------------------------------------------- */

#if CONFIG_ESP_SYSTEM_PMP_IDRAM_SPLIT
static const uint16_t foo_buf[8] = {
    0x0001, 0x0001, 0x0001, 0x0001,
    0x0001, 0x0001, 0x0001, 0x0001,
};

void test_irom_reg_write_violation(void)
{
    extern int _instruction_reserved_end;
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_instruction_reserved_end - 0x100));
    printf("Flash (IROM): Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

void test_drom_reg_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(foo_buf));
    printf("Flash (DROM): Write operation | Address: %p\n", test_addr);
    *test_addr = RND_VAL;
}

void test_drom_reg_execute_violation(void)
{
    printf("Flash (DROM): Execute operation | Address: %p\n", foo_buf);
    void (*func_ptr)(void);
    func_ptr = (void(*)(void))foo_buf;
    func_ptr();
}
#endif
