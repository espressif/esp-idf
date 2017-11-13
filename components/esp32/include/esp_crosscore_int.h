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
#ifndef __ESP_CROSSCORE_INT_H
#define __ESP_CROSSCORE_INT_H


/**
 * Initialize the crosscore interrupt system for this CPU.
 * This needs to be called once on every CPU that is used
 * by FreeRTOS.
 *
 * If multicore FreeRTOS support is enabled, this will be
 * called automatically by the startup code and should not
 * be called manually.
 */
void esp_crosscore_int_init();


/**
 * Send an interrupt to a CPU indicating it should yield its
 * currently running task in favour of a higher-priority task
 * that presumably just woke up.
 *
 * This is used internally by FreeRTOS in multicore mode
 * and should not be called by the user.
 *
 * @param core_id Core that should do the yielding
 */
void esp_crosscore_int_send_yield(int core_id);


/**
 * Send an interrupt to a CPU indicating it should update its
 * CCOMPARE1 value due to a frequency switch.
 *
 * This is used internally when dynamic frequency switching is
 * enabled, and should not be called from application code.
 *
 * @param core_id Core that should update its CCOMPARE1 value
 */
void esp_crosscore_int_send_freq_switch(int core_id);

#endif
