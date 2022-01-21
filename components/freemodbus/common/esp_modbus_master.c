/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_err.h"            // for esp_err_t
#include "mbc_master.h"         // for master interface define
#include "esp_modbus_master.h"  // for public interface defines
#include "esp_modbus_callbacks.h"   // for callback functions

static const char TAG[] __attribute__((unused)) = "MB_CONTROLLER_MASTER";

// This file implements public API for Modbus master controller.
// These functions are wrappers for interface functions of the controller
static mb_master_interface_t* master_interface_ptr = NULL;

void mbc_master_init_iface(void* handler)
{
    master_interface_ptr = (mb_master_interface_t*) handler;
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
                    error,
                    "Master destroy failure, error=(0x%x).",
                    error);
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
                    error,
                    "Master get cid info failure, error=(0x%x).",
                    error);
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
                    "Master get parameter failure, error=(0x%x) (%s).",
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
                    "Master send request failure error=(0x%x) (%s).",
                    error, esp_err_to_name(error));
    return ESP_OK;
}

/**
 * Set Modbus parameter description table
 */
esp_err_t mbc_master_set_descriptor(const mb_parameter_descriptor_t* descriptor,
                                        const uint16_t num_elements)
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
                    "Master set descriptor failure, error=(0x%x) (%s).",
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
                    "Master set parameter failure, error=(0x%x) (%s).",
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
                    "Master setup failure, error=(0x%x) (%s).",
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
                    "Master start failure, error=(0x%x) (%s).",
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
