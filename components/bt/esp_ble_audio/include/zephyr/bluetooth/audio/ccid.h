/**
 * @file
 * @brief Header for Bluetooth Audio Content Control Identifier.
 *
 * SPDX-FileCopyrightText: 2020 Bose Corporation
 * SPDX-FileCopyrightText: 2021-2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CCID_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CCID_H_

/**
 * @brief Bluetooth Content Control Identifier (CCID)
 * @defgroup bt_ccid Bluetooth Content Control Identifier
 *
 * @since 3.7
 * @version 0.8.0
 *
 * @ingroup bluetooth
 * @{
 *
 * The Content Control Identifier (CCID) API manages CCIDs for @ref BT_UUID_CCID characteristics.
 */

#include <stdint.h>

#include <zephyr/bluetooth/gatt.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Minimum CCID value */
#define BT_CCID_MIN 0
/** Maximum CCID value */
#define BT_CCID_MAX 255

/**
 * @brief Allocates a CCID value.
 *
 * This should always be called right before registering a GATT service that contains a
 * @ref BT_UUID_CCID characteristic. Allocating a CCID without registering the characteristic
 * may (in very rare cases) result in duplicated CCIDs on the device.
 *
 * Requires that @kconfig{CONFIG_BT_CONN} is enabled.
 *
 * @retval ccid 8-bit unsigned CCID value on success
 * @retval -ENOMEM No more CCIDs can be allocated
 */
int bt_ccid_alloc_value(void);

/**
 * @brief Get the GATT attribute of a CCID value
 *
 * Searches the current GATT database for a CCID characteristic that has the supplied CCID value.
 *
 * Requires that @kconfig{CONFIG_BT_CONN} is enabled.
 *
 * @param ccid The CCID to search for
 *
 * @retval NULL None was found
 * @retval attr Pointer to a GATT attribute
 */
const struct bt_gatt_attr *bt_ccid_find_attr(uint8_t ccid);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_AUDIO_CCID_H_ */
