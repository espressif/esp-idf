// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "iot_button.h"

CButton::CButton(gpio_num_t gpio_num, button_active_t active_level)
{
    m_btn_handle = iot_button_create(gpio_num, active_level);
}

CButton::~CButton()
{
    iot_button_delete(m_btn_handle);
    m_btn_handle = NULL;
}

esp_err_t CButton::set_evt_cb(button_cb_type_t type, button_cb cb, void* arg)
{
    return iot_button_set_evt_cb(m_btn_handle, type, cb, arg);
}

esp_err_t CButton::set_serial_cb(button_cb cb, void* arg, TickType_t interval_tick, uint32_t start_after_sec)
{
    return iot_button_set_serial_cb(m_btn_handle, start_after_sec, interval_tick, cb, arg);
}

esp_err_t CButton::add_custom_cb(uint32_t press_sec, button_cb cb, void* arg)
{
    return iot_button_add_custom_cb(m_btn_handle, press_sec, cb, arg);
}

esp_err_t CButton::rm_cb(button_cb_type_t type)
{
    return iot_button_rm_cb(m_btn_handle, type);
}
