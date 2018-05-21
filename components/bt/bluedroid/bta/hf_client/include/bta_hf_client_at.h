/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2003-2012 Broadcom Corporation
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

/*****************************************************************************
**  Data types
*****************************************************************************/
#include "common/bt_target.h"
#if (BTA_HF_INCLUDED == TRUE)

/* ASCII character string of arguments to the AT command */
#define BTA_HF_CLIENT_AT_MAX_LEN        512

/* AT command table element */
typedef struct {
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

enum {
    BTA_HF_CLIENT_AT_NONE,
    BTA_HF_CLIENT_AT_BRSF,
    BTA_HF_CLIENT_AT_BAC,
    BTA_HF_CLIENT_AT_CIND,
    BTA_HF_CLIENT_AT_CIND_STATUS,
    BTA_HF_CLIENT_AT_CMER,
    BTA_HF_CLIENT_AT_CHLD,
    BTA_HF_CLIENT_AT_CMEE,
    BTA_HF_CLIENT_AT_BIA,
    BTA_HF_CLIENT_AT_CLIP,
    BTA_HF_CLIENT_AT_CCWA,
    BTA_HF_CLIENT_AT_COPS,
    BTA_HF_CLIENT_AT_CLCC,
    BTA_HF_CLIENT_AT_BVRA,
    BTA_HF_CLIENT_AT_VGS,
    BTA_HF_CLIENT_AT_VGM,
    BTA_HF_CLIENT_AT_ATD,
    BTA_HF_CLIENT_AT_BLDN,
    BTA_HF_CLIENT_AT_ATA,
    BTA_HF_CLIENT_AT_CHUP,
    BTA_HF_CLIENT_AT_BTRH,
    BTA_HF_CLIENT_AT_VTS,
    BTA_HF_CLIENT_AT_BCC,
    BTA_HF_CLIENT_AT_BCS,
    BTA_HF_CLIENT_AT_CNUM,
    BTA_HF_CLIENT_AT_NREC,
    BTA_HF_CLIENT_AT_BINP,
};

typedef UINT8 tBTA_HF_CLIENT_AT_CMD;

/* Maximum combined buffer for received AT events string */
#define BTA_HF_CLIENT_AT_PARSER_MAX_LEN        4096

/* This structure holds prepared AT command queued for sending */
struct queued_at_cmd {
    tBTA_HF_CLIENT_AT_CMD cmd;
    char buf[BTA_HF_CLIENT_AT_MAX_LEN];
    UINT16 buf_len;
    struct queued_at_cmd *next;
};
typedef struct queued_at_cmd tBTA_HF_CLIENT_AT_QCMD;

/* Maximum number of indicators */
#define BTA_HF_CLIENT_AT_INDICATOR_COUNT 20

/* AT command parsing control block */
typedef struct {
    char                    buf[BTA_HF_CLIENT_AT_PARSER_MAX_LEN + 1]; /* extra byte to always have \0 at the end */
    unsigned int            offset;
    tBTA_HF_CLIENT_AT_CMD   current_cmd;
    tBTA_HF_CLIENT_AT_QCMD  *queued_cmd;

    TIMER_LIST_ENT          resp_timer;    /* AT response timer */
    BOOLEAN                 resp_timer_on; /* TRUE if AT response timer is active */

    TIMER_LIST_ENT          hold_timer;    /* AT hold timer */
    BOOLEAN                 hold_timer_on; /* TRUE if AT hold timer is active */

    /* CIND: lookup table to store the sequence of incoming indicators and their values
       so when their values come later, we know which value in sequence match certain indicator */
    int                     indicator_lookup[BTA_HF_CLIENT_AT_INDICATOR_COUNT];

} tBTA_HF_CLIENT_AT_CB;

/*****************************************************************************
**  Functions
*****************************************************************************/

void bta_hf_client_at_init(void);
void bta_hf_client_at_reset(void);
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
