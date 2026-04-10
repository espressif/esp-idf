/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <assert.h>
#include <stdint.h>
#include "soc/soc_caps.h"
#include "riscv/rvruntime-frames.h"
#include "sdkconfig.h"

#if SOC_CPU_HAS_FPU || SOC_CPU_HAS_PIE
#define GDBSTUB_QXFER_FEATURES_ENABLED 1
#if SOC_CPU_HAS_PIE
#define GDBSTUB_MAX_REGISTER_SIZE 16
#endif
#endif

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
} esp_gdbstub_gdb_regfile_t;

#if GDBSTUB_QXFER_FEATURES_ENABLED
extern const char target_xml[];
#endif

#ifdef __cplusplus
}
#endif
