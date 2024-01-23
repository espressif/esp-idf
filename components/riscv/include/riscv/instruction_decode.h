/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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
