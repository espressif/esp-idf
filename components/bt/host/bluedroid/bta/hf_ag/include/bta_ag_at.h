/******************************************************************************
 *
 *  Copyright (C) 2004-2012 Broadcom Corporation
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

/******************************************************************************
 *
 *  Interface file for BTA AG AT command interpreter.
 *
 ******************************************************************************/
#ifndef BTA_AG_AT_H
#define BTA_AG_AT_H

#include "stack/bt_types.h"
#include "common/bt_target.h"

/*****************************************************************************
**  Constants
*****************************************************************************/
#if (BTA_AG_INCLUDED == TRUE)

/* AT command argument capabilities */
#define BTA_AG_AT_NONE          0x01        /* no argument */
#define BTA_AG_AT_SET           0x02        /* set value */
#define BTA_AG_AT_READ          0x04        /* read value */
#define BTA_AG_AT_TEST          0x08        /* test value range */
#define BTA_AG_AT_FREE          0x10        /* freeform argument */

/* AT command argument format */
#define BTA_AG_AT_STR           0           /* string */
#define BTA_AG_AT_INT           1           /* integer */

/*****************************************************************************
**  Data types
*****************************************************************************/

/* AT command table element */
typedef struct
{
    const char  *p_cmd;         /* AT command string */
    UINT8       arg_type;       /* allowable argument type syntax */
    UINT8       fmt;            /* whether arg is int or string */
    UINT8       min;            /* minimum value for int arg */
    INT16       max;            /* maximum value for int arg */
} tBTA_AG_AT_CMD;

/* callback function executed when command is parsed */
typedef void (tBTA_AG_AT_CMD_CBACK)(void *p_user, UINT16 cmd, UINT8 arg_type,
                                    char *p_arg, INT16 int_arg);

/* callback function executed to send "ERROR" result code */
typedef void (tBTA_AG_AT_ERR_CBACK)(void *p_user, BOOLEAN unknown, char *p_arg);

/* AT command parsing control block */
typedef struct
{
    tBTA_AG_AT_CMD          *p_at_tbl;      /* AT command table */
    tBTA_AG_AT_CMD_CBACK    *p_cmd_cback;   /* command callback */
    tBTA_AG_AT_ERR_CBACK    *p_err_cback;   /* error callback */
    void                    *p_user;        /* user-defined data */
    char                    *p_cmd_buf;     /* temp parsing buffer */
    UINT16                  cmd_pos;        /* position in temp buffer */
    UINT16                  cmd_max_len;    /* length of temp buffer to allocate */
    UINT8                   state;          /* parsing state */
} tBTA_AG_AT_CB;

/*****************************************************************************
**  Function prototypes
*****************************************************************************/

/*****************************************************************************
**
** Function         bta_ag_at_init
**
** Description      Initialize the AT command parser control block.
**
**
** Returns          void
**
*****************************************************************************/
extern void bta_ag_at_init(tBTA_AG_AT_CB *p_cb);

/*****************************************************************************
**
** Function         bta_ag_at_reinit
**
** Description      Re-initialize the AT command parser control block.  This
**                  function resets the AT command parser state and frees
**                  any GKI buffer.
**
**
** Returns          void
**
*****************************************************************************/
extern void bta_ag_at_reinit(tBTA_AG_AT_CB *p_cb);

/*****************************************************************************
**
** Function         bta_ag_at_parse
**
** Description      Parse AT commands.  This function will take the input
**                  character string and parse it for AT commands according to
**                  the AT command table passed in the control block.
**
**
** Returns          void
**
*****************************************************************************/
extern void bta_ag_at_parse(tBTA_AG_AT_CB *p_cb, char *p_buf, UINT16 len);

#endif /* #if (BTA_AG_INCLUDED == TRUE) */

#endif /* BTA_AG_AT_H */
