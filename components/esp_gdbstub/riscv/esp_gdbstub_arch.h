// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#ifdef __cplusplus
}
#endif
