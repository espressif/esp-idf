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
// #include <errno.h>
#include <string.h>
#include <stdio.h>

#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_int.h"
#include "stack/port_api.h"
#include "osi/allocator.h"

#if (BTA_HF_INCLUDED == TRUE)
/* Uncomment to enable AT traffic dumping */
/* #define BTA_HF_CLIENT_AT_DUMP 1 */

/* minimum length of AT event */
#define BTA_HF_CLIENT_AT_EVENT_MIN_LEN 3

/* timeout for AT response */
#define BTA_HF_CLIENT_AT_TIMEOUT 29989

/* timeout for AT hold timer */
#define BTA_HF_CLIENT_AT_HOLD_TIMEOUT 41

/******************************************************************************
**
**          DATA TYPES AND CONTAINERS
**
*******************************************************************************/
/* BRSF: store received values here */
extern tBTA_HF_CLIENT_CB  bta_hf_client_cb;

/******************************************************************************
**       SUPPORTED EVENT MESSAGES
*******************************************************************************/

/* CIND: supported indicator names */
#define BTA_HF_CLIENT_INDICATOR_BATTERYCHG  "battchg"
#define BTA_HF_CLIENT_INDICATOR_SIGNAL      "signal"
#define BTA_HF_CLIENT_INDICATOR_SERVICE     "service"
#define BTA_HF_CLIENT_INDICATOR_CALL        "call"
#define BTA_HF_CLIENT_INDICATOR_ROAM        "roam"
#define BTA_HF_CLIENT_INDICATOR_CALLSETUP   "callsetup"
#define BTA_HF_CLIENT_INDICATOR_CALLHELD    "callheld"

#define MIN(a, b) \
    ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); (_a < _b) ? _a : _b; })

/* CIND: represents each indicators boundaries */
typedef struct {
    char *name;
    UINT8 min;
    UINT8 max;
    UINT8 namelen;
} tBTA_HF_CLIENT_INDICATOR;

#define BTA_HF_CLIENT_AT_SUPPORTED_INDICATOR_COUNT 7

/* CIND: storage room for indicators value range and their statuses */
static const tBTA_HF_CLIENT_INDICATOR bta_hf_client_indicators[BTA_HF_CLIENT_AT_SUPPORTED_INDICATOR_COUNT] = {
    /* name                                | min | max | name length - used by parser */
    {BTA_HF_CLIENT_INDICATOR_CALL,           0,   1,    sizeof(BTA_HF_CLIENT_INDICATOR_CALL)},
    {BTA_HF_CLIENT_INDICATOR_CALLSETUP,      0,   3,    sizeof(BTA_HF_CLIENT_INDICATOR_CALLSETUP)},
    {BTA_HF_CLIENT_INDICATOR_SERVICE,        0,   1,    sizeof(BTA_HF_CLIENT_INDICATOR_SERVICE)},
    {BTA_HF_CLIENT_INDICATOR_SIGNAL,         0,   5,    sizeof(BTA_HF_CLIENT_INDICATOR_SIGNAL)},
    {BTA_HF_CLIENT_INDICATOR_ROAM,           0,   1,    sizeof(BTA_HF_CLIENT_INDICATOR_ROAM)},
    {BTA_HF_CLIENT_INDICATOR_BATTERYCHG,     0,   5,    sizeof(BTA_HF_CLIENT_INDICATOR_BATTERYCHG)},
    {BTA_HF_CLIENT_INDICATOR_CALLHELD,       0,   2,    sizeof(BTA_HF_CLIENT_INDICATOR_CALLHELD)}
};

/* +VGM/+VGS - gain min/max values  */
#define BTA_HF_CLIENT_VGS_MIN   0
#define BTA_HF_CLIENT_VGS_MAX  15
#define BTA_HF_CLIENT_VGM_MIN   0
#define BTA_HF_CLIENT_VGM_MAX  15

UINT32 service_index = 0;
BOOLEAN service_availability = TRUE;
/* helper functions for handling AT commands queueing */

static void bta_hf_client_handle_ok(void);

static void bta_hf_client_clear_queued_at(void)
{
    tBTA_HF_CLIENT_AT_QCMD *cur = bta_hf_client_cb.scb.at_cb.queued_cmd;
    tBTA_HF_CLIENT_AT_QCMD *next;

    while (cur != NULL) {
        next = cur->next;
        osi_free(cur);
        cur = next;
    }

    bta_hf_client_cb.scb.at_cb.queued_cmd = NULL;
}

static void bta_hf_client_queue_at(tBTA_HF_CLIENT_AT_CMD cmd, const char *buf, UINT16 buf_len)
{
    tBTA_HF_CLIENT_AT_QCMD *new_cmd;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if ((new_cmd = (tBTA_HF_CLIENT_AT_QCMD *) osi_malloc(sizeof(tBTA_HF_CLIENT_AT_QCMD))) != NULL) {
        new_cmd->cmd = cmd;
        new_cmd->buf_len = buf_len;
        new_cmd->next = NULL;
        memcpy(new_cmd->buf, buf, buf_len);

        if (bta_hf_client_cb.scb.at_cb.queued_cmd != NULL) {
            tBTA_HF_CLIENT_AT_QCMD *qcmd = bta_hf_client_cb.scb.at_cb.queued_cmd;

            while (qcmd->next != NULL) {
                qcmd = qcmd->next;
            }

            qcmd->next = new_cmd;
        } else {
            bta_hf_client_cb.scb.at_cb.queued_cmd = new_cmd;
        }
    }
}

static void bta_hf_client_at_resp_timer_cback (TIMER_LIST_ENT *p_tle)
{
    if (p_tle) {
        bta_hf_client_cb.scb.at_cb.resp_timer_on = FALSE;

        APPL_TRACE_ERROR("HFPClient: AT response timeout, disconnecting");

        bta_hf_client_sm_execute(BTA_HF_CLIENT_API_CLOSE_EVT, NULL);
    }
}

static void bta_hf_client_stop_at_resp_timer(void)
{
    if (bta_hf_client_cb.scb.at_cb.resp_timer_on) {
        bta_hf_client_cb.scb.at_cb.resp_timer_on = FALSE;
        bta_sys_stop_timer (&bta_hf_client_cb.scb.at_cb.resp_timer);
    }
}

static void bta_hf_client_free_at_resp_timer(void)
{
    bta_hf_client_cb.scb.at_cb.resp_timer_on = FALSE;
    bta_sys_free_timer (&bta_hf_client_cb.scb.at_cb.resp_timer);
}

static void bta_hf_client_start_at_resp_timer(void)
{
    if (bta_hf_client_cb.scb.at_cb.resp_timer_on) {
        bta_sys_stop_timer (&bta_hf_client_cb.scb.at_cb.resp_timer);
    }

    bta_hf_client_cb.scb.at_cb.resp_timer.p_cback = (TIMER_CBACK *)&bta_hf_client_at_resp_timer_cback;
    bta_sys_start_timer(&bta_hf_client_cb.scb.at_cb.resp_timer, 0, BTA_HF_CLIENT_AT_TIMEOUT);
    bta_hf_client_cb.scb.at_cb.resp_timer_on = TRUE;
}

static void bta_hf_client_send_at(tBTA_HF_CLIENT_AT_CMD cmd, char *buf, UINT16 buf_len)
{
    if ((bta_hf_client_cb.scb.at_cb.current_cmd == BTA_HF_CLIENT_AT_NONE ||
            bta_hf_client_cb.scb.svc_conn == FALSE) &&
            bta_hf_client_cb.scb.at_cb.hold_timer_on == FALSE) {
        UINT16  len;

#ifdef BTA_HF_CLIENT_AT_DUMP
        APPL_TRACE_DEBUG("%s %.*s", __FUNCTION__, buf_len - 1, buf);
#endif

        bta_hf_client_cb.scb.at_cb.current_cmd = cmd;
        /* Generate fake responses for these because they won't reliably work */
        if (!service_availability &&
                (cmd == BTA_HF_CLIENT_AT_CNUM || cmd == BTA_HF_CLIENT_AT_COPS)) {
            APPL_TRACE_WARNING("%s: No service, skipping %d command", __FUNCTION__, cmd);
            bta_hf_client_handle_ok();
            return;
        }

        PORT_WriteData(bta_hf_client_cb.scb.conn_handle, buf, buf_len, &len);

        bta_hf_client_start_at_resp_timer();

        return;
    }

    bta_hf_client_queue_at(cmd, buf, buf_len);
}

static void bta_hf_client_send_queued_at(void)
{
    tBTA_HF_CLIENT_AT_QCMD *cur = bta_hf_client_cb.scb.at_cb.queued_cmd;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (cur != NULL) {
        bta_hf_client_cb.scb.at_cb.queued_cmd = cur->next;

        bta_hf_client_send_at(cur->cmd, cur->buf, cur->buf_len);

        osi_free(cur);
    }
}

static void bta_hf_client_at_hold_timer_cback(TIMER_LIST_ENT *p_tle)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (p_tle) {
        bta_hf_client_cb.scb.at_cb.hold_timer_on = FALSE;
        bta_hf_client_send_queued_at();
    }
}

static void bta_hf_client_stop_at_hold_timer(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (bta_hf_client_cb.scb.at_cb.hold_timer_on) {
        bta_hf_client_cb.scb.at_cb.hold_timer_on = FALSE;
        bta_sys_stop_timer (&bta_hf_client_cb.scb.at_cb.hold_timer);
    }
}

static void bta_hf_client_free_at_hold_timer(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    bta_hf_client_cb.scb.at_cb.hold_timer_on = FALSE;
    bta_sys_free_timer(&bta_hf_client_cb.scb.at_cb.hold_timer);
}

static void bta_hf_client_start_at_hold_timer(void)
{
    TIMER_LIST_ENT *timer = &bta_hf_client_cb.scb.at_cb.hold_timer;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (bta_hf_client_cb.scb.at_cb.hold_timer_on) {
        bta_sys_stop_timer (timer);
    }

    timer->p_cback = (TIMER_CBACK *)&bta_hf_client_at_hold_timer_cback;
    bta_sys_start_timer(timer, 0, BTA_HF_CLIENT_AT_HOLD_TIMEOUT);
    bta_hf_client_cb.scb.at_cb.hold_timer_on = TRUE;
}

/******************************************************************************
**
**          COMMON AT EVENT HANDLING FUNCTIONS
**
**   Receives data (strings, ints, etc.) from the parser and processes this data.
**   No buffer parsing is being done here.
*******************************************************************************/

static void bta_hf_client_handle_ok(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    bta_hf_client_stop_at_resp_timer();

    if (!bta_hf_client_cb.scb.svc_conn) {
        bta_hf_client_slc_seq(FALSE);
        return;
    }

    switch (bta_hf_client_cb.scb.at_cb.current_cmd) {
    case BTA_HF_CLIENT_AT_BIA:
    case BTA_HF_CLIENT_AT_BCC:
        break;
    case BTA_HF_CLIENT_AT_BCS:
        bta_hf_client_start_at_hold_timer();
        bta_hf_client_cb.scb.at_cb.current_cmd = BTA_HF_CLIENT_AT_NONE;
        return;
    case BTA_HF_CLIENT_AT_CLIP: //last cmd is post slc seq
        if (bta_hf_client_cb.scb.send_at_reply == FALSE) {
            bta_hf_client_cb.scb.send_at_reply = TRUE;
        }
        break;
    case BTA_HF_CLIENT_AT_NONE:
        bta_hf_client_stop_at_hold_timer();
        break;
    default:
        if (bta_hf_client_cb.scb.send_at_reply) {
            bta_hf_client_at_result(BTA_HF_CLIENT_AT_RESULT_OK, 0);
        }
        break;
    }

    bta_hf_client_cb.scb.at_cb.current_cmd = BTA_HF_CLIENT_AT_NONE;

    bta_hf_client_send_queued_at();
}

static void bta_hf_client_handle_error(tBTA_HF_CLIENT_AT_RESULT_TYPE type, UINT16 cme)
{
    APPL_TRACE_DEBUG("%s %u %u", __FUNCTION__, type, cme);

    bta_hf_client_stop_at_resp_timer();

    if (!bta_hf_client_cb.scb.svc_conn) {
        bta_hf_client_slc_seq(TRUE);
        return;
    }

    switch (bta_hf_client_cb.scb.at_cb.current_cmd) {
    case BTA_HF_CLIENT_AT_BIA:
        break;
    case BTA_HF_CLIENT_AT_BCC:
    case BTA_HF_CLIENT_AT_BCS:
        bta_hf_client_cback_sco(BTA_HF_CLIENT_AUDIO_CLOSE_EVT);
        break;
    case BTA_HF_CLIENT_AT_CLIP: //last cmd is post slc seq
        if (bta_hf_client_cb.scb.send_at_reply == FALSE) {
            bta_hf_client_cb.scb.send_at_reply = TRUE;
        }
        break;
    default:
        if (bta_hf_client_cb.scb.send_at_reply) {
            bta_hf_client_at_result(type, cme);
        }
        break;
    }

    bta_hf_client_cb.scb.at_cb.current_cmd = BTA_HF_CLIENT_AT_NONE;

    bta_hf_client_send_queued_at();
}

static void bta_hf_client_handle_ring(void)
{
    APPL_TRACE_DEBUG("%s", __FUNCTION__);
    bta_hf_client_evt_val(BTA_HF_CLIENT_RING_INDICATION, 0);
}

static void bta_hf_client_handle_brsf(UINT32 value)
{
    APPL_TRACE_DEBUG("%s 0x%x", __FUNCTION__, value);
    bta_hf_client_cb.scb.peer_features = value;
}

/* handles a single indicator descriptor - registers it for value changing events */
static void bta_hf_client_handle_cind_list_item(char *name, UINT32 min, UINT32 max, UINT32 index)
{

    UINT8 i = 0;

    APPL_TRACE_DEBUG("%s %u.%s <%u:%u>", __FUNCTION__, index, name, min, max);

    /* look for a matching indicator on list of supported ones */
    for (i = 0; i < BTA_HF_CLIENT_AT_SUPPORTED_INDICATOR_COUNT; i++) {
        if (strcmp(name, BTA_HF_CLIENT_INDICATOR_SERVICE) == 0) {
            service_index = index;
        }
        /* look for a match - search one sign further than indicators name to check for string end */
        /* It will distinguish 'callheld' which could be matched by strncmp as 'call'.               */
        if (strncmp(name, bta_hf_client_indicators[i].name, bta_hf_client_indicators[i].namelen) != 0) {
            continue;
        }

        /* index - enumerates value position in the incoming sequence                      */
        /* if name matches one of the known indicators, add its incoming position          */
        /* to lookup table for easy value->indicator matching later, when only values come  */
        bta_hf_client_cb.scb.at_cb.indicator_lookup[index] = i;

        return;
    }
}

static void bta_hf_client_handle_cind_value(UINT32 index, UINT32 value)
{
    APPL_TRACE_DEBUG("%s index: %u value: %u", __FUNCTION__, index, value);

    if (index >= BTA_HF_CLIENT_AT_INDICATOR_COUNT) {
        return;
    }

    if (service_index == index) {
        if (value == 0) {
            service_availability = FALSE;
        } else {
            service_availability = TRUE;
        }
    }
    if (bta_hf_client_cb.scb.at_cb.indicator_lookup[index] == -1) {
        return;
    }

    /* get the real array index from lookup table */
    index = bta_hf_client_cb.scb.at_cb.indicator_lookup[index];

    /* Ignore out of range values */
    if (value > bta_hf_client_indicators[index].max ||
            value < bta_hf_client_indicators[index].min) {
        return;
    }

    /* tBTA_HF_CLIENT_IND_TYPE match index in bta_hf_client_indicators */
    bta_hf_client_ind(index, value);
}

static void bta_hf_client_handle_chld(UINT32 mask)
{
    APPL_TRACE_DEBUG("%s 0x%x", __FUNCTION__, mask);

    bta_hf_client_cb.scb.chld_features |= mask;
}

static void bta_hf_client_handle_ciev(UINT32 index, UINT32 value)
{
    INT8 realind = -1;

    APPL_TRACE_DEBUG("%s index: %u value: %u", __FUNCTION__, index, value);

    if (index == 0 || index > BTA_HF_CLIENT_AT_INDICATOR_COUNT) {
        APPL_TRACE_WARNING("%s: Invalid index %d", __FUNCTION__, index);
        return;
    }

    if (service_index == index - 1) {
        service_availability = value == 0 ? FALSE : TRUE;
    }

    realind = bta_hf_client_cb.scb.at_cb.indicator_lookup[index - 1];

    if (realind >= 0 && realind < BTA_HF_CLIENT_AT_SUPPORTED_INDICATOR_COUNT) {
        /* get the real in-array index from lookup table by index it comes at */
        /* if there is no bug it should automatically be correctly calculated    */
        if (value > bta_hf_client_indicators[realind].max || value < bta_hf_client_indicators[realind].min) {
            return;
        }

        /* update service availability on +ciev from AG. */
        if (service_index == (index - 1)) {
            if (value == 1) {
                service_availability = TRUE;
            } else {
                service_availability = FALSE;
            }
        }

        /* tBTA_HF_CLIENT_IND_TYPE match index in bta_hf_client_indicators */
        bta_hf_client_ind(realind, value);
    }
}

static void bta_hf_client_handle_bcs(UINT32 codec)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, codec);

    if (codec == BTM_SCO_CODEC_CVSD ||
            (codec == BTM_SCO_CODEC_MSBC && bta_hf_client_cb.msbc_enabled == TRUE)) {
        bta_hf_client_cb.scb.negotiated_codec = codec;
        bta_hf_client_send_at_bcs(codec);
    } else {
        bta_hf_client_cb.scb.negotiated_codec = BTM_SCO_CODEC_CVSD;
        bta_hf_client_send_at_bac();
    }
}

static void bta_hf_client_handle_bsir(UINT32 provided)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, provided);

    bta_hf_client_evt_val(BTA_HF_CLIENT_BSIR_EVT, provided);
}

static void bta_hf_client_handle_cmeerror(UINT32 code)
{
    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_CME, code);
}

static void bta_hf_client_handle_vgm(UINT32 value)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, value);

    if (value <= BTA_HF_CLIENT_VGM_MAX) {
        bta_hf_client_evt_val(BTA_HF_CLIENT_MIC_EVT, value);
    }
}

static void bta_hf_client_handle_vgs(UINT32 value)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, value);

    if (value <= BTA_HF_CLIENT_VGS_MAX) {
        bta_hf_client_evt_val(BTA_HF_CLIENT_SPK_EVT, value);
    }
}

static void bta_hf_client_handle_bvra(UINT32 value)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, value);

    if (value > 1) {
        return;
    }

    bta_hf_client_evt_val(BTA_HF_CLIENT_VOICE_REC_EVT, value);
}

static void bta_hf_client_handle_clip(char *numstr, UINT32 type)
{
    APPL_TRACE_DEBUG("%s %u %s", __FUNCTION__, type, numstr);

    bta_hf_client_clip(numstr);
}

static void bta_hf_client_handle_ccwa(char *numstr, UINT32 type)
{
    APPL_TRACE_DEBUG("%s %u %s", __FUNCTION__, type, numstr);

    bta_hf_client_ccwa(numstr);
}

static void bta_hf_client_handle_cops(char *opstr, UINT32 mode)
{
    APPL_TRACE_DEBUG("%s %u %s", __FUNCTION__, mode, opstr);

    bta_hf_client_operator_name(opstr);
}

static void bta_hf_client_handle_binp(char *numstr)
{
    APPL_TRACE_DEBUG("%s %s", __FUNCTION__, numstr);

    bta_hf_client_binp(numstr);
}

static void bta_hf_client_handle_clcc(UINT16 idx, UINT16 dir, UINT16 status, UINT16 mode, UINT16 mpty, char *numstr, UINT16 type)
{
    APPL_TRACE_DEBUG("%s idx: %u dir: %u status: %u mode: %u mpty: %u",
                     __FUNCTION__, idx, dir, status, mode, mpty);

    if (numstr) {
        APPL_TRACE_DEBUG("%s number: %s  type: %u", __FUNCTION__, numstr, type);
    }

    bta_hf_client_clcc(idx, dir, status, mpty, numstr);
}

static void bta_hf_client_handle_cnum( char *numstr, UINT16 type, UINT16 service)
{
    APPL_TRACE_DEBUG("%s number: %s type: %u service: %u", __FUNCTION__, numstr, type, service);

    /* TODO: should number be modified according to type? */
    bta_hf_client_cnum(numstr, service);
}

static void bta_hf_client_handle_btrh( UINT16 code)
{
    APPL_TRACE_DEBUG("%s %u", __FUNCTION__, code);

    bta_hf_client_evt_val(BTA_HF_CLIENT_BTRH_EVT, code);
}

/******************************************************************************
**
**          COMMON AT EVENTS PARSING FUNCTIONS
**
*******************************************************************************/

/* Check if prefix match and skip spaces if any */
#define AT_CHECK_EVENT(buf, event) \
if (strncmp("\r\n"event,buf,sizeof("\r\n"event) - 1) != 0) \
    return buf; \
buf += sizeof("\r\n"event) - 1; \
while (*buf == ' ') buf++;

/* check for <cr><lf> and forward buffer if match */
#define AT_CHECK_RN(buf) \
    if (strncmp("\r\n", buf, sizeof("\r\n") - 1) != 0) { \
        APPL_TRACE_ERROR("%s missing end <cr><lf>", __FUNCTION__); \
        return NULL;\
    } \
    buf += sizeof("\r\n") - 1;

/* skip rest of AT string up to <cr> */
#define AT_SKIP_REST(buf) while(*buf != '\r') buf++;

static char *bta_hf_client_parse_ok(char *buffer)
{
    AT_CHECK_EVENT(buffer, "OK");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_ok();

    return buffer;
}

static char *bta_hf_client_parse_error(char *buffer)
{
    AT_CHECK_EVENT(buffer, "ERROR");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_ERROR, 0);

    return buffer;
}

static char *bta_hf_client_parse_ring(char *buffer)
{
    AT_CHECK_EVENT(buffer, "RING");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_ring();

    return buffer;
}

/* generic uint32 parser */
static char *bta_hf_client_parse_uint32(char *buffer, void (*handler_callback)(UINT32))
{
    UINT32 value;
    int res;
    int offset;

    res = sscanf(buffer, "%u%n", &value, &offset);
    if (res < 1) {
        return NULL;
    }

    buffer += offset;

    AT_CHECK_RN(buffer);

    handler_callback(value);
    return buffer;
}

static char *bta_hf_client_parse_brsf(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+BRSF:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_brsf);
}

static char *bta_hf_client_parse_cind_values(char *buffer)
{
    /* value and its position */
    UINT16 index = 0;
    UINT32 value = 0;

    int offset;
    int res;

    while ((res = sscanf(buffer, "%u%n", &value, &offset)) > 0) {
        /* decides if its valid index and value, if yes stores it */
        bta_hf_client_handle_cind_value(index, value);

        buffer += offset;

        /* check if more values are present */
        if (*buffer != ',') {
            break;
        }

        index++;
        buffer++;
    }

    if (res > 0) {
        AT_CHECK_RN(buffer);
        return buffer;
    }

    return NULL;
}

static char *bta_hf_client_parse_cind_list(char *buffer)
{
    int offset;
    char *name = osi_malloc(129);
    UINT32 min, max;
    UINT32 index = 0;
    int res;

    if (name == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return NULL;
    }

    while ((res = sscanf(buffer, "(\"%128[^\"]\",(%u%*[-,]%u))%n", name, &min, &max, &offset)) > 2) {
        bta_hf_client_handle_cind_list_item(name, min, max, index);
        buffer += offset;
        index++;

        if (*buffer != ',') {
            break;
        }

        buffer++;
    }

    osi_free(name);

    if (res > 2) {
        AT_CHECK_RN(buffer);
        return buffer;
    }

    return NULL;
}

static char *bta_hf_client_parse_cind(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+CIND:");

    if (*buffer == '(') {
        return bta_hf_client_parse_cind_list(buffer);
    }

    return bta_hf_client_parse_cind_values(buffer);
}

static char *bta_hf_client_parse_chld(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+CHLD:");

    if (*buffer != '(') {
        return NULL;
    }

    buffer++;

    while (*buffer != '\0') {
        if (strncmp("0", buffer, 1) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_REL);
            buffer++;
        } else if (strncmp("1x", buffer, 2) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_REL_X);
            buffer += 2;
        } else if (strncmp("1", buffer, 1) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_REL_ACC);
            buffer++;
        } else if (strncmp("2x", buffer, 2) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_PRIV_X);
            buffer += 2;
        } else if (strncmp("2", buffer, 1) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_HOLD_ACC);
            buffer++;
        } else if (strncmp("3", buffer, 1) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_MERGE);
            buffer++;
        } else if (strncmp("4", buffer, 1) == 0) {
            bta_hf_client_handle_chld(BTA_HF_CLIENT_CHLD_MERGE_DETACH);
            buffer++;
        } else {
            return NULL;
        }

        if (*buffer == ',') {
            buffer++;
            continue;
        }

        if (*buffer == ')') {
            buffer++;
            break;
        }

        return NULL;
    }

    AT_CHECK_RN(buffer);

    return buffer;
}

static char *bta_hf_client_parse_ciev(char *buffer)
{
    UINT32 index, value;
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+CIEV:");

    res = sscanf(buffer, "%u,%u%n", &index, &value, &offset);
    if (res < 2) {
        return NULL;
    }

    buffer += offset;

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_ciev(index, value);
    return buffer;
}

static char *bta_hf_client_parse_bcs(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+BCS:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_bcs);
}

static char *bta_hf_client_parse_bsir(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+BSIR:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_bsir);
}

static char *bta_hf_client_parse_cmeerror(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+CME ERROR:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_cmeerror);
}

static char *bta_hf_client_parse_vgm(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+VGM:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_vgm);
}

static char *bta_hf_client_parse_vgme(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+VGM=");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_vgm);
}

static char *bta_hf_client_parse_vgs(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+VGS:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_vgs);
}

static char *bta_hf_client_parse_vgse(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+VGS=");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_vgs);
}

static char *bta_hf_client_parse_bvra(char *buffer)
{
    AT_CHECK_EVENT(buffer, "+BVRA:");

    return bta_hf_client_parse_uint32(buffer, bta_hf_client_handle_bvra);
}

static char *bta_hf_client_parse_clip(char *buffer)
{
    /* spec forces 32 chars, plus \0 here */
    char number[33];
    UINT32 type = 0;
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+CLIP:");

    /* there might be something more after %lu but HFP doesn't care */
    res = sscanf(buffer, "\"%32[^\"]\",%u%n", number, &type, &offset);
    if (res < 2) {
        return NULL;
    }

    buffer += offset;

    AT_SKIP_REST(buffer);

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_clip(number, type);
    return buffer;
}

/* in HFP context there is no difference between ccwa and clip */
static char *bta_hf_client_parse_ccwa(char *buffer)
{
    /* ac to spec 32 chars max, plus \0 here */
    char number[33];
    UINT32 type = 0;
    int res ;
    int offset;

    AT_CHECK_EVENT(buffer, "+CCWA:");

    /* there might be something more after %lu but HFP doesn't care */
    res = sscanf(buffer, "\"%32[^\"]\",%u%n", number, &type, &offset);
    if (res < 2) {
        return NULL;
    }

    buffer += offset;

    AT_SKIP_REST(buffer);

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_ccwa(number, type);
    return buffer;
}

static char *bta_hf_client_parse_cops(char *buffer)
{
    UINT8 mode;
    /* spec forces 16 chars max, plus \0 here */
    char opstr[17];
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+COPS:");

    /* TODO: Not sure if operator string actually can contain escaped " char inside */
    res = sscanf(buffer, "%hhi,0,\"%16[^\"]\"%n", &mode, opstr, &offset);
    if (res < 2) {
        return NULL;
    }

    buffer += offset;

    AT_SKIP_REST(buffer);

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_cops(opstr, mode);
    return buffer;
}

static char *bta_hf_client_parse_binp(char *buffer)
{
    /* HFP only supports phone number as BINP data */
    /* phone number is 32 chars plus one for \0*/
    char numstr[33];
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+BINP:");

    res = sscanf(buffer, "\"%32[^\"]\"\r\n%n", numstr, &offset);
    if (res < 1) {
        return NULL;
    }

    buffer += offset;

    /* some phones might sent type as well, just skip it */
    AT_SKIP_REST(buffer);

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_binp(numstr);
    return buffer;
}

static char *bta_hf_client_parse_clcc(char *buffer)
{
    UINT16 idx, dir, status, mode, mpty;
    char numstr[33];     /* spec forces 32 chars, plus one for \0*/
    UINT16 type;
    int res;
    int offset;
    AT_CHECK_EVENT(buffer, "+CLCC:");

    res = sscanf(buffer, "%hu,%hu,%hu,%hu,%hu%n",
                 &idx, &dir, &status, &mode, &mpty, &offset);
    if (res < 5) {
        return NULL;
    }

    /* Abort in case offset not set because of format error */
    if (offset == 0) {
        APPL_TRACE_ERROR("%s: Format Error %s", __func__, buffer);
        return NULL;
    }

    buffer += offset;
    offset = 0;

    /* check optional part */
    if (*buffer == ',') {
        int res2 = sscanf(buffer, ",\"%32[^\"]\",%hu%n", numstr, &type, &offset);
        if (res2 < 0) {
            return NULL;
        }

        if (res2 == 0) {
            res2 = sscanf(buffer, ",\"\",%hu%n", &type, &offset);
            if (res2 < 0) {
                return NULL;
            }

            /* numstr is not matched in second attempt, correct this */
            res2++;
            numstr[0] = '\0';
        }

        if (res2 >= 2) {
            res += res2;
            /* Abort in case offset not set because of format error */
            if (offset == 0) {
                APPL_TRACE_ERROR("%s: Format Error %s", __func__, buffer);
                return NULL;
            }

            buffer += offset;
        }
    }

    /* Skip any remaing param,as they are not defined by BT HFP spec */
    AT_SKIP_REST(buffer);
    AT_CHECK_RN(buffer);

    if (res > 6) {
        /* we also have last two optional parameters */
        bta_hf_client_handle_clcc(idx, dir, status, mode, mpty, numstr, type);
    } else {
        /* we didn't get the last two parameters */
        bta_hf_client_handle_clcc(idx, dir, status, mode, mpty, NULL, 0);
    }

    return buffer;
}

static char *bta_hf_client_parse_cnum(char *buffer)
{
    char numstr[33];     /* spec forces 32 chars, plus one for \0*/
    UINT16 type;
    UINT16 service = 0; /* 0 in case this optional parameter is not being sent */
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+CNUM:");

    res = sscanf(buffer, ",\"%32[^\"]\",%hu%n,,%hu%n", numstr, &type, &offset, &service, &offset);
    if (res < 0) {
        return NULL;
    }

    if (res == 0) {
        res = sscanf(buffer, ",\"\",%hu%n,,%hu%n", &type, &offset, &service, &offset);
        if (res < 0) {
            return NULL;
        }

        /* numstr is not matched in second attempt, correct this */
        res++;
        numstr[0] = '\0';
    }

    if (res < 2) {
        return NULL;
    }

    buffer += offset;

    AT_CHECK_RN(buffer);

    /* service is optional */
    if (res == 2) {
        bta_hf_client_handle_cnum(numstr, type, service);
        return buffer;
    }

    if (service != 4 && service != 5) {
        return NULL;
    }

    bta_hf_client_handle_cnum(numstr, type, service);
    return buffer;
}

static char *bta_hf_client_parse_btrh(char *buffer)
{
    UINT16 code = 0;
    int res;
    int offset;

    AT_CHECK_EVENT(buffer, "+BTRH:");

    res = sscanf(buffer, "%hu%n", &code, &offset);
    if (res < 1) {
        return NULL;
    }

    buffer += offset;

    AT_CHECK_RN(buffer);

    bta_hf_client_handle_btrh(code);
    return buffer;
}

static char *bta_hf_client_parse_busy(char *buffer)
{
    AT_CHECK_EVENT(buffer, "BUSY");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_BUSY, 0);

    return buffer;
}

static char *bta_hf_client_parse_delayed(char *buffer)
{
    AT_CHECK_EVENT(buffer, "DELAYED");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_DELAY, 0);

    return buffer;
}

static char *bta_hf_client_parse_no_carrier(char *buffer)
{
    AT_CHECK_EVENT(buffer, "NO CARRIER");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_NO_CARRIER, 0);

    return buffer;
}

static char *bta_hf_client_parse_no_answer(char *buffer)
{
    AT_CHECK_EVENT(buffer, "NO ANSWER");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_NO_ANSWER, 0);

    return buffer;
}

static char *bta_hf_client_parse_blacklisted(char *buffer)
{
    AT_CHECK_EVENT(buffer, "BLACKLISTED");
    AT_CHECK_RN(buffer);

    bta_hf_client_handle_error(BTA_HF_CLIENT_AT_RESULT_BLACKLISTED, 0);

    return buffer;
}

static char *bta_hf_client_skip_unknown(char *buffer)
{
    char *start;
    char *tmp;

    tmp = strstr(buffer, "\r\n");
    if (tmp == NULL) {
        return NULL;
    }

    buffer += 2;
    start = buffer;

    tmp = strstr(buffer, "\r\n");
    if (tmp == NULL) {
        return NULL;
    }

    buffer = tmp + 2;

    APPL_TRACE_DEBUG("%s %.*s", __FUNCTION__, buffer - start - 2, start);
    UNUSED(start);

    return buffer;
}


/******************************************************************************
**       SUPPORTED EVENT MESSAGES
*******************************************************************************/

/* returned values are as follow:
 * != NULL && != buf  : match and parsed ok
 * == NULL            : match but parse failed
 * != NULL && == buf  : no match
 */
typedef char *(*tBTA_HF_CLIENT_PARSER_CALLBACK)(char *);

static const tBTA_HF_CLIENT_PARSER_CALLBACK bta_hf_client_parser_cb[] = {
    bta_hf_client_parse_ok,
    bta_hf_client_parse_error,
    bta_hf_client_parse_ring,
    bta_hf_client_parse_brsf,
    bta_hf_client_parse_cind,
    bta_hf_client_parse_ciev,
    bta_hf_client_parse_chld,
    bta_hf_client_parse_bcs,
    bta_hf_client_parse_bsir,
    bta_hf_client_parse_cmeerror,
    bta_hf_client_parse_vgm,
    bta_hf_client_parse_vgme,
    bta_hf_client_parse_vgs,
    bta_hf_client_parse_vgse,
    bta_hf_client_parse_bvra,
    bta_hf_client_parse_clip,
    bta_hf_client_parse_ccwa,
    bta_hf_client_parse_cops,
    bta_hf_client_parse_binp,
    bta_hf_client_parse_clcc,
    bta_hf_client_parse_cnum,
    bta_hf_client_parse_btrh,
    bta_hf_client_parse_busy,
    bta_hf_client_parse_delayed,
    bta_hf_client_parse_no_carrier,
    bta_hf_client_parse_no_answer,
    bta_hf_client_parse_blacklisted,
    bta_hf_client_skip_unknown
};

/* calculate supported event list length */
static const UINT16 bta_hf_client_psraser_cb_count =
    sizeof(bta_hf_client_parser_cb) / sizeof(bta_hf_client_parser_cb[0]);

#ifdef BTA_HF_CLIENT_AT_DUMP
static void bta_hf_client_dump_at(void)
{
    char dump[(4 * BTA_HF_CLIENT_AT_PARSER_MAX_LEN) + 1];
    char *p1, *p2;

    p1 = bta_hf_client_cb.scb.at_cb.buf;
    p2 = dump;

    while (*p1 != '\0') {
        if (*p1 == '\r') {
            strlcpy(p2, "<cr>", 4);
            p2 += 4;
        } else if (*p1 == '\n') {
            strlcpy(p2, "<lf>", 4);
            p2 += 4;
        } else {
            *p2 = *p1;
            p2++;
        }
        p1++;
    }

    *p2 = '\0';

    APPL_TRACE_DEBUG("%s %s", __FUNCTION__, dump);
}
#endif

static void bta_hf_client_at_parse_start(void)
{
    char *buf = bta_hf_client_cb.scb.at_cb.buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

#ifdef BTA_HF_CLIENT_AT_DUMP
    bta_hf_client_dump_at();
#endif

    while (*buf != '\0') {
        int i;
        char *tmp = NULL;

        for (i = 0; i < bta_hf_client_psraser_cb_count; i++) {
            tmp = bta_hf_client_parser_cb[i](buf);
            if (tmp == NULL) {
                APPL_TRACE_ERROR("HFPCient: AT event/reply parsing failed, skipping %d", i);
                tmp = bta_hf_client_skip_unknown(buf);
                break;
            }

            /* matched or unknown skipped, if unknown failed tmp is NULL so
               this is also handled */
            if (tmp != buf) {
                buf = tmp;
                break;
            }
        }

        /* could not skip unknown (received garbage?)... disconnect */
        if (tmp == NULL) {
            APPL_TRACE_ERROR("HFPCient: could not skip unknown AT event, disconnecting");
            bta_hf_client_at_reset();
            bta_hf_client_sm_execute(BTA_HF_CLIENT_API_CLOSE_EVT, NULL);
            return;
        }

        buf = tmp;
    }
}

static BOOLEAN bta_hf_client_check_at_complete(void)
{
    BOOLEAN ret = FALSE;
    tBTA_HF_CLIENT_AT_CB *at_cb = &bta_hf_client_cb.scb.at_cb;

    if (at_cb->offset >= BTA_HF_CLIENT_AT_EVENT_MIN_LEN) {
        if (at_cb->buf[at_cb->offset - 2] == '\r' && at_cb->buf[at_cb->offset - 1] == '\n') {
            ret = TRUE;
        }
    }

    APPL_TRACE_DEBUG("%s %d", __FUNCTION__, ret);

    return ret;
}

static void bta_hf_client_at_clear_buf(void)
{
    memset(bta_hf_client_cb.scb.at_cb.buf, 0, sizeof(bta_hf_client_cb.scb.at_cb.buf));
    bta_hf_client_cb.scb.at_cb.offset = 0;
}

/******************************************************************************
**
**          MAIN PARSING FUNCTION
**
**
*******************************************************************************/
void bta_hf_client_at_parse(char *buf, unsigned int len)
{
    APPL_TRACE_DEBUG("%s offset: %u len: %u", __FUNCTION__, bta_hf_client_cb.scb.at_cb.offset, len);

    if (len + bta_hf_client_cb.scb.at_cb.offset > BTA_HF_CLIENT_AT_PARSER_MAX_LEN) {
        unsigned int tmp = bta_hf_client_cb.scb.at_cb.offset;
        unsigned int space_left = BTA_HF_CLIENT_AT_PARSER_MAX_LEN - bta_hf_client_cb.scb.at_cb.offset;
        char *tmp_buff = osi_malloc(BTA_HF_CLIENT_AT_PARSER_MAX_LEN);
        if (tmp_buff == NULL) {
            APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
            return;
        }
        APPL_TRACE_DEBUG("%s overrun, trying to recover", __FUNCTION__);

        /* fill up parser buffer */
        memcpy(bta_hf_client_cb.scb.at_cb.buf + bta_hf_client_cb.scb.at_cb.offset, buf, space_left);
        len -= space_left;
        buf += space_left;
        bta_hf_client_cb.scb.at_cb.offset += space_left;

        /* find end of last complete command before proceeding */
        while (bta_hf_client_check_at_complete() == FALSE) {
            if (bta_hf_client_cb.scb.at_cb.offset == 0) {
                APPL_TRACE_ERROR("HFPClient: AT parser buffer overrun, disconnecting");

                bta_hf_client_at_reset();
                bta_hf_client_sm_execute(BTA_HF_CLIENT_API_CLOSE_EVT, NULL);
                osi_free(tmp_buff);
                return;
            }

            bta_hf_client_cb.scb.at_cb.offset--;
        }

        /* cut buffer to complete AT event and keep cut data */
        tmp += space_left - bta_hf_client_cb.scb.at_cb.offset;
        memcpy(tmp_buff, bta_hf_client_cb.scb.at_cb.buf + bta_hf_client_cb.scb.at_cb.offset, tmp);
        bta_hf_client_cb.scb.at_cb.buf[bta_hf_client_cb.scb.at_cb.offset] = '\0';

        /* parse */
        bta_hf_client_at_parse_start();
        bta_hf_client_at_clear_buf();

        /* recover cut data */
        memcpy(bta_hf_client_cb.scb.at_cb.buf, tmp_buff, tmp);
        bta_hf_client_cb.scb.at_cb.offset += tmp;

        osi_free(tmp_buff);
    }

    memcpy(bta_hf_client_cb.scb.at_cb.buf + bta_hf_client_cb.scb.at_cb.offset, buf, len);
    bta_hf_client_cb.scb.at_cb.offset += len;

    /* If last event is complete, parsing can be started */
    if (bta_hf_client_check_at_complete() == TRUE) {
        bta_hf_client_at_parse_start();
        bta_hf_client_at_clear_buf();
    }
}

void bta_hf_client_send_at_brsf(void)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BRSF=%u\r", bta_hf_client_cb.scb.features);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BRSF , buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_bac(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (bta_hf_client_cb.msbc_enabled) {
        buf = "AT+BAC=1,2\r";
    } else {
        buf = "AT+BAC=1\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BAC, buf, strlen(buf));
}

void bta_hf_client_send_at_bcs(UINT32 codec)
{
    char * buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BCS=%u\r", codec);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BCS, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_cind(BOOLEAN status)
{
    char *buf;
    tBTA_HF_CLIENT_AT_CMD cmd;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (status) {
        buf = "AT+CIND?\r";
        cmd = BTA_HF_CLIENT_AT_CIND_STATUS;
    } else {
        buf = "AT+CIND=?\r";
        cmd = BTA_HF_CLIENT_AT_CIND;
    }

    bta_hf_client_send_at(cmd, buf, strlen(buf));
}

void bta_hf_client_send_at_cmer(BOOLEAN activate)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (activate) {
        buf = "AT+CMER=3,0,0,1\r";
    } else {
        buf = "AT+CMER=3,0,0,0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CMER, buf, strlen(buf));
}

void bta_hf_client_send_at_chld(char cmd, UINT32 idx)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }

    if (idx > 0) {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+CHLD=%c%u\r", cmd, idx);
    } else {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+CHLD=%c\r", cmd);
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CHLD, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_clip(BOOLEAN activate)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (activate) {
        buf = "AT+CLIP=1\r";
    } else {
        buf = "AT+CLIP=0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CLIP, buf, strlen(buf));
}

void bta_hf_client_send_at_ccwa(BOOLEAN activate)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (activate) {
        buf = "AT+CCWA=1\r";
    } else {
        buf = "AT+CCWA=0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CCWA, buf, strlen(buf));
}


void bta_hf_client_send_at_cmee(BOOLEAN activate)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (activate) {
        buf = "AT+CMEE=1\r";
    } else {
        buf = "AT+CMEE=0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CMEE, buf, strlen(buf));
}

void bta_hf_client_send_at_cops(BOOLEAN query)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (query) {
        buf = "AT+COPS?\r";
    } else {
        buf = "AT+COPS=3,0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_COPS, buf, strlen(buf));
}

void bta_hf_client_send_at_clcc(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "AT+CLCC\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CLCC, buf, strlen(buf));
}

void bta_hf_client_send_at_bvra(BOOLEAN enable)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (enable) {
        buf = "AT+BVRA=1\r";
    } else {
        buf = "AT+BVRA=0\r";
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BVRA, buf, strlen(buf));
}

void bta_hf_client_send_at_vgs(UINT32 volume)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;
    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+VGS=%u\r", volume);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_VGS, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_vgm(UINT32 volume)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+VGM=%u\r", volume);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_VGM, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_atd(char *number, UINT32 memory)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (number[0] != '\0') {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "ATD%s;\r", number);
    } else {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "ATD>%u;\r", memory);
    }

    at_len = MIN(at_len, BTA_HF_CLIENT_AT_MAX_LEN);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_ATD, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_bldn(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "AT+BLDN\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BLDN, buf, strlen(buf));
}

void bta_hf_client_send_at_ata(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "ATA\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_ATA, buf, strlen(buf));
}

void bta_hf_client_send_at_chup(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "AT+CHUP\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CHUP, buf, strlen(buf));
}

void bta_hf_client_send_at_btrh(BOOLEAN query, UINT32 val)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (query == TRUE) {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BTRH?\r");
    } else {
        at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BTRH=%u\r", val);
    }

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BTRH, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_vts(char code)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;
    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+VTS=%c\r", code);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_VTS, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_bcc(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "AT+BCC\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BCC, buf, strlen(buf));
}

void bta_hf_client_send_at_cnum(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    buf = "AT+CNUM\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_CNUM, buf, strlen(buf));
}

void bta_hf_client_send_at_nrec(void)
{
    char *buf;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);

    if (!(bta_hf_client_cb.scb.peer_features & BTA_HF_CLIENT_PEER_FEAT_ECNR)) {
        APPL_TRACE_ERROR("%s: Remote does not support NREC.", __FUNCTION__);
        return;
    }

    buf = "AT+NREC=0\r";

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_NREC, buf, strlen(buf));
}

void bta_hf_client_send_at_binp(UINT32 action)
{
    char *buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    int at_len;

    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }

    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BINP=%u\r", action);

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BINP, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_send_at_bia(void)
{
    char *buf;
    int at_len;
    int i;

    APPL_TRACE_DEBUG("%s", __FUNCTION__);
    if (bta_hf_client_cb.scb.peer_version < HFP_VERSION_1_6) {
        APPL_TRACE_DEBUG("Remote does not Support AT+BIA");
        return;
    }

    buf = osi_malloc(BTA_HF_CLIENT_AT_MAX_LEN);
    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }
    at_len = snprintf(buf, BTA_HF_CLIENT_AT_MAX_LEN, "AT+BIA=");

    for (i = 0; i < BTA_HF_CLIENT_AT_INDICATOR_COUNT; i++) {
        int sup = bta_hf_client_cb.scb.at_cb.indicator_lookup[i] == -1 ? 0 : 1;

        at_len += snprintf(buf + at_len, BTA_HF_CLIENT_AT_MAX_LEN - at_len, "%u,", sup);
    }

    buf[at_len - 1] = '\r';

    bta_hf_client_send_at(BTA_HF_CLIENT_AT_BIA, buf, at_len);
    osi_free(buf);
}

void bta_hf_client_at_init(void)
{
    memset(&bta_hf_client_cb.scb.at_cb, 0, sizeof(tBTA_HF_CLIENT_AT_CB));
    bta_hf_client_at_reset();
}

void bta_hf_client_at_reset(void)
{
    int i;

    bta_hf_client_free_at_resp_timer();
    bta_hf_client_free_at_hold_timer();

    bta_hf_client_clear_queued_at();

    bta_hf_client_at_clear_buf();

    for (i = 0; i < BTA_HF_CLIENT_AT_INDICATOR_COUNT; i++) {
        bta_hf_client_cb.scb.at_cb.indicator_lookup[i] = -1;
    }

    bta_hf_client_cb.scb.at_cb.current_cmd = BTA_HF_CLIENT_AT_NONE;
}
#endif /* #if (BTA_HF_INCLUDED == TRUE) */
