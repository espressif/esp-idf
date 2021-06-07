/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "riscv/rvruntime-frames.h"
#include "gdbstub_target_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef RvExcFrame esp_gdbstub_frame_t;

/* GDB regfile structure, configuration dependent */
typedef struct {
    uint32_t x[32];
    uint32_t pc;
} esp_gdbstub_gdb_regfile_t;


// Amount of HW breakpoints used in GDB
#ifndef GDB_BP_SIZE
#define GDB_BP_SIZE 2
#endif  // GDB_BP_SIZE

// Amount of HW watchpoints used in GDB
#ifndef GDB_WP_SIZE
#define GDB_WP_SIZE 2
#endif  // GDB_WP_SIZE

#ifdef __cplusplus
}
#endif
