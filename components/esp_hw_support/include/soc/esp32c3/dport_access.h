/*
 * SPDX-FileCopyrightText: 2010-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ESP_DPORT_ACCESS_H_
#define _ESP_DPORT_ACCESS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Read a sequence of DPORT registers to the buffer.
 *
 * @param[out] buff_out  Contains the read data.
 * @param[in]  address   Initial address for reading registers.
 * @param[in]  num_words The number of words.
 */
void esp_dport_access_read_buffer(uint32_t *buff_out, uint32_t address, uint32_t num_words);

#define DPORT_STALL_OTHER_CPU_START()
#define DPORT_STALL_OTHER_CPU_END()
#define DPORT_INTERRUPT_DISABLE()
#define DPORT_INTERRUPT_RESTORE()

#ifdef __cplusplus
}
#endif

#endif /* _ESP_DPORT_ACCESS_H_ */
