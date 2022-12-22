/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _MB_CONTROLLER_SLAVE_H
#define _MB_CONTROLLER_SLAVE_H

#include "driver/uart.h"    // for uart defines
#include "errno.h"          // for errno
#include "sys/queue.h"      // for list
#include "esp_log.h"        // for log write
#include "string.h"         // for strerror()

#include "esp_modbus_slave.h"    // for public type defines
#include "esp_modbus_callbacks.h"   // for callback functions

/* ----------------------- Defines ------------------------------------------*/
#define MB_INST_MIN_SIZE                    (2) // The minimal size of Modbus registers area in bytes
#define MB_INST_MAX_SIZE                    (65535 * 2) // The maximum size of Modbus area in bytes

#define MB_CONTROLLER_NOTIFY_QUEUE_SIZE     (CONFIG_FMB_CONTROLLER_NOTIFY_QUEUE_SIZE) // Number of messages in parameter notification queue
#define MB_CONTROLLER_NOTIFY_TIMEOUT        (pdMS_TO_TICKS(CONFIG_FMB_CONTROLLER_NOTIFY_TIMEOUT)) // notification timeout

/**
 * @brief Device communication parameters for master
 */
typedef struct {
    mb_mode_type_t mode;                    /*!< Modbus communication mode */
    uint8_t slave_addr;                     /*!< Slave address field */
    uart_port_t port;                       /*!< Modbus communication port (UART) number */
    uint32_t baudrate;                      /*!< Modbus baudrate */
    uart_parity_t parity;                   /*!< Modbus UART parity settings */
} mb_slave_comm_info_t;

/**
 * @brief Modbus area descriptor list item
 */
typedef struct mb_descr_entry_s{
    uint16_t start_offset;                  /*!< Modbus start address for area descriptor */
    mb_param_type_t type;                   /*!< Type of storage area descriptor */
    void* p_data;                           /*!< Instance address for storage area descriptor */
    size_t size;                            /*!< Instance size for area descriptor (bytes) */
    LIST_ENTRY(mb_descr_entry_s) entries;    /*!< The Modbus area descriptor entry */
} mb_descr_entry_t;

/**
 * @brief Modbus controller handler structure
 */
typedef struct {
    mb_port_type_t port_type;                           /*!< port type */
    mb_communication_info_t mbs_comm;                   /*!< communication info */
    TaskHandle_t mbs_task_handle;                       /*!< task handle */
    EventGroupHandle_t mbs_event_group;                 /*!< controller event group */
    QueueHandle_t mbs_notification_queue_handle;        /*!< controller notification queue */
    LIST_HEAD(mbs_area_descriptors_, mb_descr_entry_s) mbs_area_descriptors[MB_PARAM_COUNT]; /*!< register area descriptors */
} mb_slave_options_t;

typedef mb_event_group_t (*iface_check_event)(mb_event_group_t);          /*!< Interface method check_event */
typedef esp_err_t (*iface_get_param_info)(mb_param_info_t*, uint32_t);    /*!< Interface method get_param_info */
typedef esp_err_t (*iface_set_descriptor)(mb_register_area_descriptor_t); /*!< Interface method set_descriptor */

/**
 * @brief Request mode for parameter to use in data dictionary
 */
typedef struct
{
    mb_slave_options_t opts;                                    /*!< Modbus slave options */

    // Functional pointers to internal static functions of the implementation (public interface methods)
    iface_init init;                        /*!< Interface method init */
    iface_destroy destroy;                  /*!< Interface method destroy */
    iface_setup setup;                      /*!< Interface method setup */
    iface_start start;                      /*!< Interface method start */
    iface_check_event check_event;          /*!< Interface method check_event */
    iface_get_param_info get_param_info;    /*!< Interface method get_param_info */
    iface_set_descriptor set_descriptor;    /*!< Interface method set_descriptor */

    // Modbus register calback function pointers
    reg_discrete_cb slave_reg_cb_discrete;  /*!< Stack callback discrete rw method */
    reg_input_cb slave_reg_cb_input;        /*!< Stack callback input rw method */
    reg_holding_cb slave_reg_cb_holding;    /*!< Stack callback holding rw method */
    reg_coils_cb slave_reg_cb_coils;        /*!< Stack callback coils rw method */
} mb_slave_interface_t;

#endif
