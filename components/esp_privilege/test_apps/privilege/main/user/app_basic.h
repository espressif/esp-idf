/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Interface of the U-mode program app_basic.c, shared with the tests. The first argument of a run selects
 * what the program does with the second argument, arg. */

#pragma once

/* Returns arg * 3 + 1 */
#define APP_BASIC_RETURN      0
/* Ends the run from a nested function with umode_exit(arg + 7) */
#define APP_BASIC_EXIT_NESTED 1
/* Returns 0 if .data and .bss have their initial values, then sets the .data value to arg and the .bss buffer
 * to arg + index */
#define APP_BASIC_CHECK_DATA  2
/* Returns the .data value plus byte arg of the .bss buffer, which holds (uint8_t)(arg of APP_BASIC_CHECK_DATA + index) */
#define APP_BASIC_READ_DATA   3
/* Calls add (even arg) or multiply (odd arg) on 6 and 7 through a table of function pointers, and returns
 * the result times 100 plus the length of the operation name, read through a table of string pointers */
#define APP_BASIC_CALL_TABLE  4
/* Increments a .bss counter and returns it */
#define APP_BASIC_COUNT       5

/* Initial .data value */
#define APP_BASIC_DATA_INITIAL 0x12345678u

/* Error values */
#define APP_BASIC_ERROR_DATA     (-1)
#define APP_BASIC_ERROR_BSS      (-2)
#define APP_BASIC_ERROR_EXIT     (-3)
#define APP_BASIC_ERROR_SELECTOR (-4)
