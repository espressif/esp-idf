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

/**
 * @brief   Protocomm security version 2 implementation
 *
 * This is a full fledged security implementation using
 * key exchange based on SRP6a (RFC 5054)
 * and AES-GCM encryption/decryption
 */
extern const protocomm_security_t protocomm_security2;

#ifdef __cplusplus
}
#endif
