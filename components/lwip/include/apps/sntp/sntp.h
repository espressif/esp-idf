/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#warning "sntp.h in IDF's lwip port folder is deprecated. Please include esp_sntp.h"
/*
 * This header is provided only for compatibility reasons for existing
 * applications which directly include "sntp.h" from the IDF default paths.
 * and will be removed in IDF v6.0.
 * It is recommended to use "esp_sntp.h" from IDF's lwip port folder
 */
#include "esp_sntp.h"
