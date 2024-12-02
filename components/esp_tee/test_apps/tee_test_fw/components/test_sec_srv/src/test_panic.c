/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_tee_test.h"
#include "esp_log.h"

#define RND_VAL     (0xA5A5A5A5)

extern int _tee_vec_start;
extern int _tee_vec_end;
extern int _tee_iram_start;
extern int _tee_iram_end;
extern int _tee_dram_start;

typedef void (*func_ptr)(void);

__attribute__((unused)) static const char *TAG = "esp_tee_test_panic";

#pragma GCC push_options
#pragma GCC optimize ("Og")

void _ss_esp_tee_test_resv_reg1_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(0x1));
    *test_addr = RND_VAL;
}

void _ss_esp_tee_test_resv_reg1_exec_violation(void)
{
    volatile func_ptr fptr = (void(*)(void))0x1;
    fptr();
}

void _ss_esp_tee_test_iram_reg1_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_tee_vec_start) - 0x100);
    *test_addr = RND_VAL;
}

void _ss_esp_tee_test_iram_reg2_write_violation(void)
{
    uint32_t *test_addr = (uint32_t *)((uint32_t)(&_tee_iram_start) - 0x04);
    *test_addr = RND_VAL;
}

#pragma GCC pop_options

static void foo_d(void)
{
    for (int i = 0; i < 16; i++) {
        __asm__ __volatile__("NOP");
    }
}

void _ss_esp_tee_test_dram_reg1_exec_violation(void)
{
    uint8_t s_dram_buf[256];
    memcpy(&s_dram_buf, &foo_d, sizeof(s_dram_buf));
    volatile func_ptr fptr = (void(*)(void))&s_dram_buf;
    fptr();
}

void _ss_esp_tee_test_dram_reg2_exec_violation(void)
{
    uint8_t *instr = calloc(256, sizeof(uint8_t));
    assert(instr);
    memcpy(instr, &foo_d, 256);
    volatile func_ptr fptr = (void(*)(void))instr;
    fptr();
}

void _ss_esp_tee_test_illegal_instruction(void)
{
#if CONFIG_IDF_TARGET_ARCH_XTENSA
    __asm__ __volatile__("ill");
#elif CONFIG_IDF_TARGET_ARCH_RISCV
    __asm__ __volatile__("unimp");
#endif
}
