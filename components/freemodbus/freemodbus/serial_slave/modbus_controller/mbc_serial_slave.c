/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// mbc_serial_slave.c
// Implementation of the Modbus controller serial slave

#include <sys/time.h>               // for calculation of time stamp in milliseconds
#include "esp_log.h"                // for log_write
#include "mb.h"                     // for mb types definition
#include "mbutils.h"                // for mbutils functions definition for stack callback
#include "sdkconfig.h"              // for KConfig values
#include "esp_modbus_common.h"      // for common defines
#include "esp_modbus_slave.h"       // for public slave interface types
#include "mbc_slave.h"              // for private slave interface types
#include "mbc_serial_slave.h"       // for serial slave implementation definitions
#include "port_serial_slave.h"

// Shared pointer to interface structure
static mb_slave_interface_t* mbs_interface_ptr = NULL;
static const char *TAG = "MB_CONTROLLER_SLAVE";

// Modbus task function
static void modbus_slave_task(void *pvParameters)
{
    // Modbus interface must be initialized before start
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;

    MB_SLAVE_ASSERT(mbs_opts != NULL);
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
            // Send response buffer
            BOOL xSentState = xMBPortSerialTxPoll();
            if (xSentState) {
                (void)xMBPortEventPost( EV_FRAME_SENT );
            }
        }
    }
}

// Setup Modbus controller parameters
static esp_err_t mbc_serial_slave_setup(void* comm_info)
{
    MB_SLAVE_CHECK((mbs_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    MB_SLAVE_CHECK((comm_info != NULL), ESP_ERR_INVALID_ARG,
                    "mb wrong communication settings.");
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    mb_slave_comm_info_t* comm_settings = (mb_slave_comm_info_t*)comm_info;
    MB_SLAVE_CHECK(((comm_settings->mode == MB_MODE_RTU) || (comm_settings->mode == MB_MODE_ASCII)),
                ESP_ERR_INVALID_ARG, "mb incorrect mode = (0x%x).",
                (uint32_t)comm_settings->mode);
    MB_SLAVE_CHECK((comm_settings->slave_addr <= MB_ADDRESS_MAX),
                ESP_ERR_INVALID_ARG, "mb wrong slave address = (0x%x).",
                (uint32_t)comm_settings->slave_addr);
    MB_SLAVE_CHECK((comm_settings->port < UART_NUM_MAX), ESP_ERR_INVALID_ARG,
                "mb wrong port to set = (0x%x).", (uint32_t)comm_settings->port);
    MB_SLAVE_CHECK((comm_settings->parity <= UART_PARITY_ODD), ESP_ERR_INVALID_ARG,
                "mb wrong parity option = (0x%x).", (uint32_t)comm_settings->parity);

    // Set communication options of the controller
    mbs_opts->mbs_comm = *(mb_communication_info_t*)comm_settings;
    return ESP_OK;
}

// Start Modbus controller start function
static esp_err_t mbc_serial_slave_start(void)
{
    MB_SLAVE_CHECK((mbs_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    eMBErrorCode status = MB_EIO;
    const mb_communication_info_t* comm_info = (mb_communication_info_t*)&mbs_opts->mbs_comm;

    // Initialize Modbus stack using mbcontroller parameters
    status = eMBInit((eMBMode)comm_info->mode,
                         (UCHAR)comm_info->slave_addr,
                         (UCHAR)comm_info->port,
                         (ULONG)comm_info->baudrate,
                         MB_PORT_PARITY_GET(comm_info->parity));

    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack initialization failure, eMBInit() returns (0x%x).", status);
    status = eMBEnable();
    MB_SLAVE_CHECK((status == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack set slave ID failure, eMBEnable() returned (0x%x).", (uint32_t)status);
    // Set the mbcontroller start flag
    EventBits_t flag = xEventGroupSetBits(mbs_opts->mbs_event_group,
                                            (EventBits_t)MB_EVENT_STACK_STARTED);
    MB_SLAVE_CHECK((flag & MB_EVENT_STACK_STARTED),
                ESP_ERR_INVALID_STATE, "mb stack start event set error.");
    return ESP_OK;
}

// Blocking function to get event on parameter group change for application task
static mb_event_group_t mbc_serial_slave_check_event(mb_event_group_t group)
{
    MB_SLAVE_CHECK((mbs_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    MB_SLAVE_ASSERT(mbs_opts->mbs_event_group != NULL);
    BaseType_t status = xEventGroupWaitBits(mbs_opts->mbs_event_group, (BaseType_t)group,
                                            pdTRUE , pdFALSE, portMAX_DELAY);
    return (mb_event_group_t)status;
}

// Function to get notification about parameter change from application task
static esp_err_t mbc_serial_slave_get_param_info(mb_param_info_t* reg_info, uint32_t timeout)
{
    MB_SLAVE_CHECK((mbs_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
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

// Modbus controller destroy function
static esp_err_t mbc_serial_slave_destroy(void)
{
    MB_SLAVE_CHECK((mbs_interface_ptr != NULL),
                    ESP_ERR_INVALID_STATE,
                    "Slave interface is not correctly initialized.");
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
    mb_error = eMBClose();
    MB_SLAVE_CHECK((mb_error == MB_ENOERR), ESP_ERR_INVALID_STATE,
            "mb stack close failure returned (0x%x).", (uint32_t)mb_error);
    mbs_interface_ptr = NULL;
    vMBPortSetMode((UCHAR)MB_PORT_INACTIVE);
    return ESP_OK;
}

// Initialization of Modbus controller
esp_err_t mbc_serial_slave_create(void** handler)
{
    // Allocate space for options
    if (mbs_interface_ptr == NULL) {
        mbs_interface_ptr = malloc(sizeof(mb_slave_interface_t));
    }
    MB_SLAVE_ASSERT(mbs_interface_ptr != NULL);

    vMBPortSetMode((UCHAR)MB_PORT_SERIAL_SLAVE);

    mb_slave_options_t* mbs_opts = &mbs_interface_ptr->opts;
    mbs_opts->port_type = MB_PORT_SERIAL_SLAVE; // set interface port type

    // Set default values of communication options
    mbs_opts->mbs_comm.mode = MB_MODE_RTU;
    mbs_opts->mbs_comm.slave_addr = MB_DEVICE_ADDRESS;
    mbs_opts->mbs_comm.port = MB_UART_PORT;
    mbs_opts->mbs_comm.baudrate = MB_DEVICE_SPEED;
    mbs_opts->mbs_comm.parity = MB_PARITY_NONE;

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
    status = xTaskCreatePinnedToCore((void*)&modbus_slave_task,
                            "modbus_slave_task",
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
    MB_SLAVE_ASSERT(mbs_opts->mbs_task_handle != NULL); // The task is created but handle is incorrect

    // Initialize interface function pointers
    mbs_interface_ptr->check_event = mbc_serial_slave_check_event;
    mbs_interface_ptr->destroy = mbc_serial_slave_destroy;
    mbs_interface_ptr->get_param_info = mbc_serial_slave_get_param_info;
    mbs_interface_ptr->init = mbc_serial_slave_create;
    mbs_interface_ptr->set_descriptor = NULL; // Use common set descriptor function
    mbs_interface_ptr->setup = mbc_serial_slave_setup;
    mbs_interface_ptr->start = mbc_serial_slave_start;

    // Initialize stack callback function pointers
    mbs_interface_ptr->slave_reg_cb_discrete = NULL; // implemented in common layer
    mbs_interface_ptr->slave_reg_cb_input = NULL;
    mbs_interface_ptr->slave_reg_cb_holding = NULL;
    mbs_interface_ptr->slave_reg_cb_coils = NULL;

    *handler = (void*)mbs_interface_ptr;

    return ESP_OK;
}
