/*
 * SPDX-FileCopyrightText: 2014, Kenneth MacKay
 *
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * SPDX-FileContributor: 2020-2021 Espressif Systems (Shanghai) CO LTD
 */
#pragma once
#include "uECC.h"

/* Version uECC_verify() that also copies message_hash to verified_hash
   if the signature is valid, and does it in a way that is harder to attack
   with fault injection.
*/
int uECC_verify_antifault(const uint8_t *public_key,
                          const uint8_t *message_hash,
                          unsigned hash_size,
                          const uint8_t *signature,
                          uECC_Curve curve,
                          uint8_t *verified_hash);
