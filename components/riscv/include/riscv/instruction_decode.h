// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Decode the offset value from a RISC-V JAL instruction
 * @note This API will abort if the instruction is not JAL formatted.
 *
 * @param inst_addr Address of JAL instruction
 * @return int offset value
 */
int riscv_decode_offset_from_jal_instruction(const intptr_t inst_addr);

#ifdef __cplusplus
}
#endif
