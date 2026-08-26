/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */

#pragma once

#include "soc.h"
#include "soc/trng_reg.h"

/* Hardware random number generator register (CRC-synchronized TRNG output) */
#define WDEV_RND_REG    TRNG_CRC_SYNC_DATA_REG
