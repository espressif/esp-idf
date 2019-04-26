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
#include "sdkconfig.h"          // for KConfig defines
#include "mbc_slave.h"              // for slave private type definitions
#include "esp_modbus_common.h"      // for common defines
#include "esp_modbus_slave.h"       // for public slave defines
#include "esp_modbus_callbacks.h"   // for modbus callbacks function pointers declaration
#include "mbc_serial_slave.h"       // for create function of serial port

#ifdef CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT

#define MB_ID_BYTE0(id) ((uint8_t)(id))
#define MB_ID_BYTE1(id) ((uint8_t)(((uint16_t)(id) >> 8) & 0xFF))
#define MB_ID_BYTE2(id) ((uint8_t)(((uint32_t)(id) >> 16) & 0xFF))
#define MB_ID_BYTE3(id) ((uint8_t)(((uint32_t)(id) >> 24) & 0xFF))

#define MB_CONTROLLER_SLAVE_ID (CONFIG_FMB_CONTROLLER_SLAVE_ID)
#define MB_SLAVE_ID_SHORT      (MB_ID_BYTE3(MB_CONTROLLER_SLAVE_ID))

// Slave ID constant
static uint8_t mb_slave_id[] = { MB_ID_BYTE0(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE1(MB_CONTROLLER_SLAVE_ID),
                                MB_ID_BYTE2(MB_CONTROLLER_SLAVE_ID) };

#endif

// Common interface pointer for slave port
static mb_slave_interface_t* slave_interface_ptr = NULL;

/**
 * Initialization of Modbus slave controller
 */
esp_err_t mbc_slave_init(mb_port_type_t port_type, void** handler)
{
    void* port_handler = NULL;
    esp_err_t error = ESP_ERR_NOT_SUPPORTED;
    switch(port_type)
    {
        case MB_PORT_SERIAL_SLAVE:
            // Call constructor function of actual port implementation
            error = mbc_serial_slave_create(port_type, &port_handler);
            break;
        case MB_PORT_TCP_SLAVE:
            // Not yet supported
            //error = mbc_tcp_slave_create(port_type, &port_handler);
            return ESP_ERR_NOT_SUPPORTED;
        default:
            return ESP_ERR_NOT_SUPPORTED;
    }
    MB_SLAVE_CHECK((port_handler != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface initialization failure, error=(0x%x), port type=(0x%x).",
                    error, (uint16_t)port_type);

    if ((port_handler != NULL) && (error == ESP_OK)) {
        slave_interface_ptr = (mb_slave_interface_t*) port_handler;
        *handler = port_handler;
    }
    
    return error;
}

/**
 * Modbus controller destroy function
 */
esp_err_t mbc_slave_destroy()
{
    esp_err_t error = ESP_OK;
    // Is initialization done?
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    // Check if interface has been initialized
    MB_SLAVE_CHECK((slave_interface_ptr->destroy != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    // Call the slave port destroy function
    error = slave_interface_ptr->destroy();
    MB_SLAVE_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL slave destroy failure error=(0x%x).", error);
    return error;
}

/**
 * Setup Modbus controller parameters
 */
esp_err_t mbc_slave_setup(void* comm_info)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->setup != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->setup(comm_info);
    MB_SLAVE_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL slave setup failure error=(0x%x).", error);
    return error;
}

/**
 * Start Modbus controller start function
 */
esp_err_t mbc_slave_start()
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->start != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
#ifdef CONFIG_FMB_CONTROLLER_SLAVE_ID_SUPPORT
    // Set the slave ID if the KConfig option is selected
    eMBErrorCode status = eMBSetSlaveID(MB_SLAVE_ID_SHORT, TRUE, (UCHAR*)mb_slave_id, sizeof(mb_slave_id));
    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack set slave ID failure.");
#endif
    error = slave_interface_ptr->start();
    MB_SLAVE_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL slave start failure error=(0x%x).", error);
    return error;
} 

/**
 * Blocking function to get event on parameter group change for application task
 */
mb_event_group_t mbc_slave_check_event(mb_event_group_t group)
{
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    MB_EVENT_NO_EVENTS,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->check_event != NULL), 
                    MB_EVENT_NO_EVENTS,
                    "Slave interface is not correctly initialized.");
    mb_event_group_t event = slave_interface_ptr->check_event(group);
    return event;
}

/**
 * Function to get notification about parameter change from application task
 */
esp_err_t mbc_slave_get_param_info(mb_param_info_t* reg_info, uint32_t timeout)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->get_param_info != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->get_param_info(reg_info, timeout);
    MB_SLAVE_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL slave get parameter info failure error=(0x%x).", error);
    return error;
}

/**
 * Function to set area descriptors for modbus parameters
 */
esp_err_t mbc_slave_set_descriptor(mb_register_area_descriptor_t descr_data)
{
    esp_err_t error = ESP_OK;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->set_descriptor != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->set_descriptor(descr_data);
    MB_SLAVE_CHECK((error == ESP_OK), 
                    ESP_ERR_INVALID_STATE, 
                    "SERIAL slave set descriptor failure error=(0x%x).", error);
    return error;
}

/**
 * Below are stack callback functions to read/write registers
 */
eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress,
                            USHORT usNDiscrete)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->slave_reg_cb_discrete != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->slave_reg_cb_discrete(pucRegBuffer, usAddress, usNDiscrete);
    
    return error;
}

eMBErrorCode eMBRegCoilsCB(UCHAR* pucRegBuffer, USHORT usAddress,
        USHORT usNCoils, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->slave_reg_cb_coils != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->slave_reg_cb_coils(pucRegBuffer, usAddress,
                                                        usNCoils, eMode);
    return error;
}

eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress,
        USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->slave_reg_cb_holding != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->slave_reg_cb_holding(pucRegBuffer, usAddress,
                                                        usNRegs, eMode);
    return error;
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress,
                                USHORT usNRegs)
{
    eMBErrorCode error = MB_ENOERR;
    MB_SLAVE_CHECK((slave_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((slave_interface_ptr->slave_reg_cb_input != NULL), 
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    error = slave_interface_ptr->slave_reg_cb_input(pucRegBuffer, usAddress, usNRegs);
    return error;
}
