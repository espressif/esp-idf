/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _PROXY_COMMON_H_
#define _PROXY_COMMON_H_

#include "mesh/config.h"
#include "mesh/utils.h"
#include "mesh/timer.h"
#include "net.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BLE_MESH_PROXY_ADV_NET_ID                       0x00
#define BLE_MESH_PROXY_ADV_NODE_ID                      0x01
#define BLE_MESH_PROXY_ADV_PRIVATE_NET_ID               0x02
#define BLE_MESH_PROXY_ADV_PRIVATE_NODE_ID              0x03

#define BLE_MESH_PROXY_NET_PDU                          0x00
#define BLE_MESH_PROXY_BEACON                           0x01
#define BLE_MESH_PROXY_CONFIG                           0x02
#define BLE_MESH_PROXY_PROV                             0x03

#define BLE_MESH_PROXY_SAR_COMP                         0x00
#define BLE_MESH_PROXY_SAR_FIRST                        0x01
#define BLE_MESH_PROXY_SAR_CONT                         0x02
#define BLE_MESH_PROXY_SAR_LAST                         0x03

#define BLE_MESH_PROXY_CFG_FILTER_SET                   0x00
#define BLE_MESH_PROXY_CFG_FILTER_ADD                   0x01
#define BLE_MESH_PROXY_CFG_FILTER_REMOVE                0x02
#define BLE_MESH_PROXY_CFG_FILTER_STATUS                0x03
#define BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CAPS_STATUS   0x04
#define BLE_MESH_PROXY_CFG_DIRECTED_PROXY_CONTROL       0x05

#define BLE_MESH_PROXY_PRIVACY_DISABLED                 0x00
#define BLE_MESH_PROXY_PRIVACY_ENABLED                  0x01
#define BLE_MESH_PROXY_PRIVACY_NOT_SUPPORTED            0x02

#define BLE_MESH_PROXY_CLI_TYPE_UNSET                   BIT(0)
#define BLE_MESH_PROXY_CLI_TYPE_DIRECT_PROXY_CLIENT     BIT(1)
#define BLE_MESH_PROXY_CLI_TYPE_BLACK_LIST_CLIENT       BIT(2)
#define BLE_MESH_PROXY_CLI_TYPE_PROXY_CLIENT            BIT(3)

#define BLE_MESH_PROXY_PDU_TYPE(data)                   (data[0] & BIT_MASK(6))
#define BLE_MESH_PROXY_PDU_SAR(data)                    (data[0] >> 6)

#define BLE_MESH_PROXY_PDU_HDR(sar, type)               ((sar) << 6 | ((type) & BIT_MASK(6)))

/* Mesh spec 1.0.1 Section 6.6:
 * "The timeout for the SAR transfer is 20 seconds. When the timeout
 * expires, the Proxy Server shall disconnect."
 */
#define BLE_MESH_PROXY_SAR_TIMEOUT                      K_SECONDS(20)

#define BLE_MESH_PROXY_BUF_SIZE                         68

#ifdef __cplusplus
}
#endif

#endif /* _PROXY_COMMON_H_ */
