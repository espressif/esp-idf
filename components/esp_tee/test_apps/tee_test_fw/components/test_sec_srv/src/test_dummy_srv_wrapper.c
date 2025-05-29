/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdarg.h>
#include "secure_service_num.h"
#include "esp_tee.h"
#include "esp_attr.h"

void NOINLINE_ATTR esp_tee_test_dummy_sec_srv(int a, int b, int c, int d, int e, int f, int g, int h, int *i)
{
    esp_tee_service_call(10, SS_ESP_TEE_TEST_DUMMY_SEC_SRV, a, b, c, d, e, f, g, h, i);
}
