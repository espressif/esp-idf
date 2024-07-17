/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_ESP_HCI_INTERNAL_
#define H_ESP_HCI_INTERNAL_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "os/os_mbuf.h"


/* The leadingspace in user info header for ACL data */
#define ESP_HCI_INTERNAL_ACL_MBUF_LEADINGSPCAE  (4)

#define ESP_HCI_INTERNAL_BUF_CMD                (3)

/**
 * @brief Define the HCI hardware error code for synchronization loss.
 *        This error code is used to indicate a loss of synchronization between the controller and the host.
 */
#define ESP_HCI_SYNC_LOSS_ERR                   (0x1)
/** Callback function types; executed when HCI packets are received. */
typedef int esp_hci_internal_rx_cmd_fn(uint8_t *cmd, void *arg);
typedef int esp_hci_internal_rx_acl_fn(struct os_mbuf *om, void *arg);

/**
 * Configures the HCI transport to operate with a host.  The transport will
 * execute specified callbacks upon receiving HCI packets from the controller.
 *
 * @param evt_cb                The callback to execute upon receiving an HCI
 *                                  event.
 * @param evt_arg               Optional argument to pass to the event
 *                                  callback.
 * @param acl_cb                The callback to execute upon receiving ACL
 *                                  data.
 * @param acl_arg               Optional argument to pass to the ACL
 *                                  callback.
 */
void r_ble_hci_trans_cfg_hs(esp_hci_internal_rx_cmd_fn *evt_cb, void *evt_arg,
                            esp_hci_internal_rx_acl_fn *acl_cb, void *acl_arg);
/**
 * Sends ACL data from host to controller.
 *
 * @param om                    The ACL data packet to send.
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int r_ble_hci_trans_hs_acl_tx(struct os_mbuf *om);

/**
 * Sends an HCI command from the host to the controller.
 *
 * @param cmd                   The HCI command to send.  This buffer must be
 *                                  allocated via ble_hci_trans_buf_alloc().
 *
 * @return                      0 on success;
 *                              A BLE_ERR_[...] error code on failure.
 */
int r_ble_hci_trans_hs_cmd_tx(uint8_t *cmd);

/**
 * Allocates a flat buffer of the specified type.
 *
 * @param type                  The type of buffer to allocate; one of the
 *                                  BLE_HCI_TRANS_BUF_[...] constants.
 *
 * @return                      The allocated buffer on success;
 *                              NULL on buffer exhaustion.
 */
uint8_t * r_ble_hci_trans_buf_alloc(int type);

/**
 * Frees the specified flat buffer.  The buffer must have been allocated via
 * ble_hci_trans_buf_alloc().
 *
 * @param buf                   The buffer to free.
 */
void r_ble_hci_trans_buf_free(uint8_t *buf);

/**
 * Configures a callback to get executed whenever an ACL data packet is freed.
 * The function is called immediately before the free occurs.
 *
 * @param cb                    The callback to configure.
 * @param arg                   An optional argument to pass to the callback.
 *
 * @return                      0 on success;
 *                              BLE_ERR_UNSUPPORTED if the transport does not
 *                                  support this operation.
 */
int r_ble_hci_trans_set_acl_free_cb(os_mempool_put_fn *cb, void *arg);

/**
 * @brief Handle an HCI hardware error event.
 *        This function processes a hardware error code and generates the appropriate HCI hardware error event.
 *
 * @param hw_err The hardware error code that needs to be processed. The specific meaning of the error code
 *               depends on the implementation and the hardware.
 *
 * @return int   Returns 0 on success, or a non-zero error code on failure.
 *
 * @note This function should be called whenever a hardware error is detected in the HCI Layer.
 */
int r_ble_ll_hci_ev_hw_err(uint8_t hw_err);

//!TODO: Check what this API is used for
int r_ble_hci_trans_reset(void);

//!TODO: Should we initialize the hci layer in IDF ?
void esp_ble_hci_trans_init(uint8_t);

#ifdef __cplusplus
}
#endif
#endif /* H_ESP_HCI_INTERNAL_ */
