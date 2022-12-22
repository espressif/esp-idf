/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set btbb enable for BT/ieee802154
 * @param[in] print_version enable btbb version print.
 * @return NULL
 */
void bt_bb_v2_init_cmplx(int print_version);

#ifdef __cplusplus
}
#endif
