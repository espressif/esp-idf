/*
* ESPRESSIF MIT License
*
* Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
*
* Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
* it is free of charge, to any person obtaining a copy of this software and associated
* documentation files (the "Software"), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the Software is furnished
* to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
* FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
* IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "string.h"
#include "esp_log.h"
#include "sense_modbus.h"

static const char *TAG = "sense_main";

// The number of parameters that intended to be used in the particular control process
#define SENSE_MAX_CIDS 6

// Timeout to update cid over Modbus if it is not updated by set/get request from mdf
#define MODBUS_VALUE_UPDATE_TIMEOUT_US  (10000000)
#define MODBUS_GET_REQUEST_TIMEOUT      (1000)

#define INIT_DELAY_TICS                 (100 / portTICK_RATE_MS)
#define TIMEOUT_UPDATE_CIDS_MS          (1000)
#define TIMEOUT_UPDATE_CIDS_TICS        (TIMEOUT_UPDATE_CIDS_MS / portTICK_RATE_MS)

#define SENSE_TRIGGER_TASK_STACK_SIZE   (1024 * 4)
#define SENSE_TRIGGER_TASK_PRIO         (6)

#define SENSE_MAIN_TAG "SENSE_MAIN"

#define SENS_MAIN_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(SENSE_MAIN_TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

// Characteristic information table
static characteristic_descriptor_t* cid_table[SENSE_MAX_CIDS] = { 0 };
static uint16_t cid_counter = SENSE_MAX_CIDS;

// User operational task to trigger event when cid value exceeds limit
static void trigger_operation_task(void *arg)
{
    float value;
    uint64_t timeout = 0;
    uint16_t temp_value = 0;
    bool alarm_state = false;
    bool relay_state = false;
    characteristic_descriptor_t cid_data = { 0 };

    while (1) {
        alarm_state = false;
        for (int cid = 0; cid < (CID_RELAY_P2); cid++)
        {
            // Get cid data
            ESP_ERROR_CHECK_WITHOUT_ABORT(sense_modbus_get_cid_data(cid, &cid_data));
            assert(cid_data.param_key != NULL);
            assert(cid_data.instance_ptr != NULL);
            // If value is not updated during 10 seconds then update it
            timeout = esp_timer_get_time();
            value = *(float*)cid_data.instance_ptr;
            // Check limits to set alarm if exceeded limit
            if (((value > cid_data.param_opts.max) ||
                    (value < cid_data.param_opts.min)) &&
                    (cid != CID_RELAY_P1)) {
                alarm_state = true;
            }
            if (timeout > (cid_data.timestamp + MODBUS_VALUE_UPDATE_TIMEOUT_US)) {
                // The value is not updated during timeout then update it
                // The actual value is saved in the instance storage
                value = 0;
                esp_err_t error = sense_modbus_read_value(cid, (void*)&value);
                if (error != ESP_OK) {
                    ESP_LOGE(SENSE_MAIN_TAG, "Update failed for cid: %u, %s(%s) = %d, %s",
                                                (uint16_t)cid,
                                                (char*)cid_data.param_key,
                                                (char*)cid_data.param_units,
                                                *(int*)cid_data.instance_ptr,
                                                (char*)esp_err_to_name(error));
                } else {
                    // Update state of alarm
                    if (cid == CID_RELAY_P1) {
                        // Get actual relay state
                        relay_state = (*(uint16_t*)(cid_data.instance_ptr)
                                            & (uint16_t)(cid_data.param_opts.opt1));
                        const char* relay_state_str = relay_state ? "ON" : "OFF";
                        ESP_LOGI(SENSE_MAIN_TAG, "cid: %u, (%s) = %s", (uint16_t)cid,
                                                    (char*)cid_data.param_key,
                                                    (const char*)relay_state_str);
                    } else {
                        // Update read value
                        ESP_LOGI(SENSE_MAIN_TAG, "cid: %u, %s(%s) = %.2f", (uint16_t)cid,
                                                (char*)cid_data.param_key,
                                                (char*)cid_data.param_units,
                                                (float)value);
                    }
                }
            }
        }
        // Check all characteristics if they exceed limits and set alarm accordingly
        if (!alarm_state) {
            if (relay_state == true) {
                ESP_LOGI(SENSE_MAIN_TAG, "The values within the limit, then release relay.");
                temp_value = 0;
                // Release the relay in IO slave device
                // Do not check an error in this example (IO slave may be not configured)
                (void)sense_modbus_send_value(CID_RELAY_P1, &temp_value); 
                relay_state = false;
            }
        } else {
            if (!relay_state) {
                ESP_LOGI(SENSE_MAIN_TAG, "The value exceeds limit, then set relay.");
                temp_value = 0x00FF;
                (void)sense_modbus_send_value(CID_RELAY_P1, &temp_value); // Set the relay
                relay_state = true;
            }
        }

        vTaskDelay(TIMEOUT_UPDATE_CIDS_TICS);
    }
    vTaskDelete(NULL);
}

static void sense_device_init()
{
    // Initialize and start Modbus controller
    sense_modbus_init();
}

void app_main()
{
    esp_err_t result = ESP_OK;

    // Initialization of device peripheral and objects
    sense_device_init();

    // Get all supported modbus characteristics from modbus controller
    result = sense_modbus_get_characteristics(&cid_table[0], &cid_counter);
    assert((result == ESP_OK) && (cid_counter >= 1));
    ESP_LOGI(TAG, "Found (%u) characteristics in the table.", cid_counter);
    characteristic_descriptor_t* sid_descr_ptr = { 0 };
    float value = 0;
    esp_err_t err = ESP_OK;
    int cid_registered = 0;
    // Read all found characteristics
    for (uint16_t cid = 0; cid < cid_counter; cid++) {
        sid_descr_ptr = cid_table[cid];
        // Initially read of Modbus values and check status
        err = sense_modbus_read_value(cid, (void*)&value);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Characteristic (%s) data = 0x%.4x read successful.",
                                                (char*)sid_descr_ptr->param_key, (unsigned int)value);
            cid_registered++;
        } else {
            ESP_LOGE(TAG, "Characteristic (%s) read value fail, err = %d (%s).",
                                                (char*)sid_descr_ptr->param_key,
                                                (int)err,
                                                (char*)esp_err_to_name(err));
        }
    }

    // Starts operation task to check values and trigger an event
    xTaskCreate(trigger_operation_task, "trigger_operation_task",
                                        SENSE_TRIGGER_TASK_STACK_SIZE, NULL, SENSE_TRIGGER_TASK_PRIO, NULL);
}
