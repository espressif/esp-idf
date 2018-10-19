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

#ifndef __SENSE_MB_H__
#define __SENSE_MB_H__

#include "mbcontroller.h"
#include "device_params.h"

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

typedef struct
{
    uint16_t            cid;                /*!< Characteristic cid */
    const char*         param_key;          /*!< The key (name) of the parameter */
    const char*         param_units;        /*!< The units of the parameter */
    mb_parameter_opt_t  param_opts;         /*!< Parameter options */
    mb_param_perms_t    access;             /*!< Access permissions based on mode */
    void*               instance_ptr;       /*!< Data instance for the parameter */
    mb_descr_type_t     instance_type;      /*!< Type of instance value */
    size_t              instance_size;      /*!< Size of instance to save data */
    uint8_t             change_flag;        /*!< Change value flag */
    esp_err_t           status;             /*!< Status of the value */
    uint64_t            timestamp;          /*!< Time stamp of last access to parameter */
} characteristic_descriptor_t;

esp_err_t sense_modbus_init();
esp_err_t sense_modbus_get_characteristics(characteristic_descriptor_t** cid_table, uint16_t* table_size);
esp_err_t sense_modbus_read_value(uint16_t cid, void* value);
esp_err_t sense_modbus_send_value(uint16_t cid, void* value);
esp_err_t sense_modbus_get_cid_data(uint16_t cid, characteristic_descriptor_t* cid_data);

#ifdef __cplusplus
}
#endif /**< _cplusplus */

#endif /**< __SENSE_MB_H__ */
