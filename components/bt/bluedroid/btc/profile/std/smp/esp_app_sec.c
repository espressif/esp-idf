// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
void app_ble_sec_init()
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





