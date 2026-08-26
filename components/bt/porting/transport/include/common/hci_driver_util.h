/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
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

void hci_driver_util_assert_check(const uint32_t ln, const char *fn, uint32_t param1, uint32_t param2);
#define HCI_TRANS_ASSERT(cond, p1, p2)                                  \
        if (!(cond)) {                                                  \
            hci_driver_util_assert_check(__LINE__, __func__, p1, p2);   \
        }
#endif // _H_HCI_DRIVER_UTIL_
