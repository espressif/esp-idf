/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* U-mode code of privilege_code.c */

#pragma once

#include <stddef.h>
#include <stdint.h>

/* Bytes that the tests copy from the start of a snippet into the memory of a domain. Every snippet is shorter. */
#define PRIVILEGE_SNIPPET_SIZE 64

/* Entry function of U-mode code */
typedef int (*privilege_snippet_t)(uint32_t arg0, uint32_t arg1);

int privilege_snippet_return_42(uint32_t arg0, uint32_t arg1);
