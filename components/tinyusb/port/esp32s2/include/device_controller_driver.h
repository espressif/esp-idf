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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
// Espressif
#include "driver/periph_ctrl.h"
#include "freertos/xtensa_api.h"
#include "esp_intr_alloc.h"
#include "esp_log.h"
#include "soc/dport_reg.h"
#include "soc/usb_periph.h"
#include "tusb_config.h"
// TinyUSB
#include "tusb_option.h"
#include "descriptors_control.h"
#include "device/dcd.h"


#define USB_EP_DIRECTIONS 2
#define USB_MAX_EP_NUM 16

typedef struct {
    uint8_t *buffer;
    uint16_t total_len;
    uint16_t queued_len;
    uint16_t max_size;
    bool short_packet;
} xfer_ctl_t;

#ifdef __cplusplus
}
#endif
