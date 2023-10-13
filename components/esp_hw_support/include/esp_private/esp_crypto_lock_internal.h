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

#if SOC_RCC_IS_INDEPENDENT
#define MPI_RCC_ATOMIC()
#define ECC_RCC_ATOMIC()
#define HMAC_RCC_ATOMIC()
#define DS_RCC_ATOMIC()
#define ECDSA_RCC_ATOMIC()
#else /* !SOC_RCC_IS_INDEPENDENT */
#define MPI_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define ECC_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define HMAC_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define DS_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#define ECDSA_RCC_ATOMIC() PERIPH_RCC_ATOMIC()
#endif /* SOC_RCC_IS_INDEPENDENT */

#ifdef __cplusplus
}
#endif
