/** @file
 *  @brief Bluetooth Mesh Proxy APIs.
 */

/*
 * Copyright (c) 2017 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _BLE_MESH_PROXY_H_
#define _BLE_MESH_PROXY_H_

#include <stddef.h>
/**
 * @brief Bluetooth Mesh Proxy
 * @defgroup bt_mesh_proxy Bluetooth Mesh Proxy
 * @ingroup bt_mesh
 * @{
 */

/**
 * @brief Enable advertising with Node Identity.
 *
 * This API requires that GATT Proxy support has been enabled. Once called
 * each subnet will start advertising using Node Identity for the next
 * 60 seconds.
 *
 * @return 0 on success, or (negative) error code on failure.
 */
int bt_mesh_proxy_identity_enable(void);

/**
 * @}
 */

#endif /* _BLE_MESH_PROXY_H_ */
