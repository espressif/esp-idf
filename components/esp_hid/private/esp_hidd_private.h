// Copyright 2017-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "esp_err.h"
#include "esp_hidd.h"
#include "esp_hid_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    struct {
        uint16_t notify_enable : 1;
        uint16_t indicate_enable : 1;
        uint16_t reserved : 14;
    };
    uint16_t value;
} hidd_le_ccc_value_t;

typedef struct {
    uint8_t map_index;     // the index of the report map
    uint8_t report_id;     // the id of the report
    uint8_t report_type;   // input, output or feature
    uint8_t protocol_mode; // boot or report
    esp_hid_usage_t usage; // generic, keyboard, mouse, joystick or gamepad
    uint16_t value_len;    // maximum len of value by report map
    // used by gatts
    uint8_t index;           // index of the value in the gatts attr db
    uint16_t handle;         // obtained once all attributes are registered
    uint16_t ccc_handle;     // obtained once all attributes are registered
    hidd_le_ccc_value_t ccc; // notifications and/or indications enabled
} hidd_report_item_t;

struct esp_hidd_dev_s {
    void *dev;
    esp_hid_transport_t transport;

    bool        (*connected)                (void *dev);
    esp_err_t   (*deinit)                   (void *dev);
    esp_err_t   (*disconnect)               (void *dev);
    esp_err_t   (*virtual_unplug)           (void *dev);
    esp_err_t   (*battery_set)              (void *dev, uint8_t level);
    esp_err_t   (*input_set)                (void *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);
    esp_err_t   (*feature_set)              (void *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);
    esp_err_t   (*event_handler_register)   (void *dev, esp_event_handler_t callback, esp_hidd_event_t event);
    esp_err_t   (*event_handler_unregister) (void *dev, esp_event_handler_t callback, esp_hidd_event_t event);
};

typedef struct esp_hidd_dev_s esp_hidd_dev_t;

void esp_hidd_process_event_data_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id,
                                         void *event_data);

#ifdef __cplusplus
}
#endif
