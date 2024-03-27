/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "xtensa_context.h"
#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#define GDBSTUB_EXTRA_TIE_SIZE 0
#elif defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define GDBSTUB_EXTRA_TIE_SIZE 1
#else
#error "Unknown Xtensa chip"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef XtExcFrame esp_gdbstub_frame_t;

/* GDB regfile structure, configuration dependent */
typedef struct {
    uint32_t pc;
    uint32_t a[XCHAL_NUM_AREGS];

#if XCHAL_HAVE_LOOPS
    uint32_t lbeg;
    uint32_t lend;
    uint32_t lcount;
#endif

    uint32_t sar;

#if XCHAL_HAVE_WINDOWED
    uint32_t windowbase;
    uint32_t windowstart;
#endif

    uint32_t configid0;
    uint32_t configid1;
    uint32_t ps;

#if XCHAL_HAVE_THREADPTR
    uint32_t threadptr;
#endif

#if XCHAL_HAVE_BOOLEANS
    uint32_t br;
#endif

#if XCHAL_HAVE_S32C1I
    uint32_t scompare1;
#endif

#if XCHAL_HAVE_MAC16
    uint32_t acclo;
    uint32_t acchi;
    uint32_t m0;
    uint32_t m1;
    uint32_t m2;
    uint32_t m3;
#endif

#if XCHAL_HAVE_DFP_ACCEL
    uint32_t expstate;
    uint32_t f64r_lo;
    uint32_t f64r_hi;
    uint32_t f64s;
#endif

#if XCHAL_HAVE_FP
    uint32_t f[16];
    uint32_t fcr;
    uint32_t fsr;
#endif

#if GDBSTUB_EXTRA_TIE_SIZE > 0
    uint32_t tie[GDBSTUB_EXTRA_TIE_SIZE];
#endif

} esp_gdbstub_gdb_regfile_t;

#ifdef __cplusplus
}
#endif
