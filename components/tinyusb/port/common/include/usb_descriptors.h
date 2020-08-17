// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#include "tusb.h"

#define _PID_MAP(itf, n) ((CFG_TUD_##itf) << (n))

#define USB_ESPRESSIF_VID 0x303A

#define USB_STRING_DESCRIPTOR_ARRAY_SIZE 9
typedef char *tusb_desc_strarray_device_t[USB_STRING_DESCRIPTOR_ARRAY_SIZE];

extern tusb_desc_device_t descriptor_tinyusb;
extern tusb_desc_strarray_device_t descriptor_str_tinyusb;

extern tusb_desc_device_t descriptor_kconfig;
extern tusb_desc_strarray_device_t descriptor_str_kconfig;

