/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"

#if CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM
#define LOCAL_ULP_COPROC_RESERVE_MEM CONFIG_ESP32S2_ULP_COPROC_RESERVE_MEM
#elif CONFIG_ESP32S3_ULP_COPROC_RESERVE_MEM
#define LOCAL_ULP_COPROC_RESERVE_MEM CONFIG_ESP32S3_ULP_COPROC_RESERVE_MEM
#else
#define LOCAL_ULP_COPROC_RESERVE_MEM CONFIG_ESP32_ULP_COPROC_RESERVE_MEM
#endif

#define ULP_BIN_MAGIC		0x00706c75
#define HEADER_SIZE			12
MEMORY
{
    ram(RW) : ORIGIN = 0, LENGTH = LOCAL_ULP_COPROC_RESERVE_MEM
}

SECTIONS
{
    .text : AT(HEADER_SIZE)
    {
        *(.text)
    } >ram
    .data :
    {
        . = ALIGN(4);
        *(.data)
    } >ram
    .bss :
    {
        . = ALIGN(4);
        *(.bss)
    } >ram

    .header : AT(0)
    {
        LONG(ULP_BIN_MAGIC)
        SHORT(LOADADDR(.text))
        SHORT(SIZEOF(.text))
        SHORT(SIZEOF(.data))
        SHORT(SIZEOF(.bss))
    }
}
