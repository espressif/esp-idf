/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C3
#include "esp32c3/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C61    //TODO: IDF-9526, refactor this
#include "esp32c61/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32C5
#include "esp32c5/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32H2
#include "esp32h2/rom/libc_stubs.h"
#elif CONFIG_IDF_TARGET_ESP32P4
#include "esp32p4/rom/libc_stubs.h"
#endif
