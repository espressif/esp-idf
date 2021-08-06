/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "soc/dport_access.h"

// Read a sequence of DPORT registers to the buffer.
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words)
{
    for (uint32_t i = 0;  i < num_words; ++i) {
        buff_out[i] = DPORT_SEQUENCE_REG_READ(address + i * 4);
    }
}
