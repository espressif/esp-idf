/* hci.h - Bluetooth Host Control Interface definitions */

/*
 * SPDX-FileCopyrightText: 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/net_buf.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Converts a HCI error to string.
 *
 * The error codes are described in the Bluetooth Core specification,
 * Vol 1, Part F, Section 2.
 *
 * The HCI documentation found in Vol 4, Part E,
 * describes when the different error codes are used.
 *
 * See also the defined BT_HCI_ERR_* macros.
 *
 * @return The string representation of the HCI error code.
 *         If @kconfig{CONFIG_BT_HCI_ERR_TO_STR} is not enabled,
 *         this just returns the empty string
 */
static inline const char *bt_hci_err_to_str(uint8_t hci_err)
{
    ARG_UNUSED(hci_err);

    return "";
}

/** Allocate a HCI command buffer.
  *
  * This function allocates a new buffer for a HCI command. It is given
  * the OpCode (encoded e.g. using the BT_OP macro) and the total length
  * of the parameters. Upon successful return the buffer is ready to have
  * the parameters encoded into it.
  *
  * @deprecated Use bt_hci_cmd_alloc() instead.
  *
  * @param opcode     Command OpCode.
  * @param param_len  Length of command parameters.
  *
  * @return Newly allocated buffer.
  */
struct net_buf *bt_hci_cmd_create(uint16_t opcode, uint8_t param_len);

/** Send a HCI command synchronously.
  *
  * This function is used for sending a HCI command synchronously. It can
  * either be called for a buffer created using bt_hci_cmd_create(), or
  * if the command has no parameters a NULL can be passed instead.
  *
  * The function will block until a Command Status or a Command Complete
  * event is returned. If either of these have a non-zero status the function
  * will return a negative error code and the response reference will not
  * be set. If the command completed successfully and a non-NULL rsp parameter
  * was given, this parameter will be set to point to a buffer containing
  * the response parameters.
  *
  * @param opcode Command OpCode.
  * @param buf    Command buffer or NULL (if no parameters).
  * @param rsp    Place to store a reference to the command response. May
  *               be NULL if the caller is not interested in the response
  *               parameters. If non-NULL is passed the caller is responsible
  *               for calling net_buf_unref() on the buffer when done parsing
  *               it.
  *
  * @return 0 on success or negative error value on failure.
  */
int bt_hci_cmd_send_sync(uint16_t opcode, struct net_buf *buf,
                         struct net_buf **rsp);

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_HCI_H_ */
