/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "esp_err.h"            // for esp_err_t
#include "mbc_master.h"         // for master interface define
#include "esp_modbus_master.h"  // for public interface defines
#include "mbc_serial_master.h"      // for create function of the port
#include "esp_modbus_callbacks.h"   // for callback functions

// This file implements public API for Modbus master controller. 
// These functions are wrappers for interface functions of the controller
static mb_master_interface_t* master_interface_ptr = NULL;

/**
 * Initialization of Modbus controller resources
 */
esp_err_t mbc_master_init(mb_port_type_t port_type, void** handler)
{
    void* port_handler = NULL;
    esp_err_t error = ESP_ERR_NOT_SUPPORTED;
    switch(port_type)
    {
    case MB_PORT_SERIAL_MASTER:
        error = mbc_serial_master_create(port_type, &port_handler);
        break;
    case MB_PORT_TCP_MASTER:
        // TCP MAster is not yet supported
        //error = mbc_tcp_master_create(port_type, &port_handler);
        return ESP_ERR_NOT_SUPPORTED;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    MB_MASTER_CHECK((port_handler != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface initialization failure, error=(0x%x), port type=(0x%x).",
                    error, (uint16_t)port_type);

    if ((port_handler != NULL) && (error == ESP_OK)) {
        master_interface_ptr = (mb_master_interface_t*) port_handler;
        *handler = port_handler;
    }
    
    return  error;
}

/**
 * Modbus controller destroy function
 */
esp_err_t mbc_master_destroy(void)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->destroy != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->destroy();
    MB_MASTER_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL master destroy failure error=(0x%x).", 
                    (uint16_t)error);
    return error;
}

esp_err_t mbc_master_get_cid_info(uint16_t cid, const mb_parameter_descriptor_t** param_info)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->get_cid_info != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->get_cid_info(cid, param_info);
    MB_MASTER_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL master get cid info failure error=(0x%x).", 
                    (uint16_t)error);
    return error;
}

/**
 * Get parameter data for corresponding characteristic
 */
esp_err_t mbc_master_get_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t* type)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->get_parameter != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->get_parameter(cid, name, value, type);
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master get parameter failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return error;
}

/**
 * Send custom Modbus request defined as mb_param_request_t structure
 */
esp_err_t mbc_master_send_request(mb_param_request_t* request, void* data_ptr)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->send_request != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->send_request(request, data_ptr);
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master send request failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Set Modbus parameter description table
 */
esp_err_t mbc_master_set_descriptor(const mb_parameter_descriptor_t* descriptor, const uint16_t num_elements)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->set_descriptor != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->set_descriptor(descriptor, num_elements);
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master set descriptor failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Set parameter value for characteristic selected by name and cid
 */
esp_err_t mbc_master_set_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t* type)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->set_parameter != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->set_parameter(cid, name, value, type);
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master set parameter failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Setup Modbus controller parameters
 */
esp_err_t mbc_master_setup(void* comm_info)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->setup != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->setup(comm_info);
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master setup failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Modbus controller stack start function
 */
esp_err_t mbc_master_start(void)
{
    esp_err_t error = ESP_OK;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->start != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->start();
    MB_MASTER_CHECK((error == ESP_OK), 
                    error,
                    "SERIAL master start failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

eMBErrorCode eMBMasterRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                            USHORT usNDiscrete)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_discrete != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_discrete(pucRegBuffer, usAddress, usNDiscrete);
    return error;
}

eMBErrorCode eMBMasterRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_coils != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_coils(pucRegBuffer, usAddress,
                                                        usNCoils, eMode);
    return error;
}

eMBErrorCode eMBMasterRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_holding != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_holding(pucRegBuffer, usAddress,
                                                        usNRegs, eMode);
    return error;
}

eMBErrorCode eMBMasterRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                USHORT usNRegs)
{
    eMBErrorCode error = MB_ENOERR;
    MB_MASTER_CHECK((master_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    MB_MASTER_CHECK((master_interface_ptr->master_reg_cb_input != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Master interface is not correctly initialized.");
    error = master_interface_ptr->master_reg_cb_input(pucRegBuffer, usAddress, usNRegs);
    return error;
}