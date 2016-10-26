/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/************************************************************************************
 *
 *  Filename:      btif_util.c
 *
 *  Description:   Miscellaneous helper functions
 *
 *
 ***********************************************************************************/

// #include <hardware/bluetooth.h>
// #include <hardware/bt_hf.h>
// #include <hardware/bt_av.h>
// #include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define LOG_TAG "bt_btif_util"
// #include "btif_common.h"
// #include "bta_api.h"
// #include "gki.h"
// #include "btu.h"
// #include "bte.h"
// #include "btif_dm.h"
#include "btif_util.h"
// #include "bta_ag_api.h"
// #include "bta_av_api.h"
// #include "bta_hh_api.h"
// #include "bta_hf_client_api.h"
// #include "avrc_defs.h"
#include "bt_defs.h"

/************************************************************************************
**  Constants & Macros
************************************************************************************/
#define ISDIGIT(a)  ((a>='0') && (a<='9'))
#define ISXDIGIT(a) (((a>='0') && (a<='9'))||((a>='A') && (a<='F'))||((a>='a') && (a<='f')))

/************************************************************************************
**  Local type definitions
************************************************************************************/

/************************************************************************************
**  Static variables
************************************************************************************/

/************************************************************************************
**  Static functions
************************************************************************************/

/************************************************************************************
**  Externs
************************************************************************************/

/************************************************************************************
**  Functions
************************************************************************************/

/*****************************************************************************
**   Logging helper functions
*****************************************************************************/

UINT32 devclass2uint(DEV_CLASS dev_class)
{
    UINT32 cod = 0;

    if(dev_class != NULL)
    {
        /* if COD is 0, irrespective of the device type set it to Unclassified device */
        cod = (dev_class[2]) | (dev_class[1] << 8) | (dev_class[0] << 16);
    }
    return cod;
}
void uint2devclass(UINT32 cod, DEV_CLASS dev_class)
{
    dev_class[2] = (UINT8)cod;
    dev_class[1] = (UINT8)(cod >> 8);
    dev_class[0] = (UINT8)(cod >> 16);
}

static const UINT8  sdp_base_uuid[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
                                       0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB};

void uuid16_to_uuid128(uint16_t uuid16, bt_uuid_t* uuid128)
{
    uint16_t uuid16_bo;
    memset(uuid128, 0, sizeof(bt_uuid_t));

    memcpy(uuid128->uu, sdp_base_uuid, MAX_UUID_SIZE);
    uuid16_bo = ntohs(uuid16);
    memcpy(uuid128->uu + 2, &uuid16_bo, sizeof(uint16_t));
}

void string_to_uuid(char *str, bt_uuid_t *p_uuid)
{
    uint32_t uuid0, uuid4;
    uint16_t uuid1, uuid2, uuid3, uuid5;

    sscanf(str, "%08x-%04hx-%04hx-%04hx-%08x%04hx",
                &uuid0, &uuid1, &uuid2, &uuid3, &uuid4, &uuid5);

    uuid0 = htonl(uuid0);
    uuid1 = htons(uuid1);
    uuid2 = htons(uuid2);
    uuid3 = htons(uuid3);
    uuid4 = htonl(uuid4);
    uuid5 = htons(uuid5);

    memcpy(&(p_uuid->uu[0]), &uuid0, 4);
    memcpy(&(p_uuid->uu[4]), &uuid1, 2);
    memcpy(&(p_uuid->uu[6]), &uuid2, 2);
    memcpy(&(p_uuid->uu[8]), &uuid3, 2);
    memcpy(&(p_uuid->uu[10]), &uuid4, 4);
    memcpy(&(p_uuid->uu[14]), &uuid5, 2);

    return;

}

void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str)
{
    uint32_t uuid0, uuid4;
    uint16_t uuid1, uuid2, uuid3, uuid5;

    memcpy(&uuid0, &(p_uuid->uu[0]), 4);
    memcpy(&uuid1, &(p_uuid->uu[4]), 2);
    memcpy(&uuid2, &(p_uuid->uu[6]), 2);
    memcpy(&uuid3, &(p_uuid->uu[8]), 2);
    memcpy(&uuid4, &(p_uuid->uu[10]), 4);
    memcpy(&uuid5, &(p_uuid->uu[14]), 2);

    sprintf((char *)str, "%.8x-%.4x-%.4x-%.4x-%.8x%.4x",
            ntohl(uuid0), ntohs(uuid1),
            ntohs(uuid2), ntohs(uuid3),
            ntohl(uuid4), ntohs(uuid5));
    return;
}

