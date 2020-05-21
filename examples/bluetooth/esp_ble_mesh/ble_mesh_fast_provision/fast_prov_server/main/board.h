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

#ifndef _BOARD_H_
#define _BOARD_H_

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "esp_ble_mesh_defs.h"

#ifdef CONFIG_BLE_MESH_ESP_WROOM_32
#define LED_R GPIO_NUM_25
#define LED_G GPIO_NUM_26
#define LED_B GPIO_NUM_27
#elif defined(CONFIG_BLE_MESH_ESP_WROVER)
#define LED_R GPIO_NUM_0
#define LED_G GPIO_NUM_2
#define LED_B GPIO_NUM_4
#endif

struct _led_state {
    uint8_t current;
    uint8_t previous;
    uint8_t pin;
    char *name;
};

void board_output_number(esp_ble_mesh_output_action_t action, uint32_t number);

void board_prov_complete(void);

void board_led_operation(uint8_t pin, uint8_t onoff);

esp_err_t board_init(void);

#endif
