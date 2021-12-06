/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>        // standard library
#include <string.h>

#include "esp_sec_api.h"


extern void srand (unsigned int seed);
extern int random (void);

/// Application Security Environment Structure
tAPP_SEC_ENV app_sec_env;


/*******************************************************************************
**
** Function         app_ble_sec_gen_tk
**
** Description      This function is called to generate the ble tk
**
** Returns          the generate tk value
**
*******************************************************************************/
UINT32 app_ble_sec_gen_tk(void)
{
    // Generate a PIN Code (Between 100000 and 999999)
    return (100000 + (random() % 900000));
}

/*******************************************************************************
**
** Function         app_ble_sec_gen_ltk
**
** Description      This function is called to generate the ble ltk
**
** Returns          NULL
**
*******************************************************************************/
void app_ble_sec_gen_ltk(UINT8 key_size)
{
    // Counter
    UINT8 i;
    app_sec_env.key_size = key_size;

    // Randomly generate the LTK and the Random Number
    for (i = 0; i < RAND_NB_LEN; i++) {
        app_sec_env.rand_nb.nb[i] = random() % 256;
    }

    // Randomly generate the end of the LTK
    for (i = 0; i < SEC_KEY_LEN; i++) {
        app_sec_env.ltk.key[i] = (((key_size) < (16 - i)) ? 0 : random() % 256);
    }

    // Randomly generate the EDIV
    app_sec_env.ediv = random() % 65536;
}


/*******************************************************************************
**
** Function         app_ble_sec_init
**
** Description      This function is init the security env and function
**
** Returns          NULL
**
*******************************************************************************/
void app_ble_sec_init(void)
{
    // Reset Security Environment
    memset(&app_sec_env, 0, sizeof(app_sec_env));
}


/*******************************************************************************
**
** Function         app_ble_security_start
**
** Description      This function is called by the slave when the seurity start
**
** Returns          NULL
**
*******************************************************************************/
void app_ble_security_start(void)
{

}
