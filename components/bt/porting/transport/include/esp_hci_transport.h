/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_ESP_HCI_TRANSPORT_
#define H_ESP_HCI_TRANSPORT_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "os/os_mbuf.h"
#include "esp_hci_driver.h"

/**
 * @brief Enumeration of HCI packet indicators
 */
typedef enum {
    HCI_CMD_IND = 0x01,   /*!< HCI Command Indicator */
    HCI_ACL_IND,          /*!< HCI ACL Data Indicator */
    HCI_SYNC_IND,         /*!< HCI Synchronous Data Indicator */
    HCI_EVT_IND,          /*!< HCI Event Indicator */
    HCI_ISO_IND,          /*!< HCI Isochronous Data Indicator */
    HCI_VENDOR_IND,       /*!< HCI Vendor data Indicator */
} hci_trans_pkt_ind_t;

/**
 * @brief Enumeration of HCI Transport Mode
 */
typedef enum {
    HCI_TRANSPORT_VHCI,         /*!< VHCI Transport Mode */
    HCI_TRANSPORT_UART_NO_DMA,  /*!< UART_NO_DMA Transport Mode */
    HCI_TRANSPORT_UART_UHCI,    /*!< UART_UHCI Transport Mode */
    HCI_TRANSPORT_SDIO,         /*!< SDIO Transport Mode */
    HCI_TRANSPORT_USB,          /*!< USB Transport Mode */
} hci_trans_mode_t;

typedef int hci_transport_host_recv_fn(hci_trans_pkt_ind_t type, uint8_t *data, uint16_t len);
#define HCI_TRANSPORT_CMD_SZ                            (260)
/**
 * @brief Initialize the HCI transport layer.
 *        It should be called before using any other functions in the transport layer.
 *
 * @param hci_transport_mode The mode in which the HCI transport should operate.
 *
 * @return int    Returns 0 on success, or a non-zero error code on failure.
 */
int hci_transport_init(uint8_t hci_transport_mode);

/**
 * @brief Deinitialize the HCI transport layer for releasing any allocated resources.
 */
void hci_transport_deinit(void);

/**
 * @brief Set the host's HCI callback which will be invoked when receiving ACL/Events from controller.
 * @param callback  hci_transport_host_recv_fn type variable
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_transport_host_callback_register(hci_transport_host_recv_fn *callback);

/**
 * @brief Called to send HCI commands form host to controller.
 * @param data      Point to the commands data
 * @param length    Length of data
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_transport_host_cmd_tx(uint8_t *data, uint32_t length);

/**
 * @brief Called to send HCI ACL form host to controller.
 * @param data      Point to the ACL data
 * @param length    Length of data
 * @return int 0 on success, non-zero error code on failure.
 */
int hci_transport_host_acl_tx(uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif
#endif /* H_ESP_HCI_TRANSPORT_ */
