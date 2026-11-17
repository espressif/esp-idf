/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <assert.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "riscv/rvruntime-frames.h"
#include "sdkconfig.h"

#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_DSP || SOC_CPU_HAS_PIE
#define GDBSTUB_QXFER_FEATURES_ENABLED 1
#if SOC_CPU_HAS_PIE
#define GDBSTUB_MAX_REGISTER_SIZE 16
#endif
#endif

/* First remote regnum after GDB's fixed RISC-V map (RISCV_LAST_REGNUM == 4210).
 * GDB numbers the following registers in each feature contiguously. */
#define GDBSTUB_PIE_REGNUM_FIRST 4211

#if SOC_CPU_HAS_DSP
#if SOC_CPU_HAS_PIE
#define GDBSTUB_DSP_REGNUM_FIRST 4226
#else
#define GDBSTUB_DSP_REGNUM_FIRST 4211
#endif
#endif

#define GDBSTUB_REGNUM_STR_IMPL(n) #n
#define GDBSTUB_REGNUM_STR(n) GDBSTUB_REGNUM_STR_IMPL(n)

#ifdef __cplusplus
extern "C" {
#endif

typedef RvExcFrame esp_gdbstub_frame_t;

#if SOC_CPU_HAS_FPU
    static_assert(sizeof(RvFPUSaveArea) == (32 + 1) * sizeof(uint32_t),
        "Expected 32 float registers + fcsr. Please update gdbstub internals.");
#endif
#if SOC_CPU_HAS_PIE
    static_assert(sizeof(RvPIESaveArea) == ((8 + 4 + 1) * (4 * sizeof(uint32_t))) + (2 * sizeof(uint32_t)),
        "Expected 8 Q regs, QACC, UA_STATE, XACC. Please update gdbstub internals.");
#endif
#if SOC_CPU_HAS_DSP
    static_assert(sizeof(RvDSPSaveArea) == 4 * sizeof(uint32_t),
        "Expected XACC_L, XACC_H, SAR, STATUS. Please update gdbstub internals.");
#endif

/* GDB regfile structure, configuration dependent */
typedef struct {
    uint32_t x[32];
    uint32_t pc;
#if SOC_CPU_HAS_FPU
    RvFPUSaveArea f;
#endif
#if SOC_CPU_HAS_PIE
    RvPIESaveArea pie;
#endif
#if SOC_CPU_HAS_DSP
    RvDSPSaveArea dsp;
#endif
} esp_gdbstub_gdb_regfile_t;

#if GDBSTUB_QXFER_FEATURES_ENABLED
extern const char target_xml[];
#endif

#ifdef __cplusplus
}
#endif
