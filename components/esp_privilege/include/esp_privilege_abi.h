/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/**
 * @file
 * @brief ECALL numbers understood by the M-mode trap handler
 *
 * U-mode code places the number in a7 before executing ECALL. This header is usable from C
 * and assembly, and from U-mode code built without ESP-IDF.
 */

/** End the run. a0 holds the value returned to the caller that started the run. */
#define ESP_PRIVILEGE_ECALL_EXIT 0
