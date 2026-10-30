/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_privilege_abi.h"

#if !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__))
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Handle an ecall from U-mode, called by the exception handler
 *
 * @param frame Exception frame, can be modified to change the context the task is resumed in
 * @return true if the ecall was handled, false if it must be treated as an unhandled exception
 */
bool esp_privilege_ecall_handler(void *frame);

#endif /* !(defined(_ASMLANGUAGE) || defined(__ASSEMBLER__)) */
