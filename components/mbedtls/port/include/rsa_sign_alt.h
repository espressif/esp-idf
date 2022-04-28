/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifndef _RSA_SIGN_ALT_H_
#define _RSA_SIGN_ALT_H_

#ifdef __cpluscplus
extern "C" {
#endif

#ifdef CONFIG_ESP_TLS_USE_DS_PERIPHERAL
#include "esp_ds/esp_rsa_sign_alt.h"
#else

#error "DS configuration flags not activated, please enable required menuconfig flags"

#endif

#ifdef __cpluscplus
}
#endif

#endif
