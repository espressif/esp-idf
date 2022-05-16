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

#ifndef _ESP_MB_SLAVE_INTERFACE_H
#define _ESP_MB_SLAVE_INTERFACE_H

// Public interface header for slave
#include <stdint.h>                 // for standard int types definition
#include <stddef.h>                 // for NULL and std defines
#include "soc/soc.h"                // for BITN definitions
#include "freertos/FreeRTOS.h"      // for task creation and queues access
#include "freertos/event_groups.h"  // for event groups
#include "esp_modbus_common.h"      // for common types

#ifdef __cplusplus
extern "C" {
#endif

#define MB_SLAVE_CHECK(a, err_code, format, ...) MB_RETURN_ON_FALSE(a, err_code, TAG, format __VA_OPT__(,) __VA_ARGS__)

#define MB_SLAVE_ASSERT(con) do { \
        if (!(con)) { ESP_LOGE(TAG, "assert errno:%d, errno_str: !(%s)", errno, strerror(errno)); assert(0 && #con); } \
    } while (0)

/**
 * @brief Parameter access event information type
 */
typedef struct {
    uint32_t time_stamp;                    /*!< Timestamp of Modbus Event (uS)*/
    uint16_t mb_offset;                     /*!< Modbus register offset */
    mb_event_group_t type;                  /*!< Modbus event type */
    uint8_t* address;                       /*!< Modbus data storage address */
    size_t size;                            /*!< Modbus event register size (number of registers)*/
} mb_param_info_t;

/**
 * @brief Parameter storage area descriptor
 */
typedef struct {
    uint16_t start_offset;                  /*!< Modbus start address for area descriptor */
    mb_param_type_t type;                   /*!< Type of storage area descriptor */
    void* address;                          /*!< Instance address for storage area descriptor */
    size_t size;                            /*!< Instance size for area descriptor (bytes) */
} mb_register_area_descriptor_t;

/**
 * @brief Initialize Modbus Slave controller and stack for TCP port
 *
 * @param[out] handler handler(pointer) to master data structure
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_NO_MEM         Parameter error
 *     - ESP_ERR_NOT_SUPPORTED  Port type not supported
 *     - ESP_ERR_INVALID_STATE  Initialization failure
 */
esp_err_t mbc_slave_init_tcp(void** handler);

/**
 * @brief Initialize Modbus Slave controller and stack for Serial port
 *
 * @param[out] handler handler(pointer) to master data structure
 * @param[in] port_type the type of port
 * @return
 *     - ESP_OK                 Success
 *     - ESP_ERR_NO_MEM         Parameter error
 *     - ESP_ERR_NOT_SUPPORTED  Port type not supported
 *     - ESP_ERR_INVALID_STATE  Initialization failure
 */
esp_err_t mbc_slave_init(mb_port_type_t port_type, void** handler);

/**
 * @brief Initialize Modbus Slave controller interface handle
 *
 * @param[in] handler - pointer to slave interface data structure
 */
void mbc_slave_init_iface(void* handler);

/**
 * @brief Destroy Modbus controller and stack
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_STATE Parameter error
 */
esp_err_t mbc_slave_destroy(void);

/**
 * @brief Start Modbus communication stack
 *
 * @return
 *     - ESP_OK   Success
 *     - ESP_ERR_INVALID_ARG Modbus stack start error
 */
esp_err_t mbc_slave_start(void);

/**
 * @brief Set Modbus communication parameters for the controller
 *
 * @param comm_info Communication parameters structure.
 *
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_INVALID_ARG Incorrect parameter data
 */
esp_err_t mbc_slave_setup(void* comm_info);

/**
 * @brief Wait for specific event on parameter change.
 *
 * @param group Group event bit mask to wait for change
 *
 * @return
 *     - mb_event_group_t event bits triggered
 */
mb_event_group_t mbc_slave_check_event(mb_event_group_t group);

/**
 * @brief Get parameter information
 *
 * @param[out] reg_info parameter info structure
 * @param timeout Timeout in milliseconds to read information from
 *                parameter queue
 * @return
 *     - ESP_OK Success
 *     - ESP_ERR_TIMEOUT Can not get data from parameter queue
 *                       or queue overflow
 */
esp_err_t mbc_slave_get_param_info(mb_param_info_t* reg_info, uint32_t timeout);

/**
 * @brief Set Modbus area descriptor
 *
 * @param descr_data Modbus registers area descriptor structure
 *
 * @return
 *     - ESP_OK: The appropriate descriptor is set
 *     - ESP_ERR_INVALID_ARG: The argument is incorrect
 */
esp_err_t mbc_slave_set_descriptor(mb_register_area_descriptor_t descr_data);

#ifdef __cplusplus
}
#endif

#endif
