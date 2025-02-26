/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_err.h"

void dummy_secure_service(int a, int b, int c, int d, int e, int f, int g, int h, int *i)
{
    esp_tee_service_call(10, SS_DUMMY_SECURE_SERVICE, a, b, c, d, e, f, g, h, i);
}
