/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MB_CONTROLLER_MASTER_H
#define _MB_CONTROLLER_MASTER_H

#include <sys/queue.h>              // for list
#include "freertos/FreeRTOS.h"      // for task creation and queue access
#include "freertos/task.h"          // for task api access
#include "freertos/event_groups.h"  // for event groups
#include "driver/uart.h"            // for UART types
#include "errno.h"                  // for errno
#include "esp_log.h"                // for log write
#include "string.h"                 // for strerror()
#include "esp_modbus_common.h"      // for common types
#include "esp_modbus_master.h"      // for public master types
#include "esp_modbus_callbacks.h"

/* ----------------------- Defines ------------------------------------------*/

/**
 * @brief Request mode for parameter to use in data dictionary
 */
typedef enum {
    MB_PARAM_READ, /*!< Read parameter values. */
    MB_PARAM_WRITE /*!< Write parameter values. */
} mb_param_mode_t;

/**
 * @brief Device communication parameters for master
 */
typedef struct {
    mb_mode_type_t mode;                    /*!< Modbus communication mode */
    uint8_t dummy;                          /*!< Dummy field */
    uart_port_t port;                       /*!< Modbus communication port (UART) number */
    uint32_t baudrate;                      /*!< Modbus baudrate */
    uart_parity_t parity;                   /*!< Modbus UART parity settings */
} mb_master_comm_info_t;

#if MB_MASTER_TCP_ENABLED
/**
 * @brief Modbus slave addr list item for the master
 */
typedef struct mb_slave_addr_entry_s{
    uint16_t index;                             /*!< Index of the slave address */
    const char* ip_address;                     /*!< IP address string of the slave */
    uint8_t slave_addr;                         /*!< Short slave address */
    void* p_data;                               /*!< pointer to data structure */
    LIST_ENTRY(mb_slave_addr_entry_s) entries;  /*!< The slave address entry */
} mb_slave_addr_entry_t;
#endif

/**
 * @brief Modbus controller handler structure
 */
typedef struct {
    mb_port_type_t port_type;                           /*!< Modbus port type */
    mb_communication_info_t mbm_comm;                   /*!< Modbus communication info */
    uint8_t* mbm_reg_buffer_ptr;                        /*!< Modbus data buffer pointer */
    uint16_t mbm_reg_buffer_size;                       /*!< Modbus data buffer size */
    TaskHandle_t mbm_task_handle;                       /*!< Modbus task handle */
    EventGroupHandle_t mbm_event_group;                 /*!< Modbus controller event group */
    const mb_parameter_descriptor_t* mbm_param_descriptor_table; /*!< Modbus controller parameter description table */
    size_t mbm_param_descriptor_size;                   /*!< Modbus controller parameter description table size*/
#if MB_MASTER_TCP_ENABLED
    LIST_HEAD(mbm_slave_addr_info_, mb_slave_addr_entry_s) mbm_slave_list; /*!< Slave address information list */
    uint16_t mbm_slave_list_count;
#endif
} mb_master_options_t;

typedef esp_err_t (*iface_get_cid_info)(uint16_t, const mb_parameter_descriptor_t**); /*!< Interface get_cid_info method */
typedef esp_err_t (*iface_get_parameter)(uint16_t, char*, uint8_t*, uint8_t*);        /*!< Interface get_parameter method */
typedef esp_err_t (*iface_send_request)(mb_param_request_t*, void*);                  /*!< Interface send_request method */
typedef esp_err_t (*iface_set_descriptor)(const mb_parameter_descriptor_t*, const uint16_t); /*!< Interface set_descriptor method */
typedef esp_err_t (*iface_set_parameter)(uint16_t, char*, uint8_t*, uint8_t*);        /*!< Interface set_parameter method */

/**
 * @brief Modbus controller interface structure
 */
typedef struct {
    // Master object interface options
    mb_master_options_t opts;

    // Public interface methods
    iface_init init;                        /*!< Interface method init */
    iface_destroy destroy;                  /*!< Interface method destroy */
    iface_setup setup;                      /*!< Interface method setup */
    iface_start start;                      /*!< Interface method start */
    iface_get_cid_info get_cid_info;        /*!< Interface get_cid_info method */
    iface_get_parameter get_parameter;      /*!< Interface get_parameter method */
    iface_send_request send_request;        /*!< Interface send_request method */
    iface_set_descriptor set_descriptor;    /*!< Interface set_descriptor method */
    iface_set_parameter set_parameter;      /*!< Interface set_parameter method */
    // Modbus register calback function pointers
    reg_discrete_cb master_reg_cb_discrete; /*!< Stack callback discrete rw method */
    reg_input_cb master_reg_cb_input;       /*!< Stack callback input rw method */
    reg_holding_cb master_reg_cb_holding;   /*!< Stack callback holding rw method */
    reg_coils_cb master_reg_cb_coils;       /*!< Stack callback coils rw method */
} mb_master_interface_t;

#endif //_MB_CONTROLLER_MASTER_H
