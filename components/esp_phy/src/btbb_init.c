/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "esp_private/btbb.h"

#define BTBB_ENABLE_VERSION_PRINT 1

static _lock_t s_btbb_access_lock;
/* Reference count of enabling BT BB */
static uint8_t s_btbb_access_ref = 0;

void esp_btbb_enable(void)
{
    _lock_acquire(&s_btbb_access_lock);
    if (s_btbb_access_ref == 0) {
        bt_bb_v2_init_cmplx(BTBB_ENABLE_VERSION_PRINT);
    }
    s_btbb_access_ref++;
    _lock_release(&s_btbb_access_lock);
}

void esp_btbb_disable(void)
{
    _lock_acquire(&s_btbb_access_lock);
    if (s_btbb_access_ref > 0) {
        s_btbb_access_ref--;
    }
    _lock_release(&s_btbb_access_lock);
}
