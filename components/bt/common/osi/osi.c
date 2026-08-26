/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "osi/osi.h"
#include "osi/mutex.h"

int osi_init(void)
{
    int ret = 0;

    if (osi_mutex_global_init() != 0) {
        ret = -1;
    }

    return ret;
}

void osi_deinit(void)
{
    osi_mutex_global_deinit();
}
