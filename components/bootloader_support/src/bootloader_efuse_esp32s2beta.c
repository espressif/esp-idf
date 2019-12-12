// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

#include "sdkconfig.h"
#include "bootloader_clock.h"
#include "bootloader_common.h"

int bootloader_clock_get_rated_freq_mhz()
{
    /* No known limitation: all chips are 240MHz rated */
    return 240;
}

uint8_t bootloader_common_get_chip_revision(void)
{
    /* No other revisions for ESP32-S2beta */
    return 0;
}
