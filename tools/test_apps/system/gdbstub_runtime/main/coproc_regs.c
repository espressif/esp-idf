/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#if CONFIG_IDF_TARGET_ARCH_RISCV
#include "riscv/rvruntime-frames.h"
#endif
#include "soc/soc_caps.h"

static SemaphoreHandle_t sem = NULL;
#if SOC_CPU_HAS_PIE
static volatile bool test_pie_ready = false;
#endif
#if SOC_CPU_HAS_FPU
static volatile bool test_fpu_ready = false;
#endif

#if SOC_CPU_HAS_PIE
static void test_pie(void * arg)
{
    RvPIESaveArea pie_regs_sample, pie_regs;
    register void* ptr asm("a5");

    memset(pie_regs_sample.q0, 0x10 + (uint32_t) arg, sizeof(pie_regs_sample.q0));
    memset(pie_regs_sample.q1, 0x11 + (uint32_t) arg, sizeof(pie_regs_sample.q1));
    memset(pie_regs_sample.q2, 0x12 + (uint32_t) arg, sizeof(pie_regs_sample.q2));
    memset(pie_regs_sample.q3, 0x13 + (uint32_t) arg, sizeof(pie_regs_sample.q3));
    memset(pie_regs_sample.q4, 0x14 + (uint32_t) arg, sizeof(pie_regs_sample.q4));
    memset(pie_regs_sample.q5, 0x15 + (uint32_t) arg, sizeof(pie_regs_sample.q5));
    memset(pie_regs_sample.q6, 0x16 + (uint32_t) arg, sizeof(pie_regs_sample.q6));
    memset(pie_regs_sample.q7, 0x17 + (uint32_t) arg, sizeof(pie_regs_sample.q7));
    memset(pie_regs_sample.qacc_l_l, 0x18 + (uint32_t) arg, sizeof(pie_regs_sample.qacc_l_l));
    memset(pie_regs_sample.qacc_l_h, 0x19 + (uint32_t) arg, sizeof(pie_regs_sample.qacc_l_h));
    memset(pie_regs_sample.qacc_h_l, 0x1a + (uint32_t) arg, sizeof(pie_regs_sample.qacc_h_l));
    memset(pie_regs_sample.qacc_h_h, 0x1b + (uint32_t) arg, sizeof(pie_regs_sample.qacc_h_h));
    memset(pie_regs_sample.ua_state, 0x1c + (uint32_t) arg, sizeof(pie_regs_sample.ua_state));
    memset(&pie_regs_sample.xacc, 0x1d + (uint32_t) arg, sizeof(uint8_t) * 5 /* 40-bits */);

pie_start:
    asm volatile ("nop");

    while (!test_pie_ready) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

    __asm__ volatile ("mv %0, %1" : "=r"(ptr) : "r" (&pie_regs)); /* ptr = &pie_regs; */

    __asm__ volatile ("esp.vst.128.ip q0, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q1, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q2, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q3, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q4, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q5, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q6, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.vst.128.ip q7, %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.l.l.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.l.h.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.h.l.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.qacc.h.h.128.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.ua.state.ip %0, 16" :: "r" (ptr));
    __asm__ volatile ("esp.st.s.xacc.ip %0, 0" :: "r" (ptr));

    if (!memcmp(&pie_regs_sample, &pie_regs, sizeof(pie_regs))) {
        xSemaphoreGive((SemaphoreHandle_t) sem);
    }

    vTaskDelete(NULL);
}
#endif

#if SOC_CPU_HAS_FPU
static void test_fpu(void * arg)
{
#if CONFIG_IDF_TARGET_ARCH_XTENSA
    struct {
        float f[16];
    } fpu_regs_sample, fpu_regs;
#endif
#if CONFIG_IDF_TARGET_ARCH_RISCV
    RvFPUSaveArea fpu_regs_sample, fpu_regs;
#endif
    uint32_t *ptr = (uint32_t *)&fpu_regs_sample;
    for (int i = 0; i < sizeof(fpu_regs_sample)/sizeof(uint32_t); i++) {
        ptr[i] = i + (int) arg;
    }

    /* Set FPU owner to current task by calling an instruction */
#if CONFIG_IDF_TARGET_ARCH_XTENSA
    __asm__ volatile ("ssi f0, %0, 0" :: "a" (&fpu_regs));
#endif
#if 0 // TODO IDF-15053: set CONFIG_IDF_TARGET_ARCH_RISCV
    __asm__ volatile ("fsw ft0, %0" : "=m" (fpu_regs.ft0));
#endif

fpu_start:
    asm volatile ("nop");

    while (!test_fpu_ready) {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }

#if CONFIG_IDF_TARGET_ARCH_XTENSA
    __asm__ volatile ("ssi f0, %0, 0" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f1, %0, 4" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f2, %0, 8" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f3, %0, 12" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f4, %0, 16" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f5, %0, 20" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f6, %0, 24" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f7, %0, 28" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f8, %0, 32" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f9, %0, 36" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f10, %0, 40" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f11, %0, 44" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f12, %0, 48" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f13, %0, 52" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f14, %0, 56" :: "a" (&fpu_regs));
    __asm__ volatile ("ssi f15, %0, 60" :: "a" (&fpu_regs));
#endif
#if CONFIG_IDF_TARGET_ARCH_RISCV
    __asm__ volatile ("fsw ft0, %0" : "=m" (fpu_regs.ft0));
    __asm__ volatile ("fsw ft1, %0" : "=m" (fpu_regs.ft1));
    __asm__ volatile ("fsw ft2, %0" : "=m" (fpu_regs.ft2));
    __asm__ volatile ("fsw ft3, %0" : "=m" (fpu_regs.ft3));
    __asm__ volatile ("fsw ft4, %0" : "=m" (fpu_regs.ft4));
    __asm__ volatile ("fsw ft5, %0" : "=m" (fpu_regs.ft5));
    __asm__ volatile ("fsw ft6, %0" : "=m" (fpu_regs.ft6));
    __asm__ volatile ("fsw ft7, %0" : "=m" (fpu_regs.ft7));
    __asm__ volatile ("fsw fs0, %0" : "=m" (fpu_regs.fs0));
    __asm__ volatile ("fsw fs1, %0" : "=m" (fpu_regs.fs1));
    __asm__ volatile ("fsw fa0, %0" : "=m" (fpu_regs.fa0));
    __asm__ volatile ("fsw fa1, %0" : "=m" (fpu_regs.fa1));
    __asm__ volatile ("fsw fa2, %0" : "=m" (fpu_regs.fa2));
    __asm__ volatile ("fsw fa3, %0" : "=m" (fpu_regs.fa3));
    __asm__ volatile ("fsw fa4, %0" : "=m" (fpu_regs.fa4));
    __asm__ volatile ("fsw fa5, %0" : "=m" (fpu_regs.fa5));
    __asm__ volatile ("fsw fa6, %0" : "=m" (fpu_regs.fa6));
    __asm__ volatile ("fsw fa7, %0" : "=m" (fpu_regs.fa7));
    __asm__ volatile ("fsw fs2, %0" : "=m" (fpu_regs.fs2));
    __asm__ volatile ("fsw fs3, %0" : "=m" (fpu_regs.fs3));
    __asm__ volatile ("fsw fs4, %0" : "=m" (fpu_regs.fs4));
    __asm__ volatile ("fsw fs5, %0" : "=m" (fpu_regs.fs5));
    __asm__ volatile ("fsw fs6, %0" : "=m" (fpu_regs.fs6));
    __asm__ volatile ("fsw fs7, %0" : "=m" (fpu_regs.fs7));
    __asm__ volatile ("fsw fs8, %0" : "=m" (fpu_regs.fs8));
    __asm__ volatile ("fsw fs9, %0" : "=m" (fpu_regs.fs9));
    __asm__ volatile ("fsw fs10, %0" : "=m" (fpu_regs.fs10));
    __asm__ volatile ("fsw fs11, %0" : "=m" (fpu_regs.fs11));
    __asm__ volatile ("fsw ft8, %0" : "=m" (fpu_regs.ft8));
    __asm__ volatile ("fsw ft9, %0" : "=m" (fpu_regs.ft9));
    __asm__ volatile ("fsw ft10, %0" : "=m" (fpu_regs.ft10));
    __asm__ volatile ("fsw ft11, %0" : "=m" (fpu_regs.ft11));
    __asm__ volatile ("csrr %0, fcsr" : "=r" (fpu_regs.fcsr));
#endif

    if (!memcmp(&fpu_regs_sample, &fpu_regs, sizeof(fpu_regs))) {
        xSemaphoreGive((SemaphoreHandle_t) sem);
    }

    vTaskDelete(NULL);
}
#endif

/* TODO: IDF-12550. Extend test for both CPU0 and CPU1 */
void test_coproc_regs(void) {
    sem = xSemaphoreCreateCounting(2, 0);
#if SOC_CPU_HAS_FPU
    xTaskCreatePinnedToCore(test_fpu, "test_fpu_1", 4096, (void *)1, 10, NULL, 0);
    xTaskCreatePinnedToCore(test_fpu, "test_fpu_2", 4096, (void *)2, 10, NULL, 0);

    for (int i = 0; i < 2;) {
        if (xSemaphoreTake(sem, 500 / portTICK_PERIOD_MS)) {
            i++;
        }
    }
    vTaskDelay(10); // Allow tasks to clean up
fpu_succeed:
#endif

#if SOC_CPU_HAS_PIE
    xTaskCreatePinnedToCore(test_pie, "test_pie_1", 4096, (void *)1, 10, NULL, 0);
    xTaskCreatePinnedToCore(test_pie, "test_pie_2", 4096, (void *)2, 10, NULL, 0);

    for (int i = 0; i < 2;) {
        if (xSemaphoreTake(sem, 500 / portTICK_PERIOD_MS)) {
            i++;
        }
    }
    vTaskDelay(10); // Allow tasks to clean up
pie_succeed:
#endif

    vSemaphoreDelete(sem);
}
