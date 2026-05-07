/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include "psa/crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Check whether a PSA key is persistent based on its attributes.
 *
 * Used by ESP opaque drivers to decide between inline (persistent) and
 * pointer-based (volatile) storage formats.
 */
static inline bool esp_opaque_key_is_persistent(const psa_key_attributes_t *attributes)
{
    return PSA_KEY_LIFETIME_GET_PERSISTENCE(psa_get_key_lifetime(attributes))
           != PSA_KEY_PERSISTENCE_VOLATILE;
}

#ifdef __cplusplus
}
#endif
