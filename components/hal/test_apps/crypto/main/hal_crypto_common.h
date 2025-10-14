/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#pragma once

// efuse key blocks for DS
#define DS_KEY_BLOCK_1 0
#define DS_KEY_BLOCK_2 1
#define DS_KEY_BLOCK_3 2

// efuse key blocks for HMAC
#define HMAC_KEY_BLOCK_1 4
#define HMAC_KEY_BLOCK_2 5

/*
 * ECDSA and other peripheral testcases cannot run together as block used for burning keys are overlapped
 */

// efuse key blocks for ECDSA
#define ECDSA_KEY_BLOCK_1 0
#define ECDSA_KEY_BLOCK_2 1
#define ECDSA_KEY_BLOCK_3 2
#define ECDSA_KEY_BLOCK_4 3
