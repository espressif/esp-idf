/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// mbc_tcp_master.c
// TCP master implementation of the Modbus controller

#include <sys/time.h>               // for calculation of time stamp in milliseconds
#include "esp_log.h"                // for log_write
#include <string.h>                 // for memcpy
#include <sys/queue.h>              // for list
#include "freertos/FreeRTOS.h"      // for task creation and queue access
#include "freertos/task.h"          // for task api access
#include "freertos/event_groups.h"  // for event groups
#include "freertos/queue.h"         // for queue api access
#include "mb_m.h"                   // for modbus stack master types definition
#include "port.h"                   // for port callback functions and defines
#include "mbutils.h"                // for mbutils functions definition for stack callback
#include "sdkconfig.h"              // for KConfig values
#include "esp_modbus_common.h"      // for common types
#include "esp_modbus_master.h"      // for public master types
#include "mbc_master.h"             // for private master types
#include "mbc_tcp_master.h"         // for tcp master create function and types
#include "port_tcp_master.h"        // for tcp master port defines and types

#if MB_MASTER_TCP_ENABLED

/*-----------------------Master mode use these variables----------------------*/

// The response time is average processing time + data transmission
#define MB_RESPONSE_TIMEOUT pdMS_TO_TICKS(CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND)
#define MB_TCP_CONNECTION_TOUT pdMS_TO_TICKS(CONFIG_FMB_TCP_CONNECTION_TOUT_SEC * 1000)

static mb_master_interface_t* mbm_interface_ptr = NULL;
static const char *TAG = "MB_CONTROLLER_MASTER";

// Searches the slave address in the address info list and returns address info if found, else NULL
static mb_slave_addr_entry_t* mbc_tcp_master_find_slave_addr(uint8_t slave_addr)
{
    mb_slave_addr_entry_t* it;
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;

    if (LIST_EMPTY(&mbm_opts->mbm_slave_list)) {
        return NULL;
    }
    LIST_FOREACH(it, &mbm_opts->mbm_slave_list, entries) {
        if (slave_addr == it->slave_addr) {
            return it;
        }
    }
    return NULL;
}

static esp_err_t mbc_tcp_master_add_slave(uint16_t index, uint8_t slave_addr, const char* ip_addr)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    // Initialize interface properties
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;

    mb_slave_addr_entry_t* new_slave_entry = (mb_slave_addr_entry_t*) heap_caps_malloc(sizeof(mb_slave_addr_entry_t),
                                               MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    MB_MASTER_CHECK((new_slave_entry != NULL), ESP_ERR_NO_MEM, "mb can not allocate memory for slave entry.");
    new_slave_entry->index = index;
    new_slave_entry->ip_address = ip_addr;
    new_slave_entry->slave_addr = slave_addr;
    new_slave_entry->p_data = NULL;
    LIST_INSERT_HEAD(&mbm_opts->mbm_slave_list, new_slave_entry, entries);
    MB_MASTER_CHECK((mbm_opts->mbm_slave_list_count < (MB_TCP_PORT_MAX_CONN - 1)),
                        ESP_ERR_INVALID_STATE, "mb max number of slaves < %d.", MB_TCP_PORT_MAX_CONN);
    mbm_opts->mbm_slave_list_count++;
    return ESP_OK;
}

static void mbc_tcp_master_free_slave_list(void)
{
    mb_slave_addr_entry_t* it;
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);

    // Initialize interface properties
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;

    while ((it = LIST_FIRST(&mbm_opts->mbm_slave_list))) {
        LIST_REMOVE(it, entries);
        mbm_opts->mbm_slave_list_count--;
        free(it);
    }
}

// Modbus event processing task
static void modbus_tcp_master_task(void *pvParameters)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_ASSERT(mbm_opts != NULL);

    // Main Modbus stack processing cycle
    for (;;) {
        // Wait for poll events
        BaseType_t status = xEventGroupWaitBits(mbm_opts->mbm_event_group,
                                                (BaseType_t)(MB_EVENT_STACK_STARTED),
                                                pdFALSE, // do not clear bits
                                                pdFALSE,
                                                portMAX_DELAY);
        // Check if stack started then poll for data
        if (status & MB_EVENT_STACK_STARTED) {
            (void)eMBMasterPoll(); // Allow stack to process data
        }
    }
}

// Setup Modbus controller parameters
static esp_err_t mbc_tcp_master_setup(void* comm_info)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_CHECK((mbm_opts != NULL), ESP_ERR_INVALID_ARG, "mb incorrect options pointer.");

    const mb_communication_info_t* comm_info_ptr = (mb_communication_info_t*)comm_info;
    // Check communication options
    MB_MASTER_CHECK((comm_info_ptr->ip_mode == MB_MODE_TCP),
                ESP_ERR_INVALID_ARG, "mb incorrect mode = (0x%x).",
                (uint32_t)comm_info_ptr->ip_mode);
    MB_MASTER_CHECK((comm_info_ptr->ip_addr != NULL),
                    ESP_ERR_INVALID_ARG, "mb wrong slave ip address table.");
    MB_MASTER_CHECK(((comm_info_ptr->ip_addr_type == MB_IPV4) || (comm_info_ptr->ip_addr_type == MB_IPV6)),
                    ESP_ERR_INVALID_ARG, "mb incorrect addr type = (0x%x).", (uint8_t)comm_info_ptr->ip_addr_type);
    MB_MASTER_CHECK((comm_info_ptr->ip_netif_ptr != NULL),
                        ESP_ERR_INVALID_ARG, "mb incorrect iface address.");
    // Save the communication options
    mbm_opts->mbm_comm = *(mb_communication_info_t*)comm_info_ptr;
    return ESP_OK;
}

// Modbus controller stack start function
static esp_err_t mbc_tcp_master_start(void)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    eMBErrorCode status = MB_EIO;
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_CHECK((mbm_opts != NULL), ESP_ERR_INVALID_ARG, "mb incorrect options pointer.");
    const mb_communication_info_t* comm_info = (mb_communication_info_t*)&mbm_opts->mbm_comm;

    // Initialize Modbus stack using mbcontroller parameters
    status = eMBMasterTCPInit((USHORT)comm_info->ip_port);
    MB_MASTER_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack initialization failure, eMBMasterInit() returns (0x%x).", status);

    MB_MASTER_CHECK((mbm_opts->mbm_param_descriptor_size >= 1), ESP_ERR_INVALID_ARG, "mb table size is incorrect.");

    bool result = false;
    const char** comm_ip_table = (const char**)comm_info->ip_addr;
    MB_MASTER_CHECK((comm_ip_table != NULL), ESP_ERR_INVALID_ARG, "mb ip table address is incorrect.");

    eMBPortProto proto = (comm_info->ip_mode == MB_MODE_TCP) ? MB_PROTO_TCP : MB_PROTO_UDP;
    eMBPortIpVer ip_ver = (comm_info->ip_addr_type == MB_IPV4) ? MB_PORT_IPV4 : MB_PORT_IPV6;
    vMBTCPPortMasterSetNetOpt(comm_info->ip_netif_ptr, ip_ver, proto);

    status = eMBMasterEnable();
    MB_MASTER_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
                    "mb stack enable failure, eMBMasterEnable() returned (0x%x).", (uint32_t)status);

    // Add slave IP address for each slave to initialize connection
    mb_slave_addr_entry_t *p_slave_info;

    LIST_FOREACH(p_slave_info, &mbm_opts->mbm_slave_list, entries) {
        result = (BOOL)xMBTCPPortMasterAddSlaveIp(p_slave_info->index, p_slave_info->ip_address, p_slave_info->slave_addr);
        MB_MASTER_CHECK(result, ESP_ERR_INVALID_STATE, "mb stack add slave IP failed: %s.", *comm_ip_table);
    }

    // Add end of list condition
    (void)xMBTCPPortMasterAddSlaveIp(0xFF, NULL, 0xFF);

    // Wait for connection done event
    bool start = (bool)xMBTCPPortMasterWaitEvent(mbm_opts->mbm_event_group,
                                                    (EventBits_t)MB_EVENT_STACK_STARTED, MB_TCP_CONNECTION_TOUT);
    MB_MASTER_CHECK((start), ESP_ERR_INVALID_STATE,
                            "mb stack could not connect to slaves for %d seconds.",
                            CONFIG_FMB_TCP_CONNECTION_TOUT_SEC);
    return ESP_OK;
}

// Modbus controller destroy function
static esp_err_t mbc_tcp_master_destroy(void)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_CHECK((mbm_opts != NULL), ESP_ERR_INVALID_ARG, "mb incorrect options pointer.");
    eMBErrorCode mb_error = MB_ENOERR;

    // Disable and then destroy the Modbus stack
    mb_error = eMBMasterDisable();
    MB_MASTER_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack disable failure.");
    mb_error = eMBMasterClose();
    MB_MASTER_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE,
                    "mb stack close failure returned (0x%x).", (uint32_t)mb_error);
    // Stop polling by clearing correspondent bit in the event group
    xEventGroupClearBits(mbm_opts->mbm_event_group,
                         (EventBits_t)MB_EVENT_STACK_STARTED);
    (void)vTaskDelete(mbm_opts->mbm_task_handle);
    mbm_opts->mbm_task_handle = NULL;
    (void)vEventGroupDelete(mbm_opts->mbm_event_group);
    mbm_opts->mbm_event_group = NULL;
    mbc_tcp_master_free_slave_list();
    free(mbm_interface_ptr); // free the memory allocated for options
    vMBPortSetMode((UCHAR)MB_PORT_INACTIVE);
    mbm_interface_ptr = NULL;
    return ESP_OK;
}

// Set Modbus parameter description table
static esp_err_t mbc_tcp_master_set_descriptor(const mb_parameter_descriptor_t* descriptor, const uint16_t num_elements)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    MB_MASTER_CHECK((descriptor != NULL), ESP_ERR_INVALID_ARG, "mb incorrect descriptor.");
    MB_MASTER_CHECK((num_elements >= 1), ESP_ERR_INVALID_ARG, "mb table size is incorrect.");
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_CHECK((mbm_opts != NULL), ESP_ERR_INVALID_ARG, "mb options.");

    const char** comm_ip_table = (const char**)mbm_opts->mbm_comm.ip_addr;
    MB_MASTER_CHECK((comm_ip_table != NULL), ESP_ERR_INVALID_ARG, "mb ip table address is incorrect.");

    const mb_parameter_descriptor_t *reg_ptr = descriptor;
    uint16_t slave_cnt = 0;
    mb_slave_addr_entry_t* p_slave = NULL;

    // Go through all items in the table to check all Modbus registers
    for (int idx = 0; idx < (num_elements); idx++, reg_ptr++)
    {
        // Below is the code to check consistency of the table format and required fields.
        MB_MASTER_CHECK((reg_ptr->cid == idx), ESP_ERR_INVALID_ARG, "mb descriptor cid field is incorrect.");
        MB_MASTER_CHECK((reg_ptr->param_key != NULL), ESP_ERR_INVALID_ARG, "mb descriptor param key is incorrect.");
        MB_MASTER_CHECK((reg_ptr->mb_size > 0), ESP_ERR_INVALID_ARG, "mb descriptor param size is incorrect.");
        // Is the slave already in the list?
        p_slave = mbc_tcp_master_find_slave_addr(reg_ptr->mb_slave_addr);
        // Add it to slave list if not there.
        if (!p_slave) {
            // Is the IP address correctly defined for the slave?
            MB_MASTER_CHECK((comm_ip_table[slave_cnt]), ESP_ERR_INVALID_STATE, "mb missing IP address for cid #%d.", reg_ptr->cid);
            // Add slave to the list
            MB_MASTER_ASSERT(mbc_tcp_master_add_slave(idx, reg_ptr->mb_slave_addr, comm_ip_table[slave_cnt++]) == ESP_OK);
        }
    }
    mbm_opts->mbm_param_descriptor_table = descriptor;
    mbm_opts->mbm_param_descriptor_size = num_elements;
    return ESP_OK;
}

// Send custom Modbus request defined as mb_param_request_t structure
static esp_err_t mbc_tcp_master_send_request(mb_param_request_t* request, void* data_ptr)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_CHECK((request != NULL), ESP_ERR_INVALID_ARG, "mb request structure.");
    MB_MASTER_CHECK((data_ptr != NULL), ESP_ERR_INVALID_ARG, "mb incorrect data pointer.");

    eMBMasterReqErrCode mb_error = MB_MRE_NO_REG;
    esp_err_t error = ESP_FAIL;

    uint8_t mb_slave_addr = request->slave_addr;
    uint8_t mb_command = request->command;
    uint16_t mb_offset = request->reg_start;
    uint16_t mb_size = request->reg_size;

    // Set the buffer for callback function processing of received data
    mbm_opts->mbm_reg_buffer_ptr = (uint8_t*)data_ptr;
    mbm_opts->mbm_reg_buffer_size = mb_size;

    // Calls appropriate request function to send request and waits response
    switch(mb_command)
    {
    case MB_FUNC_READ_COILS:
        mb_error = eMBMasterReqReadCoils((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                         (USHORT)mb_size, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_WRITE_SINGLE_COIL:
        mb_error = eMBMasterReqWriteCoil((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                         *(USHORT *)data_ptr, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_WRITE_MULTIPLE_COILS:
        mb_error = eMBMasterReqWriteMultipleCoils((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                  (USHORT)mb_size, (UCHAR *)data_ptr,
                                                  (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_READ_DISCRETE_INPUTS:
        mb_error = eMBMasterReqReadDiscreteInputs((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                  (USHORT)mb_size, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_READ_HOLDING_REGISTER:
        mb_error = eMBMasterReqReadHoldingRegister((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                   (USHORT)mb_size, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_WRITE_REGISTER:
        mb_error = eMBMasterReqWriteHoldingRegister((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                    *(USHORT *)data_ptr, (LONG)MB_RESPONSE_TIMEOUT);
        break;

    case MB_FUNC_WRITE_MULTIPLE_REGISTERS:
        mb_error = eMBMasterReqWriteMultipleHoldingRegister((UCHAR)mb_slave_addr,
                                                            (USHORT)mb_offset, (USHORT)mb_size,
                                                            (USHORT *)data_ptr, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_READWRITE_MULTIPLE_REGISTERS:
        mb_error = eMBMasterReqReadWriteMultipleHoldingRegister((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                                (USHORT)mb_size, (USHORT *)data_ptr,
                                                                (USHORT)mb_offset, (USHORT)mb_size,
                                                                (LONG)MB_RESPONSE_TIMEOUT);
        break;
    case MB_FUNC_READ_INPUT_REGISTER:
        mb_error = eMBMasterReqReadInputRegister((UCHAR)mb_slave_addr, (USHORT)mb_offset,
                                                 (USHORT)mb_size, (LONG)MB_RESPONSE_TIMEOUT);
        break;
    default:
        ESP_LOGE(TAG, "%s: Incorrect function in request (%u) ",
                 __FUNCTION__, mb_command);
        mb_error = MB_MRE_NO_REG;
        break;
    }

    // Propagate the Modbus errors to higher level
    switch(mb_error)
    {
        case MB_MRE_NO_ERR:
            error = ESP_OK;
            break;

        case MB_MRE_NO_REG:
            error = ESP_ERR_NOT_SUPPORTED; // Invalid register request
            break;

        case MB_MRE_TIMEDOUT:
            error = ESP_ERR_TIMEOUT; // Slave did not send response
            break;

        case MB_MRE_EXE_FUN:
        case MB_MRE_REV_DATA:
            error = ESP_ERR_INVALID_RESPONSE; // Invalid response from slave
            break;

        case MB_MRE_MASTER_BUSY:
            error = ESP_ERR_INVALID_STATE; // Master is busy (previous request is pending)
            break;

        default:
            ESP_LOGE(TAG, "%s: Incorrect return code (%x) ", __FUNCTION__, mb_error);
            error = ESP_FAIL;
            break;
    }

    return error;
}

static esp_err_t mbc_tcp_master_get_cid_info(uint16_t cid, const mb_parameter_descriptor_t** param_buffer)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;

    MB_MASTER_CHECK((param_buffer != NULL), ESP_ERR_INVALID_ARG, "mb incorrect data buffer pointer.");
    MB_MASTER_CHECK((mbm_opts->mbm_param_descriptor_table != NULL), ESP_ERR_INVALID_ARG, "mb incorrect descriptor table or not set.");
    MB_MASTER_CHECK((cid < mbm_opts->mbm_param_descriptor_size), ESP_ERR_NOT_FOUND, "mb incorrect cid of characteristic.");

    // It is assumed that characteristics cid increased in the table
    const mb_parameter_descriptor_t* reg_info = &mbm_opts->mbm_param_descriptor_table[cid];

    MB_MASTER_CHECK((reg_info->param_key != NULL), ESP_ERR_INVALID_ARG, "mb incorrect characteristic key.");
    *param_buffer = reg_info;
    return ESP_OK;
}

// Helper function to get modbus command for each type of Modbus register area
static uint8_t mbc_tcp_master_get_command(mb_param_type_t param_type, mb_param_mode_t mode)
{
    uint8_t command = 0;
    switch(param_type)
    { //
        case MB_PARAM_HOLDING:
            command = (mode == MB_PARAM_WRITE) ? MB_FUNC_WRITE_MULTIPLE_REGISTERS : MB_FUNC_READ_HOLDING_REGISTER;
            break;
        case MB_PARAM_INPUT:
            command = MB_FUNC_READ_INPUT_REGISTER;
            break;
        case MB_PARAM_COIL:
            command = (mode == MB_PARAM_WRITE) ? MB_FUNC_WRITE_MULTIPLE_COILS : MB_FUNC_READ_COILS;
            break;
        case MB_PARAM_DISCRETE:
            if (mode != MB_PARAM_WRITE) {
                command = MB_FUNC_READ_DISCRETE_INPUTS;
            } else {
                ESP_LOGE(TAG, "%s: Incorrect mode (%u)", __FUNCTION__, (uint8_t)mode);
            }
            break;
        default:
            ESP_LOGE(TAG, "%s: Incorrect param type (%u)", __FUNCTION__, param_type);
            break;
    }
    return command;
}

// Helper function to set parameter buffer according to its type
static esp_err_t mbc_tcp_master_set_param_data(void* dest, void* src, mb_descr_type_t param_type, size_t param_size)
{
    esp_err_t err = ESP_OK;
    MB_MASTER_CHECK((dest != NULL), ESP_ERR_INVALID_ARG, "incorrect parameter pointer.");
    MB_MASTER_CHECK((src != NULL), ESP_ERR_INVALID_ARG, "incorrect parameter pointer.");
    // Transfer parameter data into value of characteristic
    switch(param_type)
    {
        case PARAM_TYPE_U8:
            *((uint8_t*)dest) = *((uint8_t*)src);
            break;
        case PARAM_TYPE_U16:
            *((uint16_t*)dest) = *((uint16_t*)src);
            break;
        case PARAM_TYPE_U32:
            *((uint32_t*)dest) = *((uint32_t*)src);
            break;
        case PARAM_TYPE_FLOAT:
            *((float*)dest) = *(float*)src;
            break;
        case PARAM_TYPE_ASCII:
            memcpy((void*)dest, (void*)src, (size_t)param_size);
            break;
        default:
            ESP_LOGE(TAG, "%s: Incorrect param type (%u).",
                        __FUNCTION__, (uint16_t)param_type);
            err = ESP_ERR_NOT_SUPPORTED;
            break;
    }
    return err;
}

// Helper to search parameter by name in the parameter description table and fills Modbus request fields accordingly
static esp_err_t mbc_tcp_master_set_request(char* name, mb_param_mode_t mode, mb_param_request_t* request,
                                                mb_parameter_descriptor_t* reg_data)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    esp_err_t error = ESP_ERR_NOT_FOUND;
    MB_MASTER_CHECK((name != NULL), ESP_ERR_INVALID_ARG, "mb incorrect parameter name.");
    MB_MASTER_CHECK((request != NULL), ESP_ERR_INVALID_ARG, "mb incorrect request parameter.");
    MB_MASTER_CHECK((mode <= MB_PARAM_WRITE), ESP_ERR_INVALID_ARG, "mb incorrect mode.");
    MB_MASTER_ASSERT(mbm_opts->mbm_param_descriptor_table != NULL);
    const mb_parameter_descriptor_t* reg_ptr = mbm_opts->mbm_param_descriptor_table;
    for (uint16_t counter = 0; counter < (mbm_opts->mbm_param_descriptor_size); counter++, reg_ptr++)
    {
        // Check the cid of the parameter is equal to record number in the table
        // Check the length of name and parameter key strings from table
        size_t param_key_len = strlen((const char*)reg_ptr->param_key);
        if (param_key_len != strlen((const char*)name)) {
            continue; // The length of strings is different then check next record in the table
        }
        // Compare the name of parameter with parameter key from table
        int comp_result = memcmp((const void*)name, (const void*)reg_ptr->param_key, (size_t)param_key_len);
        if (comp_result == 0) {
            // The correct line is found in the table and reg_ptr points to the found parameter description
            request->slave_addr = reg_ptr->mb_slave_addr;
            request->reg_start = reg_ptr->mb_reg_start;
            request->reg_size = reg_ptr->mb_size;
            request->command = mbc_tcp_master_get_command(reg_ptr->mb_param_type, mode);
            MB_MASTER_CHECK((request->command > 0), ESP_ERR_INVALID_ARG, "mb incorrect command or parameter type.");
            if (reg_data != NULL) {
                *reg_data = *reg_ptr; // Set the cid registered parameter data
            }
            error = ESP_OK;
            break;
        }
    }
    return error;
}

// Get parameter data for corresponding characteristic
static esp_err_t mbc_tcp_master_get_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t *type)
{
    MB_MASTER_CHECK((name != NULL), ESP_ERR_INVALID_ARG, "mb incorrect descriptor.");
    MB_MASTER_CHECK((type != NULL), ESP_ERR_INVALID_ARG, "type pointer is incorrect.");
    MB_MASTER_CHECK((value != NULL), ESP_ERR_INVALID_ARG, "value pointer is incorrect.");
    esp_err_t error = ESP_ERR_INVALID_RESPONSE;
    mb_param_request_t request ;
    mb_parameter_descriptor_t reg_info = { 0 };
    uint8_t* pdata = NULL;

    error = mbc_tcp_master_set_request(name, MB_PARAM_READ, &request, &reg_info);
    if ((error == ESP_OK) && (cid == reg_info.cid)) {
        // alloc buffer to store parameter data
        pdata = calloc(1, (reg_info.mb_size << 1));
        if (!pdata) {
            return ESP_ERR_INVALID_STATE;
        }
        error = mbc_tcp_master_send_request(&request, pdata);
        if (error == ESP_OK) {
            // If data pointer is NULL then we don't need to set value (it is still in the cache of cid)
            if (value != NULL) {
                error = mbc_tcp_master_set_param_data((void*)value, (void*)pdata,
                                                    reg_info.param_type, reg_info.param_size);
                if (error != ESP_OK) {
                    ESP_LOGE(TAG, "fail to set parameter data.");
                    error = ESP_ERR_INVALID_STATE;
                } else {
                    ESP_LOGD(TAG, "%s: Good response for get cid(%u) = %s",
                             __FUNCTION__, (unsigned)reg_info.cid, (char*)esp_err_to_name(error));
                }
            }
        } else {
            ESP_LOGD(TAG, "%s: Bad response to get cid(%u) = %s",
                     __FUNCTION__, reg_info.cid, (char*)esp_err_to_name(error));
            error = ESP_ERR_INVALID_RESPONSE;
        }
        free(pdata);
        // Set the type of parameter found in the table
        *type = reg_info.param_type;
    } else {
        ESP_LOGE(TAG, "%s: The cid(%u) not found in the data dictionary.",
                 __FUNCTION__, reg_info.cid);
        error = ESP_ERR_INVALID_ARG;
    }
    return error;
}

// Set parameter value for characteristic selected by name and cid
static esp_err_t mbc_tcp_master_set_parameter(uint16_t cid, char* name, uint8_t* value, uint8_t *type)
{
    MB_MASTER_CHECK((name != NULL), ESP_ERR_INVALID_ARG, "mb incorrect descriptor.");
    MB_MASTER_CHECK((value != NULL), ESP_ERR_INVALID_ARG, "value pointer is incorrect.");
    MB_MASTER_CHECK((type != NULL), ESP_ERR_INVALID_ARG, "type pointer is incorrect.");

    esp_err_t error = ESP_ERR_INVALID_RESPONSE;
    mb_param_request_t request ;
    mb_parameter_descriptor_t reg_info = { 0 };
    uint8_t* pdata = NULL;

    error = mbc_tcp_master_set_request(name, MB_PARAM_WRITE, &request, &reg_info);
    if ((error == ESP_OK) && (cid == reg_info.cid)) {
        pdata = calloc(1, (reg_info.mb_size << 1)); // alloc parameter buffer
        if (!pdata) {
            return ESP_ERR_INVALID_STATE;
        }
        // Transfer value of characteristic into parameter buffer
        error = mbc_tcp_master_set_param_data((void*)pdata, (void*)value,
                                              reg_info.param_type, reg_info.param_size);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "fail to set parameter data.");
            free(pdata);
            return ESP_ERR_INVALID_STATE;
        }
        // Send request to write characteristic data
        error = mbc_tcp_master_send_request(&request, pdata);
        if (error == ESP_OK) {
            ESP_LOGD(TAG, "%s: Good response for set cid(%u) = %s",
                                    __FUNCTION__, (unsigned)reg_info.cid, (char*)esp_err_to_name(error));
        } else {
            ESP_LOGD(TAG, "%s: Bad response to set cid(%u) = %s",
                                    __FUNCTION__, reg_info.cid, (char*)esp_err_to_name(error));
        }
        free(pdata);
        // Set the type of parameter found in the table
        *type = reg_info.param_type;
    } else {
        ESP_LOGE(TAG, "%s: The requested cid(%u) not found in the data dictionary.",
                                    __FUNCTION__, reg_info.cid);
        error = ESP_ERR_INVALID_ARG;
    }
    return error;
}

/* ----------------------- Callback functions for Modbus stack ---------------------------------*/
// These are executed by modbus stack to read appropriate type of registers.

/**
 * Modbus master input register callback function.
 *
 * @param pucRegBuffer input register buffer
 * @param usAddress input register address
 * @param usNRegs input register number
 *
 * @return result
 */
// Callback function for reading of MB Input Registers
eMBErrorCode eMBRegInputCBTcpMaster(UCHAR* pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_ASSERT(pucRegBuffer != NULL);
    USHORT usRegInputNregs = (USHORT)mbm_opts->mbm_reg_buffer_size; // Number of input registers to be transferred
    UCHAR* pucInputBuffer = (UCHAR*)mbm_opts->mbm_reg_buffer_ptr; // Get instance address
    USHORT usRegs = usNRegs;
    eMBErrorCode eStatus = MB_ENOERR;
    // If input or configuration parameters are incorrect then return an error to stack layer
    if ((pucInputBuffer != NULL)
            && (usNRegs >= 1)
            && (usRegInputNregs == usRegs)) {
        while (usRegs > 0) {
            _XFER_2_RD(pucInputBuffer, pucRegBuffer);
            usRegs -= 1;
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus master holding register callback function.
 *
 * @param pucRegBuffer holding register buffer
 * @param usAddress holding register address
 * @param usNRegs holding register number
 * @param eMode read or write
 *
 * @return result
 */
// Callback function for reading of MB Holding Registers
// Executed by stack when request to read/write holding registers is received
eMBErrorCode eMBRegHoldingCBTcpMaster(UCHAR *pucRegBuffer, USHORT usAddress,
                                      USHORT usNRegs, eMBRegisterMode eMode)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t *mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_ASSERT(pucRegBuffer != NULL);
    USHORT usRegHoldingNregs = (USHORT)mbm_opts->mbm_reg_buffer_size;
    UCHAR *pucHoldingBuffer = (UCHAR *)mbm_opts->mbm_reg_buffer_ptr;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT usRegs = usNRegs;
    // Check input and configuration parameters for correctness
    if ((pucHoldingBuffer != NULL) && (usRegHoldingNregs == usNRegs) && (usNRegs >= 1))
    {
        switch (eMode)
        {
        case MB_REG_WRITE:
            while (usRegs > 0)
            {
                _XFER_2_RD(pucRegBuffer, pucHoldingBuffer);
                usRegs -= 1;
            };
            break;
        case MB_REG_READ:
            while (usRegs > 0)
            {
                _XFER_2_WR(pucHoldingBuffer, pucRegBuffer);
                pucHoldingBuffer += 2;
                usRegs -= 1;
            };
            break;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus master coils callback function.
 *
 * @param pucRegBuffer coils buffer
 * @param usAddress coils address
 * @param usNCoils coils number
 * @param eMode read or write
 *
 * @return result
 */
// Callback function for reading of MB Coils Registers
eMBErrorCode eMBRegCoilsCBTcpMaster(UCHAR *pucRegBuffer, USHORT usAddress,
                                    USHORT usNCoils, eMBRegisterMode eMode)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_ASSERT(NULL != pucRegBuffer);
    USHORT usRegCoilNregs = (USHORT)mbm_opts->mbm_reg_buffer_size;
    UCHAR* pucRegCoilsBuf = (UCHAR*)mbm_opts->mbm_reg_buffer_ptr;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegIndex;
    USHORT usCoils = usNCoils;
    usAddress--; // The address is already + 1
    if ((usRegCoilNregs >= 1)
            && (pucRegCoilsBuf != NULL)
            && (usNCoils == usRegCoilNregs)) {
        iRegIndex = (usAddress % 8);
        switch (eMode) {
            case MB_REG_WRITE:
                while (usCoils > 0) {
                    UCHAR ucResult = xMBUtilGetBits((UCHAR*)pucRegCoilsBuf, iRegIndex, 1);
                    xMBUtilSetBits(pucRegBuffer, iRegIndex - (usAddress % 8) , 1, ucResult);
                    iRegIndex++;
                    usCoils--;
                }
                break;
            case MB_REG_READ:
                while (usCoils > 0) {
                    UCHAR ucResult = xMBUtilGetBits(pucRegBuffer, iRegIndex - (usAddress % 8), 1);
                    xMBUtilSetBits((uint8_t*)pucRegCoilsBuf, iRegIndex, 1, ucResult);
                    iRegIndex++;
                    usCoils--;
                }
                break;
        } // switch ( eMode )
    } else {
        // If the configuration or input parameters are incorrect then return error to stack
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

/**
 * Modbus master discrete callback function.
 *
 * @param pucRegBuffer discrete buffer
 * @param usAddress discrete address
 * @param usNDiscrete discrete number
 *
 * @return result
 */
// Callback function for reading of MB Discrete Input Registers
eMBErrorCode eMBRegDiscreteCBTcpMaster(UCHAR * pucRegBuffer, USHORT usAddress,
                                       USHORT usNDiscrete)
{
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    MB_MASTER_ASSERT(pucRegBuffer != NULL);
    USHORT usRegDiscreteNregs = (USHORT)mbm_opts->mbm_reg_buffer_size;
    UCHAR* pucRegDiscreteBuf = (UCHAR*)mbm_opts->mbm_reg_buffer_ptr;
    eMBErrorCode eStatus = MB_ENOERR;
    USHORT iRegBitIndex, iNReg;
    UCHAR* pucDiscreteInputBuf;
    iNReg = usNDiscrete / 8 + 1;
    pucDiscreteInputBuf = (UCHAR*) pucRegDiscreteBuf;
    // It is already plus one in Modbus function method.
    usAddress--;
    if ((usRegDiscreteNregs >= 1)
            && (pucRegDiscreteBuf != NULL)
            && (usNDiscrete >= 1)) {
        iRegBitIndex = (USHORT)(usAddress) % 8; // Get bit index
        while (iNReg > 1)
        {
            xMBUtilSetBits(pucDiscreteInputBuf++, iRegBitIndex, 8, *pucRegBuffer++);
            iNReg--;
        }
        // last discrete
        usNDiscrete = usNDiscrete % 8;
        // xMBUtilSetBits has bug when ucNBits is zero
        if (usNDiscrete != 0)
        {
            xMBUtilSetBits(pucDiscreteInputBuf, iRegBitIndex, usNDiscrete, *pucRegBuffer++);
        }
    } else {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

// Initialization of resources for Modbus TCP master controller
esp_err_t mbc_tcp_master_create(void** handler)
{
    // Allocate space for master interface structure
    if (mbm_interface_ptr == NULL) {
        mbm_interface_ptr = malloc(sizeof(mb_master_interface_t));
    }
    MB_MASTER_ASSERT(mbm_interface_ptr != NULL);

    // Initialize interface properties
    mb_master_options_t* mbm_opts = &mbm_interface_ptr->opts;
    mbm_opts->port_type = MB_PORT_TCP_MASTER;

    vMBPortSetMode((UCHAR)MB_PORT_TCP_MASTER);

    mbm_opts->mbm_comm.ip_mode = MB_MODE_TCP;
    mbm_opts->mbm_comm.ip_port = MB_TCP_DEFAULT_PORT;

    // Initialization of active context of the modbus controller
    BaseType_t status = 0;
    // Parameter change notification queue
    mbm_opts->mbm_event_group = xEventGroupCreate();
    MB_MASTER_CHECK((mbm_opts->mbm_event_group != NULL), ESP_ERR_NO_MEM, "mb event group error.");
    // Create modbus controller task
    status = xTaskCreate((void*)&modbus_tcp_master_task,
                            "modbus_tcp_master_task",
                            MB_CONTROLLER_STACK_SIZE,
                            NULL, // No parameters
                            MB_CONTROLLER_PRIORITY,
                            &mbm_opts->mbm_task_handle);
    if (status != pdPASS) {
        vTaskDelete(mbm_opts->mbm_task_handle);
        MB_MASTER_CHECK((status == pdPASS), ESP_ERR_NO_MEM,
                        "mb controller task creation error, xTaskCreate() returns (0x%x).",
                        (uint32_t)status);
    }
    MB_MASTER_ASSERT(mbm_opts->mbm_task_handle != NULL); // The task is created but handle is incorrect

    LIST_INIT(&mbm_opts->mbm_slave_list); // Init slave address list
    mbm_opts->mbm_slave_list_count = 0;

    // Initialize public interface methods of the interface
    mbm_interface_ptr->init = mbc_tcp_master_create;
    mbm_interface_ptr->destroy = mbc_tcp_master_destroy;
    mbm_interface_ptr->setup = mbc_tcp_master_setup;
    mbm_interface_ptr->start = mbc_tcp_master_start;
    mbm_interface_ptr->get_cid_info = mbc_tcp_master_get_cid_info;
    mbm_interface_ptr->get_parameter = mbc_tcp_master_get_parameter;
    mbm_interface_ptr->send_request = mbc_tcp_master_send_request;
    mbm_interface_ptr->set_descriptor = mbc_tcp_master_set_descriptor;
    mbm_interface_ptr->set_parameter = mbc_tcp_master_set_parameter;

    mbm_interface_ptr->master_reg_cb_discrete = eMBRegDiscreteCBTcpMaster;
    mbm_interface_ptr->master_reg_cb_input = eMBRegInputCBTcpMaster;
    mbm_interface_ptr->master_reg_cb_holding = eMBRegHoldingCBTcpMaster;
    mbm_interface_ptr->master_reg_cb_coils = eMBRegCoilsCBTcpMaster;

    *handler = mbm_interface_ptr;

    return ESP_OK;
}

#endif //#if MB_MASTER_TCP_ENABLED
