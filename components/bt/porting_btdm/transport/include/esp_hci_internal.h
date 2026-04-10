/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef H_ESP_HCI_INTERNAL_
#define H_ESP_HCI_INTERNAL_

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
#include "esp_hci_driver.h"
#include "ble_mbuf.h"


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
typedef int esp_hci_internal_rx_acl_fn(struct ble_mbuf *om, void *arg);
#if CONFIG_BT_LE_ISO_SUPPORT
typedef int esp_hci_internal_rx_iso_fn(const uint8_t *data, uint16_t len, void *arg);
/**
 * @brief Configure the ISO data receive callback for BLE HCI transport.
 *
 * @param iso_cb   The callback to execute upon receiving ISO data.
 *
 * @param iso_arg  Optional argument to pass to the ISO callback.
 */
void r_ble_iso_trans_cfg_hs(esp_hci_internal_rx_iso_fn *iso_cb, void *iso_arg);

#define ble_iso_trans_cfg_hs   r_ble_iso_trans_cfg_hs

extern int r_ble_hci_trans_hs_iso_tx(const uint8_t *data, uint16_t length, void *arg);
#define ble_hci_trans_hs_iso_tx  r_ble_hci_trans_hs_iso_tx
#endif // CONFIG_BT_LE_ISO_SUPPORT
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
int r_ble_hci_trans_hs_acl_tx(struct ble_mbuf *om);

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

// ********************************************************************************************
//                  btdm common
// ********************************************************************************************

typedef int (*btdm_hci_trans_tx_func_t)(hci_driver_packet_t *pkt);

#define HCI_INTERNAL_CONN_MASK                          (0x0fff)
#define HCI_INTERNAL_CONN_IS_BLE(conn_handle)           (!(conn_handle & 0x0800))
#define HCI_INTERNAL_CONN_IS_BREDR(conn_handle)         (conn_handle & 0x0800)
#define HCI_INTERNAL_CONN_IS_BREDR_ACL(conn_handle)     ((conn_handle & 0x0800) && (conn_handle & 0x000f))
#define HCI_INTERNAL_CONN_IS_BREDR_SYNC(conn_handle)    ((conn_handle & 0x0800) && (conn_handle & 0x00f0))

int r_btdm_hci_trans_register_tx(btdm_hci_trans_tx_func_t *tx_func, bool async);

int r_btdm_hci_trans_rx(hci_driver_packet_t *pkt);

hci_driver_packet_t *r_btdm_hci_trans_buf_alloc(uint8_t type, uint16_t conn_handle);
void r_btdm_hci_trans_buf_free(hci_driver_packet_t *pkt);
uint16_t r_btdm_get_le16(const void *buf);

#define btdm_hci_trans_buf_alloc    r_btdm_hci_trans_buf_alloc
#define btdm_hci_trans_buf_free     r_btdm_hci_trans_buf_free
#define btdm_get_le16               r_btdm_get_le16
#define btdm_hci_trans_register_tx  r_btdm_hci_trans_register_tx
#define btdm_hci_trans_rx           r_btdm_hci_trans_rx

void btdm_hci_pkt_flag_ble_setf(hci_driver_packet_t *pkt, uint8_t is_ble);
uint8_t btdm_hci_pkt_flag_ble_getf(hci_driver_packet_t *pkt);
uint8_t btdm_hci_pkt_flag_bredr_getf(hci_driver_packet_t *pkt);
uint8_t btdm_hci_pkt_flag_cmdpool_getf(hci_driver_packet_t *pkt);

// ********************************************************************************************
//                      BREDR
// ********************************************************************************************

int bredr_hci_trans_acl_rx(hci_driver_packet_t *pkt);
int bredr_hci_trans_sync_rx(hci_driver_packet_t *pkt);
void bredr_hci_trans_register_tx(btdm_hci_trans_tx_func_t *acl_tx_func,
                                btdm_hci_trans_tx_func_t *sync_tx_func,
                                btdm_hci_trans_tx_func_t *evt_tx_func);
void bredr_hci_trans_acl_tx_done(hci_driver_packet_t *pkt);
void bredr_hci_trans_sync_tx_done(hci_driver_packet_t *pkt);
void bredr_hci_trans_evt_tx_done(hci_driver_packet_t *pkt);
int bredr_hci_trans_acl_free(hci_driver_packet_t *pkt);
int bredr_hci_trans_sync_free(hci_driver_packet_t *pkt);
int bredr_hci_trans_evt_free(hci_driver_packet_t *pkt);

#ifdef __cplusplus
}
#endif
#endif /* H_ESP_HCI_INTERNAL_ */
