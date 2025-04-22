/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "btc/btc_task.h"
#include "osi/thread.h"

#if BTC_DYNAMIC_MEMORY == FALSE
void *btc_profile_cb_tab[BTC_PID_NUM] = {};
#else
void **btc_profile_cb_tab;
#endif

void esp_profile_cb_reset(void)
{
    #if BTC_DYNAMIC_MEMORY == TRUE
    if (btc_profile_cb_tab == NULL) {
        return;
    }
    #endif

    int i;

    for (i = 0; i < BTC_PID_NUM; i++) {
        btc_profile_cb_tab[i] = NULL;
    }
}

int btc_profile_cb_set(btc_pid_t profile_id, void *cb)
{
    #if BTC_DYNAMIC_MEMORY == TRUE
    if (btc_profile_cb_tab == NULL) {
        return -1;
    }
    #endif

    if (profile_id < 0 || profile_id >= BTC_PID_NUM) {
        return -1;
    }

    btc_profile_cb_tab[profile_id] = cb;

    return 0;
}

void *btc_profile_cb_get(btc_pid_t profile_id)
{
    #if BTC_DYNAMIC_MEMORY == TRUE
    if (btc_profile_cb_tab == NULL) {
        return NULL;
    }
    #endif

    if (profile_id < 0 || profile_id >= BTC_PID_NUM) {
        return NULL;
    }

    return btc_profile_cb_tab[profile_id];
}
