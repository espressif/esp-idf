/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// mbc_tcp_slave.c
// Implementation of the Modbus controller TCP slave

#include <sys/time.h>               // for calculation of time stamp in milliseconds
#include "esp_log.h"                // for log_write
#include "mb.h"                     // for mb types definition
#include "mbutils.h"                // for mbutils functions definition for stack callback
#include "port.h"                   // for port callback functions and defines
#include "sdkconfig.h"              // for KConfig values
#include "esp_modbus_common.h"      // for common defines
#include "esp_modbus_slave.h"       // for public slave interface types
#include "mbc_slave.h"              // for private slave interface types
#include "mbc_tcp_slave.h"          // for tcp slave mb controller defines
#include "port_tcp_slave.h"         // for tcp slave port defines

#if MB_TCP_ENABLED

// Shared pointer to interface structure
static mb_slave_interface_t* mbs_interface_ptr = NULL;
static const char *TAG = "MB_CONTROLLER_SLAVE";

// Modbus task function
static void modbus_tcp_slave_task(void *pvParameters)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;

    // Main Modbus stack processing cycle
    for (;;) {
        BaseType_t status = xEventGroupWaitBits(mbs_opts->mbs_event_group,
                                                (BaseType_t)(MB_EVENT_STACK_STARTED),
                                                pdFALSE, // do not clear bits
                                                pdFALSE,
                                                portMAX_DELAY);
        // Check if stack started then poll for data
        if (status & MB_EVENT_STACK_STARTED) {
            (void)eMBPoll(); // allow stack to process data
        }
    }
}

// Setup Modbus controller parameters
static esp_err_t mbc_tcp_slave_setup(void* comm_info)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    MB_SLAVE_CHECK((comm_info != NULL), ESP_ERR_INVALID_ARG,
                    "mb wrong communication settings.");
    mb_communication_info_t* comm_settings = (mb_communication_info_t*)comm_info;
    MB_SLAVE_CHECK((comm_settings->ip_mode == MB_MODE_TCP),
                ESP_ERR_INVALID_ARG, "mb incorrect mode = (0x%x).", (uint8_t)comm_settings->ip_mode);
    MB_SLAVE_CHECK(((comm_settings->ip_addr_type == MB_IPV4) || (comm_settings->ip_addr_type == MB_IPV6)),
                    ESP_ERR_INVALID_ARG, "mb incorrect addr type = (0x%x).", (uint8_t)comm_settings->ip_addr_type);
    MB_SLAVE_CHECK((comm_settings->ip_netif_ptr != NULL),
                        ESP_ERR_INVALID_ARG, "mb incorrect iface address.");
    // Set communication options of the controller
    mbs_opts->mbs_comm = *comm_settings;
    return ESP_OK;
}

// Start Modbus controller start function
static esp_err_t mbc_tcp_slave_start(void)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    eMBErrorCode status = MB_EIO;

    // Initialize Modbus stack using mbcontroller parameters
    status = eMBTCPInit((USHORT)mbs_opts->mbs_comm.ip_port);
    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack initialization failure, eMBInit() returns (0x%x).", status);

    eMBPortProto proto = (mbs_opts->mbs_comm.ip_mode == MB_MODE_TCP) ? MB_PROTO_TCP : MB_PROTO_UDP;
    eMBPortIpVer ip_ver = (mbs_opts->mbs_comm.ip_addr_type == MB_IPV4) ? MB_PORT_IPV4 : MB_PORT_IPV6;
    vMBTCPPortSlaveSetNetOpt(mbs_opts->mbs_comm.ip_netif_ptr, ip_ver, proto, (char*)mbs_opts->mbs_comm.ip_addr);

    status = eMBEnable();
    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb TCP stack start failure, eMBEnable() returned (0x%x).", (uint32_t)status);
    // Set the mbcontroller start flag
    EventBits_t flag = xEventGroupSetBits(mbs_opts->mbs_event_group,
                                            (EventBits_t)MB_EVENT_STACK_STARTED);
    MB_SLAVE_CHECK((flag & MB_EVENT_STACK_STARTED),
                ESP_ERR_INVALID_STATE, "mb stack start event set error.");
    return ESP_OK;
}

// Modbus controller destroy function
static esp_err_t mbc_tcp_slave_destroy(void)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    eMBErrorCode mb_error = MB_ENOERR;
    // Stop polling by clearing correspondent bit in the event group
    EventBits_t flag = xEventGroupClearBits(mbs_opts->mbs_event_group,
                                    (EventBits_t)MB_EVENT_STACK_STARTED);
    MB_SLAVE_CHECK((flag & MB_EVENT_STACK_STARTED),
                ESP_ERR_INVALID_STATE, "mb stack stop event failure.");
    // Disable and then destroy the Modbus stack
    mb_error = eMBDisable();
    MB_SLAVE_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE, "mb stack disable failure.");
    (void)vTaskDelete(mbs_opts->mbs_task_handle);
    (void)vQueueDelete(mbs_opts->mbs_notification_queue_handle);
    (void)vEventGroupDelete(mbs_opts->mbs_event_group);
    (void)vMBTCPPortClose();
    mbs_interface_ptr = NULL;
    vMBPortSetMode((UCHAR)MB_PORT_INACTIVE);
    return ESP_OK;
}

// Blocking function to get event on parameter group change for application task
static mb_event_group_t mbc_tcp_slave_check_event(mb_event_group_t group)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    MB_SLAVE_ASSERT(mbs_opts->mbs_event_group != NULL);
    BaseType_t status = xEventGroupWaitBits(mbs_opts->mbs_event_group, (BaseType_t)group,
                                            pdTRUE , pdFALSE, portMAX_DELAY);
    return (mb_event_group_t)status;
}

// Function to get notification about parameter change from application task
static esp_err_t mbc_tcp_slave_get_param_info(mb_param_info_t* reg_info, uint32_t timeout)
{
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    esp_err_t err = ESP_ERR_TIMEOUT;
    MB_SLAVE_CHECK((mbs_opts->mbs_notification_queue_handle != NULL),
                ESP_ERR_INVALID_ARG, "mb queue handle is invalid.");
    MB_SLAVE_CHECK((reg_info != NULL), ESP_ERR_INVALID_ARG, "mb register information is invalid.");
    BaseType_t status = xQueueReceive(mbs_opts->mbs_notification_queue_handle,
                                        reg_info, pdMS_TO_TICKS(timeout));
    if (status == pdTRUE) {
        err = ESP_OK;
    }
    return err;
}

/* ----------------------- Callback functions for Modbus stack ---------------------------------*/
// These are executed by modbus stack to read appropriate type of registers.

// This is required to suppress warning when register start address is zero
#pragma GCC diagnostic ignored "-Wtype-limits"

// Initialization of Modbus controller
esp_err_t mbc_tcp_slave_create(void** handler)
{
    // Allocate space for options
    if (mbs_interface_ptr == NULL) {
        mbs_interface_ptr = malloc(sizeof(mb_slave_interface_t));
    }
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    mbs_opts->port_type = MB_PORT_TCP_SLAVE; // set interface port type

    vMBPortSetMode((UCHAR)MB_PORT_TCP_SLAVE);

    // Set default values of communication options
    mbs_opts->mbs_comm.ip_port = MB_TCP_DEFAULT_PORT;

    // Initialization of active context of the Modbus controller
    BaseType_t status = 0;

    // Parameter change notification queue
    mbs_opts->mbs_event_group = xEventGroupCreate();
    MB_SLAVE_CHECK((mbs_opts->mbs_event_group != NULL),
            ESP_ERR_NO_MEM, "mb event group error.");
    // Parameter change notification queue
    mbs_opts->mbs_notification_queue_handle = xQueueCreate(
                                                MB_CONTROLLER_NOTIFY_QUEUE_SIZE,
                                                sizeof(mb_param_info_t));
    MB_SLAVE_CHECK((mbs_opts->mbs_notification_queue_handle != NULL),
            ESP_ERR_NO_MEM, "mb notify queue creation error.");
    // Create Modbus controller task
    status = xTaskCreatePinnedToCore((void*)&modbus_tcp_slave_task,
                            "modbus_tcp_slave_task",
                            MB_CONTROLLER_STACK_SIZE,
                            NULL,
                            MB_CONTROLLER_PRIORITY,
                            &mbs_opts->mbs_task_handle,
                            MB_PORT_TASK_AFFINITY);
    if (status != pdPASS) {
        vTaskDelete(mbs_opts->mbs_task_handle);
        MB_SLAVE_CHECK((status == pdPASS), ESP_ERR_NO_MEM,
                "mb controller task creation error, xTaskCreate() returns (0x%x).",
                (uint32_t)status);
    }

    // The task is created but handle is incorrect
    MB_SLAVE_ASSERT(mbs_opts->mbs_task_handle != NULL);

    // Initialization of interface pointers
    mbs_interface_ptr->init = mbc_tcp_slave_create;
    mbs_interface_ptr->destroy = mbc_tcp_slave_destroy;
    mbs_interface_ptr->setup = mbc_tcp_slave_setup;
    mbs_interface_ptr->start = mbc_tcp_slave_start;
    mbs_interface_ptr->check_event = mbc_tcp_slave_check_event;
    mbs_interface_ptr->get_param_info = mbc_tcp_slave_get_param_info;
    mbs_interface_ptr->set_descriptor = NULL; // Use common descriptor setter

    // Initialize stack callback function pointers
    mbs_interface_ptr->slave_reg_cb_discrete = NULL; // implemented in common layer
    mbs_interface_ptr->slave_reg_cb_input = NULL;
    mbs_interface_ptr->slave_reg_cb_holding = NULL;
    mbs_interface_ptr->slave_reg_cb_coils = NULL;

    *handler = (void*)mbs_interface_ptr;

    return ESP_OK;
}

#endif //#if MB_TCP_ENABLED
