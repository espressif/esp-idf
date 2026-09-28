/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_defs.h"
#include "common/bt_target.h"
#include "stack/sdp_api.h"
#include "bta_opp_int.h"

#if BTA_OPP_CLIENT_INCLUDED

static void bta_opp_client_sdp_cback(UINT16 status, void *user_data)
{
    tBTA_OPP_CLIENT_DISC_RESULT *p_buf;
    tBTA_OPP_CCB *p_ccb = (tBTA_OPP_CCB *)user_data;

    if (p_ccb == NULL || p_ccb->allocated == 0) {
        return;
    }

    if ((p_buf = (tBTA_OPP_CLIENT_DISC_RESULT *)osi_malloc(sizeof(tBTA_OPP_CLIENT_DISC_RESULT))) != NULL) {
        p_buf->hdr.event = BTA_OPP_CLIENT_DISC_RES_EVT;
        p_buf->hdr.layer_specific = p_ccb->allocated;
        p_buf->status = status;
        bta_sys_sendmsg(p_buf);
    } else {
        /* Cannot deliver DISC_RES; tear down so the client does not hang in OPENING. */
        bta_opp_client_free_sdp_db(p_ccb);
        bta_opp_close_connection(p_ccb, BTA_OPP_NO_RESOURCE, TRUE);
    }
}

BOOLEAN bta_opp_client_sdp_find_attr(tBTA_OPP_CCB *p_ccb)
{
    tSDP_DISC_REC *p_rec = NULL;
    tSDP_DISC_ATTR *p_attr;
    tSDP_PROTOCOL_ELEM pe;

    p_ccb->peer_rfcomm_scn = 0;
    p_ccb->peer_l2cap_psm = 0;

    while ((p_rec = SDP_FindServiceInDb(p_ccb->p_disc_db, UUID_SERVCLASS_OBEX_OBJECT_PUSH, p_rec)) != NULL) {
        if (!SDP_FindProtocolListElemInRec(p_rec, UUID_PROTOCOL_RFCOMM, &pe)) {
            continue;
        }
        p_ccb->peer_rfcomm_scn = (UINT8)pe.params[0];
        if ((p_attr = SDP_FindAttributeInRec(p_rec, ATTR_ID_GOEP_L2CAP_PSM)) != NULL) {
            p_ccb->peer_l2cap_psm = p_attr->attr_value.v.u16;
        }
        return TRUE;
    }

    return FALSE;
}

BOOLEAN bta_opp_client_do_disc(tBTA_OPP_CCB *p_ccb)
{
    tSDP_UUID uuid_list[1];
    UINT16 attr_list[3];
    BOOLEAN db_inited = FALSE;

    attr_list[0] = ATTR_ID_SERVICE_CLASS_ID_LIST;
    attr_list[1] = ATTR_ID_PROTOCOL_DESC_LIST;
    attr_list[2] = ATTR_ID_GOEP_L2CAP_PSM;
    uuid_list[0].uu.uuid16 = UUID_SERVCLASS_OBEX_OBJECT_PUSH;
    uuid_list[0].len = LEN_UUID_16;

    if (p_ccb->p_disc_db != NULL) {
        return FALSE;
    }

    p_ccb->p_disc_db = (tSDP_DISCOVERY_DB *)osi_malloc(BT_DEFAULT_BUFFER_SIZE);
    if (p_ccb->p_disc_db != NULL) {
        db_inited = SDP_InitDiscoveryDb(p_ccb->p_disc_db, BT_DEFAULT_BUFFER_SIZE, 1, uuid_list, 3, attr_list);
    }

    if (db_inited) {
        db_inited = SDP_ServiceSearchAttributeRequest2(p_ccb->bd_addr, p_ccb->p_disc_db,
                                                      bta_opp_client_sdp_cback, p_ccb);
    }

    if (!db_inited) {
        bta_opp_client_free_sdp_db(p_ccb);
        return FALSE;
    }
    return TRUE;
}

void bta_opp_client_free_sdp_db(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb->p_disc_db != NULL) {
        /* Cancel first: SDP CCB keeps p_db and may still write into it. */
        SDP_CancelServiceSearch(p_ccb->p_disc_db);
        osi_free(p_ccb->p_disc_db);
        p_ccb->p_disc_db = NULL;
    }
}

#endif /* BTA_OPP_CLIENT_INCLUDED */
