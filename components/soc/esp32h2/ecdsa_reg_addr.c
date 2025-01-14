/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This file initialises the memory register addresses for the ECDSA accelerator
// This software initialization is required due to incompatibility between the old and new ECDSA versions
// for the ESP32-H2 ECDSA accelerator
#include <stddef.h>
#include "soc/ecdsa_reg.h"

// Initializing the memory address with the base address of the old ECDSA version
uint32_t ECDSA_R_MEM = (DR_REG_ECDSA_BASE + 0xA00);
uint32_t ECDSA_S_MEM = (DR_REG_ECDSA_BASE + 0xA20);
uint32_t ECDSA_Z_MEM = (DR_REG_ECDSA_BASE + 0xA40);
uint32_t ECDSA_QAX_MEM = (DR_REG_ECDSA_BASE + 0xA60);
uint32_t ECDSA_QAY_MEM = (DR_REG_ECDSA_BASE + 0xA80);

void ecdsa_compatible_mem_reg_addr_init(void)
{
    // set the memory registers based on the DATE register value
    ECDSA_R_MEM = (DR_REG_ECDSA_BASE + ECDSA_REG_GET_OFFSET(0xA00, 0x340));
    ECDSA_S_MEM = (DR_REG_ECDSA_BASE + ECDSA_REG_GET_OFFSET(0xA20, 0x360));
    ECDSA_Z_MEM = (DR_REG_ECDSA_BASE + ECDSA_REG_GET_OFFSET(0xA40, 0x380));
    ECDSA_QAX_MEM = (DR_REG_ECDSA_BASE + ECDSA_REG_GET_OFFSET(0xA60, 0x3A0));
    ECDSA_QAY_MEM = (DR_REG_ECDSA_BASE + ECDSA_REG_GET_OFFSET(0xA80, 0x3C0));
}
