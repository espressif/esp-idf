/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <protocomm_security.h>

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_ESP_PROTOCOMM_SUPPORT_SECURITY_VERSION_0
/**
 * @brief   Protocomm security version 0 implementation
 *
 * This is a simple implementation to be used when no
 * security is required for the protocomm instance
 */
extern const protocomm_security_t protocomm_security0;
#endif

#ifdef __cplusplus
}
#endif
