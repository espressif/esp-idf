/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <sys/param.h>
#include "esp_ota_ops.h"
#include "esp_attr.h"
#include "sdkconfig.h"

const esp_app_desc_t *esp_ota_get_app_description(void)
{
    return esp_app_get_description();
}

int esp_ota_get_app_elf_sha256(char* dst, size_t size)
{
   return esp_app_get_elf_sha256(dst, size);
}
