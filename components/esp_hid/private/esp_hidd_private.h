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

struct esp_hidd_dev_s {
    void *dev;
    esp_hid_transport_t transport;

    bool        (*connected)                (void *dev);
    esp_err_t   (*deinit)                   (void *dev);
    esp_err_t   (*battery_set)              (void *dev, uint8_t level);
    esp_err_t   (*input_set)                (void *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);
    esp_err_t   (*feature_set)              (void *dev, size_t map_index, size_t report_id, uint8_t *data, size_t length);
    esp_err_t   (*event_handler_register)   (void *dev, esp_event_handler_t callback, esp_hidd_event_t event);
    esp_err_t   (*event_handler_unregister) (void *dev, esp_event_handler_t callback, esp_hidd_event_t event);
};

typedef struct esp_hidd_dev_s esp_hidd_dev_t;

#ifdef __cplusplus
}
#endif
