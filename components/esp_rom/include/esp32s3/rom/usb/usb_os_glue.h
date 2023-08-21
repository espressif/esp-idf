/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef void(*usb_osglue_intdisena_routine_t)(void);
typedef int(*usb_osglue_wait_routine_t)(int delay_us);

typedef struct {
    /* Disable USB interrupt */
    usb_osglue_intdisena_routine_t int_dis_proc;
    /* Enable USB interrupt */
    usb_osglue_intdisena_routine_t int_ena_proc;
    /* Wait for a set amount of uS. Return the amount actually waited. If delay_us is 0, just yield.*/
    usb_osglue_wait_routine_t wait_proc;
} usb_osglue_data_t;

extern usb_osglue_data_t rom_usb_osglue;

#ifdef __cplusplus
}
#endif
