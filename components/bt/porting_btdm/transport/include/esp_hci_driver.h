/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef H_ESP_HCI_DRIVER_
#define H_ESP_HCI_DRIVER_
#include <stdint.h>
#include "queue.h"
#include "esp_bt.h"
#include "esp_hci_transport.h"
/**
 * @brief Enumeration of HCI transport direction.
 */
typedef enum {
    HCI_DRIVER_DIR_LEC2H = 0x00,  ///< From controller to host.
    HCI_DRIVER_DIR_BREDRC2H,
    HCI_DRIVER_DIR_BTDMC2H,
    HCI_DRIVER_DIR_H2C,         ///< From host to controller.
} hci_driver_direction_t;

typedef enum {
    HCI_DRIVER_TYPE_CMD = 0x01, ///< HCI Command Indicator.
    HCI_DRIVER_TYPE_ACL,        ///< HCI ACL Data Indicator.
    HCI_DRIVER_TYPE_SYNC,       ///< HCI Synchronous Data Indicator.
    HCI_DRIVER_TYPE_EVT,        ///< HCI Event Indicator.
    HCI_DRIVER_TYPE_ISO,        ///< HCI Isochronous Data Indicator.
    HCI_DRIVER_TYPE_VENDOR,     ///< HCI Vendor data Indicator.
} hci_driver_data_type_t;

typedef enum {
    HCI_DRIVER_CMD = 0x00,
    HCI_DRIVER_LE_EVT,
    HCI_DRIVER_BREDR_EVT,
    HCI_DRIVER_BTDM_EVT,
    HCI_DRIVER_LE_ACL,
    HCI_DRIVER_LE_ISO,
    HCI_DRIVER_BREDR_ACL,
    HCI_DRIVER_BREDR_SYNC,
} hci_driver_data_source_t;

struct hci_driver_packet
{
    STAILQ_ENTRY(hci_driver_packet) next;
    uint8_t *data;
    uint16_t length;
    uint8_t flags;
    uint8_t type;
};
typedef struct hci_driver_packet hci_driver_packet_t;

typedef int hci_driver_forward_fn(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                                  hci_driver_direction_t dir, uint8_t data_source);

#define HCI_DRIVER_D2P(data) ((void *)((uint32_t)data - sizeof(hci_driver_packet_t)))

/**
 *  @brief Structure of HCI driver operations.
 */
typedef struct hci_driver_ops {
    int (*hci_driver_tx)(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                         hci_driver_direction_t dir);
    int (*hci_driver_init)(hci_driver_forward_fn *cb);
    void (*hci_driver_deinit)(void);
} hci_driver_ops_t;


#if CONFIG_BT_NIMBLE_ENABLED
typedef int hci_driver_host_recv_fn(hci_driver_data_type_t type, uint8_t *data, uint16_t len);

/**
 * @brief Set the host's HCI callback which will be invoked when receiving ACL/Events from controller.
 * @param callback  hci_driver_host_recv_fn type variable
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_driver_host_callback_register(hci_driver_host_recv_fn *callback);

/**
 * @brief Called to send HCI commands form host to controller.
 * @param data      Point to the commands data
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_driver_host_cmd_tx(uint8_t *data);

/**
 * @brief Called to send HCI ACL form host to controller.
 * @param data      Point to the ACL data
 * @param length    Length of data
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_driver_host_acl_tx(uint8_t *data, uint32_t length);

#if CONFIG_BT_LE_ISO_SUPPORT
/**
 * @brief Called to send HCI ISO daata form host to controller.
 * @param data      Point to the ISO data
 * @param length    Length of data
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_driver_host_iso_tx(uint8_t *data, uint32_t length);
#endif // CONFIG_BT_LE_ISO_SUPPORT
#endif // CONFIG_BT_NIMBLE_ENABLED

#if UC_BT_CTRL_HCI_INTERFACE_USE_RAM
extern hci_driver_ops_t hci_driver_vhci_ops;
#endif // UC_BT_CTRL_HCI_INTERFACE_USE_RAM

#if UC_BT_CTRL_HCI_INTERFACE_USE_UART
extern hci_driver_ops_t hci_driver_uart_ops;
#if UC_BT_CTRL_UART_HCI_DMA_MODE
extern hci_driver_ops_t hci_driver_uart_dma_ops;
#endif // UC_BT_CTRL_UART_HCI_DMA_MODE
#endif // UC_BT_CTRL_HCI_INTERFACE_USE_UART

#endif // H_ESP_HCI_DRIVER_
