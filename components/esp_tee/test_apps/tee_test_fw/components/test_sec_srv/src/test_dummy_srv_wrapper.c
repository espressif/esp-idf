/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_err.h"

void dummy_secure_service(void)
{
    esp_tee_service_call(1, SS_DUMMY_SECURE_SERVICE);
}
