/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ESP_SEC_API_H__
#define __ESP_SEC_API_H__

#include "stack/bt_types.h"

#define APP_SEC_IRK_FLAG        (0)
#define RAND_NB_LEN         0x08
#define SEC_KEY_LEN         0x10

/*
 * STRUCTURES DEFINITIONS
 ****************************************************************************************
 */


/// Generic Security key structure
typedef struct {
    /// Key value MSB -> LSB
    UINT8 key[SEC_KEY_LEN];
} smp_sec_key;

///Random number structure
typedef struct {
    ///8-byte array for random number
    UINT8     nb[RAND_NB_LEN];
} rand_nb;

typedef struct {
    // LTK
    smp_sec_key ltk;
    // Random Number
    rand_nb rand_nb;
    // EDIV
    UINT16 ediv;
    // LTK key size
    UINT8 key_size;

    // Last paired peer address type
    UINT8 peer_addr_type;
    // Last paired peer address
    BD_ADDR peer_addr;

    // authentication level
    UINT8 auth;

} tAPP_SEC_ENV;

extern tAPP_SEC_ENV app_sec_env;

/*
* GLOBAL FUNCTIONS DECLARATIONS
****************************************************************************************
*/

void app_ble_sec_init(void);

void app_ble_sec_pairing_cmp_evt_send(UINT8);

UINT32 app_ble_sec_gen_tk(void);

void app_ble_sec_gen_ltk(UINT8 key_size);

void app_ble_security_start(void);

#endif /* __ESP_SEC_API_H__ */
