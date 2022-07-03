/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <unistd.h>
#include <setjmp.h>
#include "esp_chip_info.h"

extern jmp_buf buf;

void esp_restart(void)
{
    printf("\n");
    sleep(1); // pause for dramatic effect
    longjmp(buf, 0);
}

void esp_chip_info(esp_chip_info_t* out_info)
{
    out_info->cores = 8;
    out_info->features = (uint32_t) -1;
}
