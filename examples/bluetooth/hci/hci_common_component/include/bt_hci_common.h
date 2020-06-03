/* Baisc macros and functions for Bluetooth Host Controller Interface Layer.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stdio.h"
#include "string.h"

#define HCI_H4_CMD_PREAMBLE_SIZE           (4)

/*  HCI Command Opcode group Field (OGF) */
#define HCI_GRP_HOST_CONT_BASEBAND_CMDS    (0x03 << 10)            /* 0x0C00 */
#define HCI_GRP_BLE_CMDS                   (0x08 << 10)

/*  HCI Command Opcode Command Field (OCF) */
#define HCI_RESET                       (0x0003 | HCI_GRP_HOST_CONT_BASEBAND_CMDS)
#define HCI_BLE_WRITE_ADV_ENABLE        (0x000A | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_DATA          (0x0008 | HCI_GRP_BLE_CMDS)
#define HCI_BLE_WRITE_ADV_PARAMS        (0x0006 | HCI_GRP_BLE_CMDS)

/* HCI Command length. */
#define HCIC_PARAM_SIZE_WRITE_ADV_ENABLE        1
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_PARAMS    15
#define HCIC_PARAM_SIZE_BLE_WRITE_ADV_DATA      31

#define BD_ADDR_LEN     (6)                     /* Device address length */
typedef uint8_t bd_addr_t[BD_ADDR_LEN];         /* Device address */

#define UINT16_TO_STREAM(p, u16) {*(p)++ = (uint8_t)(u16); *(p)++ = (uint8_t)((u16) >> 8);}
#define UINT8_TO_STREAM(p, u8)   {*(p)++ = (uint8_t)(u8);}
#define BDADDR_TO_STREAM(p, a)   {int ijk; for (ijk = 0; ijk < BD_ADDR_LEN;  ijk++) *(p)++ = (uint8_t) a[BD_ADDR_LEN - 1 - ijk];}
#define ARRAY_TO_STREAM(p, a, len) {int ijk; for (ijk = 0; ijk < len;        ijk++) *(p)++ = (uint8_t) a[ijk];}

enum {
    H4_TYPE_COMMAND = 1,
    H4_TYPE_ACL     = 2,
    H4_TYPE_SCO     = 3,
    H4_TYPE_EVENT   = 4
};

/**
 * @brief Writes reset bit in buf and returns size of input buffer after
 *        writing in it.
 *
 * @param buf Input buffer to write which will be sent to controller.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_reset(uint8_t *buf);

/**
 * @brief   This function is used to request the Controller to start or stop advertising.
 *
 * @param buf         Input buffer to write which will be sent to controller.
 * @param adv_enable  1 to enable advertising and 0 to disable.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_enable (uint8_t *buf, uint8_t adv_enable);

/**
 * @brief   This function is used by the Host to set the advertising parameters.
 *
 * @param  buf               Input buffer to write which will be sent to controller.
 * @param  adv_int_min       Minimum advertising interval.
 * @param  adv_int_max       Maximum advertising interval.
 * @param  adv_type          Advertising type.
 * @param  addr_type_own     Own advertising type.
 * @param  addr_type_peer    Peer device's address type.
 * @param  peer_addr         Peer device's BD address.
 * @param  channel_map       Advertising channel map.
 * @param  adv_filter_policy Advertising Filter Policy.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_param (uint8_t *buf, uint16_t adv_int_min, uint16_t adv_int_max,
        uint8_t adv_type, uint8_t addr_type_own,
        uint8_t addr_type_peer, bd_addr_t peer_addr,
        uint8_t channel_map, uint8_t adv_filter_policy);

/**
 * @brief   This function is used to set the data used in advertising packets that have a data field.
 *
 * @param  buf       Input buffer to write which will be sent to controller.
 * @param  data_len  Length of p_data.
 * @param  p_data    Data to be set.
 *
 * @return  Size of buf after writing into it.
 */
uint16_t make_cmd_ble_set_adv_data(uint8_t *buf, uint8_t data_len, uint8_t *p_data);

#ifdef __cplusplus
}
#endif
