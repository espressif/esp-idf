/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
extern char EXTERNAL_PROJECT_LINKER_CHECK;

int _start(void)
{
    return (int)&EXTERNAL_PROJECT_LINKER_CHECK;
}
