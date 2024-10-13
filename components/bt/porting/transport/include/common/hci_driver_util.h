/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _H_HCI_DRIVER_UTIL_
#define _H_HCI_DRIVER_UTIL_
#include <stdint.h>

int hci_driver_util_init(void);

void hci_driver_util_deinit(void);

void hci_driver_util_tx_list_enqueue(hci_driver_data_type_t type, uint8_t *data, uint32_t len);

uint32_t hci_driver_util_tx_list_dequeue(uint32_t max_tx_len, void **tx_data, bool *last_frame);

#endif // _H_HCI_DRIVER_UTIL_
