// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
