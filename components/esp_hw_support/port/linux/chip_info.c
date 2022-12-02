/*
 * SPDX-FileCopyrightText: 2013-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_chip_info.h"

void esp_chip_info(esp_chip_info_t *out_info)
{
    memset(out_info, 0, sizeof(*out_info));

    out_info->model = CHIP_POSIX_LINUX;

    // TODO: May need to adjust once networking becomes available on POSIX/Linux
    out_info->features = 0;
    out_info->revision = 0;
    out_info->cores = 1;
}
