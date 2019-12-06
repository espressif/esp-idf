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

// This module allows to hide all Modbus dependencies and work with characteristics
// instead of Modbus parameters
#include "stdlib.h"
#include "string.h"             // for memset, memcpy functions
#include "esp_err.h"
#include "esp_log.h"
#include "sense_modbus.h"       // for Modbus defines

// This module provide an easy way to work with characteristics instead of
// Modbus parameters

static const char* TAG = "SENSE_MB";

#define SENSE_MB_CHECK(a, ret_val, str, ...) \
    if (!(a)) { \
        ESP_LOGE(TAG, "%s(%u): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val); \
    }

// Define port options for the master application
#define MB_BAUDRATE 115200
#define MB_PORTNUM 2
#define MB_PARITY UART_PARITY_DISABLE

// Keep the pointer to active characteristic table and its size
static characteristic_descriptor_t** active_cid_table = NULL;
static uint32_t active_cid_table_size = 0;

// The function to get pointer to parameter storage (instance) according to parameter description table
static void* sense_modbus_get_param_data(const mb_parameter_descriptor_t* param_descriptor)
{
    assert(param_descriptor != NULL);
    void* instance_ptr = NULL;
    if (param_descriptor->param_offset != 0) {
       switch(param_descriptor->mb_param_type)
       {
           case MB_PARAM_HOLDING:
               instance_ptr = (void*)((uint32_t)&holding_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_INPUT:
               instance_ptr = (void*)((uint32_t)&input_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_COIL:
               instance_ptr = (void*)((uint32_t)&coil_reg_params + param_descriptor->param_offset - 1);
               break;
           case MB_PARAM_DISCRETE:
               instance_ptr = (void*)((uint32_t)&discrete_reg_params + param_descriptor->param_offset - 1);
               break;
           default:
               instance_ptr = NULL;
               break;
       }
    } else {
        instance_ptr = malloc((size_t)(param_descriptor->param_size));;
    }
    return instance_ptr;
}

// The helper function to get time stamp in microseconds
static uint64_t sense_modbus_get_time_stamp_us(void)
{
    uint64_t time_stamp = esp_timer_get_time();
    return time_stamp;
}

// Initialization of Modbus stack
esp_err_t sense_modbus_init(void)
{
    mb_communication_info_t comm = {
            .port = MB_PORTNUM,
            .mode = MB_MODE_RTU,
            .baudrate = MB_BAUDRATE,
            .parity = MB_PARITY
    };
    void* master_handler = NULL;

    esp_err_t err = mbc_master_init(MB_PORT_SERIAL_MASTER, &master_handler);
    SENSE_MB_CHECK((master_handler != NULL), ESP_ERR_INVALID_STATE,
                                "mb controller initialization fail.");
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            "mb controller initialization fail, returns(0x%x).",
                            (uint32_t)err);
    err = mbc_master_setup((void*)&comm);
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            "mb controller setup fail, returns(0x%x).",
                            (uint32_t)err);
    err = mbc_master_start();
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
                            "mb controller start fail, returns(0x%x).",
                            (uint32_t)err);
    // Set UART pin numbers
    err = uart_set_pin(MB_PORTNUM, CONFIG_MB_UART_TXD, CONFIG_MB_UART_RXD,
                                    CONFIG_MB_UART_RTS, UART_PIN_NO_CHANGE);
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
            "mb serial set pin failure, uart_set_pin() returned (0x%x).", (uint32_t)err); 
    // Set driver mode to Half Duplex
    err = uart_set_mode(MB_PORTNUM, UART_MODE_RS485_HALF_DUPLEX);
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
            "mb serial set mode failure, uart_set_mode() returned (0x%x).", (uint32_t)err);
    vTaskDelay(5);
    err = mbc_master_set_descriptor(&device_parameters[0], num_device_parameters);
    SENSE_MB_CHECK((err == ESP_OK), ESP_ERR_INVALID_STATE,
                                "mb controller set descriptor fail, returns(0x%x).",
                                (uint32_t)err);
    return err;
}

// Check Modbus parameters description table and create characteristic description table according to it
esp_err_t sense_modbus_get_characteristics(characteristic_descriptor_t** cid_table, uint16_t* table_size)
{
    SENSE_MB_CHECK((table_size != NULL), ESP_ERR_INVALID_ARG, "incorrect cid table size.");
    SENSE_MB_CHECK((*table_size >= 1), ESP_ERR_NOT_FOUND, "incorrect cid of characteristic.");
    SENSE_MB_CHECK((cid_table != NULL), ESP_ERR_INVALID_ARG, "incorrect cid table pointer.");

    esp_err_t error_code = ESP_OK;
    const mb_parameter_descriptor_t* param_descriptor = NULL;
    characteristic_descriptor_t* cid_instance_ptr = NULL;
    uint16_t cid_cnt = 0;
    for (cid_cnt = 0; (error_code != ESP_ERR_NOT_FOUND) && (cid_cnt < *table_size); cid_cnt++) {
        // Get data from parameters description table
        // and use this information to fill the characteristics description table
        // and having all required fields in just one table
        error_code = mbc_master_get_cid_info(cid_cnt, &param_descriptor);
        if ((error_code != ESP_ERR_NOT_FOUND) && (param_descriptor != NULL)) {
            void* temp_data_ptr = sense_modbus_get_param_data(param_descriptor);
            SENSE_MB_CHECK((temp_data_ptr != NULL),
                                ESP_ERR_INVALID_STATE, "incorrect instance pointer for cid.");
            // Allocate space for characteristic description
            cid_instance_ptr = (characteristic_descriptor_t*)malloc(sizeof(characteristic_descriptor_t));
            // If not enough memory, try to repair from this error 
            SENSE_MB_CHECK((cid_instance_ptr != NULL), 
                                ESP_ERR_NO_MEM, "incorrect memory allocation for the cid.");
            cid_table[cid_cnt] = cid_instance_ptr;
            // Fill the description according to parameter description table
            cid_instance_ptr->instance_ptr = temp_data_ptr;
            cid_instance_ptr->param_opts = param_descriptor->param_opts;
            cid_instance_ptr->cid = param_descriptor->cid;
            cid_instance_ptr->access = param_descriptor->access;
            cid_instance_ptr->param_key = param_descriptor->param_key;
            cid_instance_ptr->param_units = param_descriptor->param_units;
            cid_instance_ptr->status = ESP_FAIL;
            cid_instance_ptr->instance_size = (size_t)param_descriptor->param_size;
            cid_instance_ptr->instance_type = param_descriptor->param_type;
            cid_instance_ptr->change_flag = 0; // clear flag
            cid_instance_ptr->timestamp = 0; // Keeps timestamp of updated value
            assert(cid_instance_ptr->param_key != NULL);
        } else {
            break;
        }
    }
    ESP_LOGI(TAG, "%s: Found (%u) characteristics in table.", __FUNCTION__, cid_cnt);
    // Keep the table pointer and size of table
    active_cid_table = cid_table;
    active_cid_table_size = cid_cnt;
    *table_size = cid_cnt;
    return ESP_OK;
}

// Read characteristic value from Modbus parameter according to description table
esp_err_t sense_modbus_read_value(uint16_t cid, void *value)
{
    assert(active_cid_table != NULL);
    SENSE_MB_CHECK((value != NULL), ESP_ERR_INVALID_ARG, "incorrect value pointer.");
    SENSE_MB_CHECK((cid < active_cid_table_size), ESP_ERR_INVALID_ARG, "incorrect cid to read.");
    characteristic_descriptor_t* cid_info = active_cid_table[cid];
    SENSE_MB_CHECK((cid_info->instance_ptr != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table instance_pointer.");
    SENSE_MB_CHECK((cid_info->param_key != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table or not initialized.");
    SENSE_MB_CHECK((cid_info->cid == cid), ESP_ERR_INVALID_STATE, "incorrect cid table or not initialized.");

    // Set the data pointer for get request to value instance
    uint8_t* value_ptr = (uint8_t*)cid_info->instance_ptr;
    uint8_t type = 0;
    memset((void*)value_ptr, 0, cid_info->instance_size); // Clear value instance first

    // Send Modbus request to read cid correspond registers
    esp_err_t error = mbc_master_get_parameter(cid, (char*)cid_info->param_key, value_ptr, &type);
    SENSE_MB_CHECK((type <= PARAM_TYPE_ASCII), ESP_ERR_NOT_SUPPORTED, "returned data type is not supported (%u)", type);
    cid_info->status = error; // Keep last read status of the cid in the information table
    if (error == ESP_OK) {
        // Copy the value to param
        memcpy((void*)value, (void*)value_ptr, cid_info->instance_size);
        cid_info->timestamp = sense_modbus_get_time_stamp_us(); // Set timestamp of last access to cid
    }
    return error;
}

// Write characteristic value into associated Modbus parameter
esp_err_t sense_modbus_send_value(uint16_t cid, void* value)
{
    assert(active_cid_table != NULL);
    SENSE_MB_CHECK((cid < active_cid_table_size), ESP_ERR_INVALID_ARG, "incorrect cid to set.");
    characteristic_descriptor_t* cid_info = active_cid_table[cid];
    SENSE_MB_CHECK((cid_info->instance_ptr != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table instance_ptr.");
    SENSE_MB_CHECK((cid_info->param_key != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table param_key or not initialized.");
    SENSE_MB_CHECK((cid_info->cid == cid), ESP_ERR_INVALID_STATE, "incorrect cid in table or not initialized.");

    uint8_t* value_ptr = (uint8_t*)cid_info->instance_ptr;
    // Set the instance value of the characteristic to the value
    memcpy((void*)value_ptr, (void*)value, cid_info->instance_size);

    uint8_t type = 0;
    // Set modbus parameter according to value of characteristic
    esp_err_t error = mbc_master_set_parameter(cid, (char*)cid_info->param_key, value_ptr, &type);
    SENSE_MB_CHECK((cid_info->instance_type == type), ESP_ERR_NOT_FOUND, "incorrect type of parameter (%u)", type);
    SENSE_MB_CHECK((type <= PARAM_TYPE_ASCII), ESP_ERR_NOT_SUPPORTED, "returned data type is not supported (%u)", type);
    cid_info->change_flag = 0x55; // Set value changed flag to inform higher level
    cid_info->status = error; // Keep last read/write status of cid in the information table
    cid_info->timestamp = sense_modbus_get_time_stamp_us(); // Set time stamp of last access to cid
    return error;
}

// Get cid data from characteristic description table
esp_err_t sense_modbus_get_cid_data(uint16_t cid, characteristic_descriptor_t* cid_data)
{
    assert(active_cid_table != NULL);
    SENSE_MB_CHECK((cid_data != NULL), ESP_ERR_INVALID_ARG, "incorrect data pointer.");
    SENSE_MB_CHECK((cid < active_cid_table_size), ESP_ERR_INVALID_ARG, "cid is not found in the table.");

    characteristic_descriptor_t* cid_info = active_cid_table[cid];
    SENSE_MB_CHECK((cid_info != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table or not initialized.");
    SENSE_MB_CHECK((cid_info->instance_ptr != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table instance_ptr.");
    SENSE_MB_CHECK((cid_info->param_key != NULL), ESP_ERR_INVALID_STATE, "incorrect cid table param_key.");
    SENSE_MB_CHECK((cid_info->cid == cid), ESP_ERR_INVALID_STATE, "incorrect cid in the table or not initialized.");
    *cid_data = *cid_info; // Set cid data
    cid_info->change_flag = 0x00; // Reset flag once we get changed value

    return ESP_OK;
}

// Todo: add modbus code here
