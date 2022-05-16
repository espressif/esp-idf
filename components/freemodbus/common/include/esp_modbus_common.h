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

#ifndef _MB_IFACE_COMMON_H
#define _MB_IFACE_COMMON_H

#include "driver/uart.h"                    // for UART types

#ifdef __cplusplus
extern "C" {
#endif

#if __has_include("esp_check.h")
#include "esp_check.h"

#define MB_RETURN_ON_FALSE(a, err_code, tag, format, ...) ESP_RETURN_ON_FALSE(a, err_code, tag, format __VA_OPT__(,) __VA_ARGS__)

#else

// if cannot include esp_check then use custom check macro

#define MB_RETURN_ON_FALSE(a, err_code, tag, format, ...) do {                                         \
        if (!(a)) {                                                                              \
            ESP_LOGE(tag, "%s(%d): " format, __FUNCTION__, __LINE__ __VA_OPT__(,) __VA_ARGS__);        \
            return err_code;                                                                               \
        }                                                                                                  \
} while(0)

#endif

#define MB_CONTROLLER_STACK_SIZE            (CONFIG_FMB_CONTROLLER_STACK_SIZE)   // Stack size for Modbus controller
#define MB_CONTROLLER_PRIORITY              (CONFIG_FMB_PORT_TASK_PRIO - 1)    // priority of MB controller task

// Default port defines
#define MB_DEVICE_ADDRESS   (1)             // Default slave device address in Modbus
#define MB_DEVICE_SPEED     (115200)        // Default Modbus speed for now hard defined
#define MB_UART_PORT        (UART_NUM_MAX - 1)  // Default UART port number
#define MB_PAR_INFO_TOUT    (10)            // Timeout for get parameter info
#define MB_PARITY_NONE      (UART_PARITY_DISABLE)

// The Macros below handle the endianness while transfer N byte data into buffer
#define _XFER_4_RD(dst, src) { \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 1); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 0); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 3); \
    *(uint8_t *)(dst)++ = *(uint8_t*)(src + 2); \
    (src) += 4; \
}

#define _XFER_2_RD(dst, src) { \
    *(uint8_t *)(dst)++ = *(uint8_t *)(src + 1); \
    *(uint8_t *)(dst)++ = *(uint8_t *)(src + 0); \
    (src) += 2; \
}

#define _XFER_4_WR(dst, src) { \
    *(uint8_t *)(dst + 1) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 0) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 3) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 2) = *(uint8_t *)(src)++ ; \
}

#define _XFER_2_WR(dst, src) { \
    *(uint8_t *)(dst + 1) = *(uint8_t *)(src)++; \
    *(uint8_t *)(dst + 0) = *(uint8_t *)(src)++; \
}

/**
 * @brief Types of actual Modbus implementation
 */
typedef enum
{
    MB_PORT_SERIAL_MASTER = 0x00,   /*!< Modbus port type serial master. */
    MB_PORT_SERIAL_SLAVE,           /*!< Modbus port type serial slave. */
    MB_PORT_TCP_MASTER,             /*!< Modbus port type TCP master. */
    MB_PORT_TCP_SLAVE,              /*!< Modbus port type TCP slave. */
    MB_PORT_COUNT,                  /*!< Modbus port count. */
    MB_PORT_INACTIVE = 0xFF
} mb_port_type_t;

/**
 * @brief Event group for parameters notification
 */
typedef enum
{
    MB_EVENT_NO_EVENTS = 0x00,
    MB_EVENT_HOLDING_REG_WR = BIT0,         /*!< Modbus Event Write Holding registers. */
    MB_EVENT_HOLDING_REG_RD = BIT1,         /*!< Modbus Event Read Holding registers. */
    MB_EVENT_INPUT_REG_RD = BIT3,           /*!< Modbus Event Read Input registers. */
    MB_EVENT_COILS_WR = BIT4,               /*!< Modbus Event Write Coils. */
    MB_EVENT_COILS_RD = BIT5,               /*!< Modbus Event Read Coils. */
    MB_EVENT_DISCRETE_RD = BIT6,            /*!< Modbus Event Read Discrete bits. */
    MB_EVENT_STACK_STARTED = BIT7           /*!< Modbus Event Stack started */
} mb_event_group_t;

/**
 * @brief Type of Modbus parameter
 */
typedef enum {
    MB_PARAM_HOLDING = 0x00,         /*!< Modbus Holding register. */
    MB_PARAM_INPUT,                  /*!< Modbus Input register. */
    MB_PARAM_COIL,                   /*!< Modbus Coils. */
    MB_PARAM_DISCRETE,               /*!< Modbus Discrete bits. */
    MB_PARAM_COUNT,
    MB_PARAM_UNKNOWN = 0xFF
} mb_param_type_t;

/*!
 * \brief Modbus serial transmission modes (RTU/ASCII).
 */
typedef enum {
    MB_MODE_RTU,                     /*!< RTU transmission mode. */
    MB_MODE_ASCII,                   /*!< ASCII transmission mode. */
    MB_MODE_TCP,                     /*!< TCP communication mode. */
    MB_MODE_UDP                      /*!< UDP communication mode. */
} mb_mode_type_t;

/*!
 * \brief Modbus TCP type of address.
 */
typedef enum {
    MB_IPV4 = 0,                     /*!< TCP IPV4 addressing */
    MB_IPV6 = 1                      /*!< TCP IPV6 addressing */
} mb_tcp_addr_type_t;

/**
 * @brief Device communication structure to setup Modbus controller
 */
typedef union {
    // Serial communication structure
    struct {
        mb_mode_type_t mode;                    /*!< Modbus communication mode */
        uint8_t slave_addr;                     /*!< Modbus slave address field (dummy for master) */
        uart_port_t port;                       /*!< Modbus communication port (UART) number */
        uint32_t baudrate;                      /*!< Modbus baudrate */
        uart_parity_t parity;                   /*!< Modbus UART parity settings */
        uint16_t dummy_port;                    /*!< Dummy field, unused */
    };
    // TCP/UDP communication structure
    struct {
        mb_mode_type_t ip_mode;                /*!< Modbus communication mode */
        uint16_t ip_port;                      /*!< Modbus port */
        mb_tcp_addr_type_t ip_addr_type;       /*!< Modbus address type */
        void* ip_addr;                         /*!< Modbus address table for connection */
        void* ip_netif_ptr;                    /*!< Modbus network interface */
    };
} mb_communication_info_t;

/**
 * common interface method types
 */
typedef esp_err_t (*iface_init)(void**);        /*!< Interface method init */
typedef esp_err_t (*iface_destroy)(void);       /*!< Interface method destroy */
typedef esp_err_t (*iface_setup)(void*);        /*!< Interface method setup */
typedef esp_err_t (*iface_start)(void);         /*!< Interface method start */

#ifdef __cplusplus
}
#endif

#endif // _MB_IFACE_COMMON_H
