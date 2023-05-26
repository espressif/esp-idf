/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "riscv/rvruntime-frames.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef RvExcFrame esp_gdbstub_frame_t;

/* GDB regfile structure, configuration dependent */
typedef struct {
    uint32_t x[32];
    uint32_t pc;
} esp_gdbstub_gdb_regfile_t;

#ifdef __cplusplus
}
#endif
