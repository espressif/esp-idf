/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_private/periph_ctrl.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

// NOTE: [ESP-TEE] Since the clock configuration APIs are part
// of the TEE, the XYZ_RCC_ATOMIC macros need to be defined as void.
#if NON_OS_BUILD
#define MPI_RCC_ATOMIC()
#define ECC_RCC_ATOMIC()
#define HMAC_RCC_ATOMIC()
#define DS_RCC_ATOMIC()
#define ECDSA_RCC_ATOMIC()
#define AES_RCC_ATOMIC()
#define SHA_RCC_ATOMIC()
#define KEY_MANAGER_RCC_ATOMIC()
#else
#define MPI_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define ECC_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define HMAC_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define DS_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define ECDSA_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define AES_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define SHA_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define KEY_MANAGER_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#endif

#ifdef __cplusplus
}
#endif
